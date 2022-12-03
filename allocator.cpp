#include "allocator.h"
#include <vector>
#include <string>
#include <climits>
#include <unordered_map>
#include <algorithm>

using namespace std;

int debug = false;

int maxVR = -1;
struct tableRow
{
    int vrNumber;
    int lastUse;
};
std::vector<tableRow> SRtoVR;
int vrName;
void initTable(std::vector<tableRow> &table)
{
    int maxReg = getMaxReg();
    for (int i = 0; i <= maxReg; ++i)
    {
        tableRow row;
        row.vrNumber = -1;
        row.lastUse = INT_MAX;
        SRtoVR.push_back(row);
    }
}

void update(Op &op, int idx)
{
    if (SRtoVR[op.sr].vrNumber == -1)
    {
        SRtoVR[op.sr].vrNumber = vrName++;
    }
    op.vr = SRtoVR[op.sr].vrNumber;
    op.nextUse = SRtoVR[op.sr].lastUse;
    SRtoVR[op.sr].lastUse = idx;
    return;
}

void computeLastUse(std::vector<Inst> &block)
{
    vrName = 0;
    int n = block.size();
    initTable(SRtoVR);
    for (int i = n - 1; i >= 0; --i)
    {
        // cout << "at 1" << endl;
        Inst &line = block[i];
        InstructionCategory cat = line.opcode.cat;
        Instructions operation = line.opcode.op;
        switch (cat)
        {
        case MEMOP:
            if (operation == store)
            {
                // handle store
                // only last two lines
                update(line.op1, i);
                update(line.op2, i);
            }
            else
            {
                // handle load
                // first four lines
                update(line.dest, i);
                SRtoVR[line.dest.sr].vrNumber = -1;
                SRtoVR[line.dest.sr].lastUse = INT_MAX;
                update(line.op1, i);
            }
            break;
        case LOADI:
            // first three lines
            update(line.dest, i);
            SRtoVR[line.dest.sr].vrNumber = -1;
            SRtoVR[line.dest.sr].lastUse = INT_MAX;
            break;
        case ARITHOP:
            update(line.dest, i);
            SRtoVR[line.dest.sr].vrNumber = -1;
            SRtoVR[line.dest.sr].lastUse = INT_MAX;
            update(line.op1, i);
            update(line.op2, i);
            break;
        default:
            break;
        }
    }
}

std::vector<int> freeRegs;
std::vector<int> physReg;
std::vector<int> memLocs;
std::vector<int> nextUses;

void printTable()
{
    int padding = 4;
    // cout << " vr |  physReg | nextUse | memLoc | "<< endl;

    for (int i = 0; i < physReg.size(); i++)
    {
        if (physReg[i] != -1 || memLocs[i] != -1)
        {
            cout << "//| " << i << " | " << setw(padding) << physReg[i];
            if (nextUses[i] == INT_MAX)
            {
                cout << "|" << setw(padding) << "inf";
            }
            else
            {
                cout << "|" << setw(padding) << nextUses[i];
            }
            cout << " | " << setw(padding) << memLocs[i] << endl;
        }
    }
}

std::vector<Inst> resultILOC;
int loc;
int idx;
int n;
Token blankToken = {COMMENT, {NOP, nop}, -1};

int spill()
{
    int spilledVR = -999;
    int tmp_max = -999;
    // find the register with the furthest next use
    for (int j = 0; j < vrName; ++j)
    {
        // cout << "looking at vr:" << j << " nu: " << nextUses[j] << " pr: " << physReg[j] << endl;
        int use = nextUses[j];
        if (use > tmp_max)
        {
            // cout << "new latest use is: " << j << endl;
            tmp_max = use;
            spilledVR = j;
        }
    }
    int pr = physReg[spilledVR];

    Inst load_line(-2, -1,{LOADI, loadI}, loc, -1, 0);
    load_line.dest.pr = load_line.dest.sr;
    Inst store_line(-2, -1,{MEMOP, store}, pr, 0, -1);
    store_line.op1.pr = store_line.op1.sr;
    store_line.op2.pr = store_line.op2.sr;

    // push spilling ILOC into result vector
    resultILOC.push_back(load_line);
    resultILOC.push_back(store_line);
    physReg[spilledVR] = -1;
    nextUses[spilledVR] = -1;
    memLocs[spilledVR] = loc;
    loc += 4;
    // return the VR of the PR that was spilled
    return pr;
}

int retrieve(int vr)
{
    Inst loadI_line(-1, -1,{LOADI, loadI}, memLocs[vr], -1, 0);
    loadI_line.dest.pr = 0;
    int reg = fetchPR(vr);
    Inst load_line(-1, -1,{MEMOP, load}, 0, -1, reg);
    load_line.op1.pr = 0;
    load_line.dest.pr = reg;
    resultILOC.push_back(loadI_line);
    resultILOC.push_back(load_line);

    // return the register that we retrieve from memory to
    return reg;
}

int fetchPR(int vr)
{

    // for(auto num : freeRegs) cout << num << " ";
    // cout << "\n";
    int reg;
    // if there is an available register
    if (freeRegs.back() != 0)
    {
        // pop from the stack
        reg = freeRegs.back();
        freeRegs.pop_back();

        // assign the pr to the vr
        physReg[vr] = reg;

        // return the assigned pr value
        return reg;
    }
    else
    {
        // we have to spill a value to memory
        reg = spill();
        physReg[vr] = reg;

        // return the register that was spilled
        return reg;
    }
}

int assign(int vr)
{
    if (vr != -1)
    {
        // if the value is stored in memory
        if (physReg[vr] != -1)
        {
            // if the vr already has an assigned pr return it
            return physReg[vr];
        }
        else if (memLocs[vr] != -1)
        {
            // fetch value from memory
            int res = retrieve(vr);
            // assign the pr to that virtual reg
            physReg[vr] = res;
            // return the newly avail PR
            return res;
        }
        else
        {
            cout << "Something has gone very, veyr wrong" << endl;
        }
    }
    else
    {
        return -1;
    }
}

void free(Op &op)
{
    if (op.vr != -1)
    {
        if (op.nextUse == INT_MAX)
        {
            if (physReg[op.vr] == -1)
                cout << "THIS SHOULD NEVER HAPPEN HELP\n";
            // if there is no next use we will free the register
            freeRegs.push_back(physReg[op.vr]);
            physReg[op.vr] = -1;
            nextUses[op.vr] = -1;
        }
        else
        {
            nextUses[op.vr] = op.nextUse;
        }
    }
}

std::vector<Inst> allocate(std::vector<Inst> &block, int prs)
{
    n = block.size();
    loc = 32768;

    // initalize tables for storing vr data
    physReg = std::vector<int>(vrName, -1);
    memLocs = std::vector<int>(vrName, -1);
    nextUses = std::vector<int>(vrName, -1);

    // build stack of avail physical registers
    for (int i = 0; i < prs; ++i)
    {
        freeRegs.push_back(i);
    }

    // traverse the ILOC block
    idx = 0;
    for (auto line : block)
    {
        if (debug)
            line.tablePrint();

        // get a PR for op1.vr
        line.op1.pr = assign(line.op1.vr);
        line.op2.pr = assign(line.op2.vr);

        free(line.op1);
        free(line.op2);

        if (line.dest.vr != -1)
        {
            // get a PR for dest.vr and update table value
            fetchPR(line.dest.vr);
            line.dest.pr = physReg[line.dest.vr];
            // update next use for dest reg
            nextUses[line.dest.vr] = line.dest.nextUse;
            // free the register if not needed
            // free(line.dest);
        }

        resultILOC.push_back(line);
        if (debug)
            printTable();

        if (debug)
            line.tablePrint();

        ++idx;
    }
    return resultILOC;
}

void printEdges(std::vector<Inst> &block)
{

    std::cout << "\nreturning all the edges association" << std::endl;
    std::vector<std::vector<int>> output1(block.size()+2);
    std::vector<int> lookup(vrName+2,-1);
    for(auto line : block)
    {
        int line_num = line.idx;
        int label = line.label;

        switch(line.opcode.cat) {
            case ARITHOP: {
                int vr1 = line.op1.vr;
                int vr2 = line.op2.vr;
                int dest = line.dest.vr;

                if(lookup[vr1] == -1 or lookup[vr2] == -1) {
                    std::cout << " bad bad bad " << std::endl;
                    std::exit(1);
                }

                if(std::find(output1[label].begin(), output1[label].end(), lookup[vr1]) == output1[label].end())
                output1[label].push_back(lookup[vr1]);
                if(std::find(output1[label].begin(), output1[label].end(), lookup[vr2]) == output1[label].end())
                output1[label].push_back(lookup[vr2]);
                lookup[dest] = label;
            }
                break;

            case LOADI: {
                int dest = line.dest.vr;
                lookup[dest] = label;
            }
                break;
            case MEMOP: {
                // load is op1 dest
                if(line.opcode.op == load) {
                    int vr1 = line.op1.vr;
                    int dest = line.dest.vr;

                    if(lookup[vr1] == -1) {
                        std::cout << "bad bad bad" << std::endl;
                        std::exit(1);
                    }
                    if(std::find(output1[label].begin(), output1[label].end(), lookup[vr1]) == output1[label].end())
                    output1[label].push_back(lookup[vr1]);
                    lookup[dest] = label;

                } else { // store
                    int vr1 = line.op1.vr;
                    int vr2 = line.op2.vr;

                    if(lookup[vr1] == -1 or lookup[vr2] == -1) {
                        std::cout << " bad bad bad " << std::endl;
                        std::exit(1);
                    }
                    if(std::find(output1[label].begin(), output1[label].end(), lookup[vr1]) == output1[label].end())
                    output1[label].push_back(lookup[vr1]);
                    if(std::find(output1[label].begin(), output1[label].end(), lookup[vr2]) == output1[label].end())
                    output1[label].push_back(lookup[vr2]);
                }
                // store is op1 op2
            }
            default:
                break;
        }
    }

    //unordered_map<Inst,std::vector<int>> instLookup;
    //unordered_map<Instructions,vector<int>> instLookup;
    vector<int> storeLookup;
    vector<int> outputLookup;
    vector<int> loadLookup;
    //storeLookup.push_back(20);
    //outputLookup.push_back(21);
    //loadLookup.push_back(22);
    for(auto line: block){
      int label = line.label;
      //std::cout << label << endl;
      if(line.opcode.op == output || line.opcode.op == store || line.opcode.op == load){
      switch(line.opcode.op){
        case output:{

          if(outputLookup.size()>0)
          if(std::find(output1[label].begin(), output1[label].end(), outputLookup.back()) == output1[label].end()){
          output1[label].push_back(outputLookup.back());
          }

          if(storeLookup.size()>0){
          if(std::find(output1[label].begin(), output1[label].end(), storeLookup.back()) == output1[label].end()){
              int num = storeLookup.back();
              output1[label].push_back(num);

            }
          }
        }

        case store:{
          if(outputLookup.size()>0)
          if(std::find(output1[label].begin(), output1[label].end(), outputLookup.back()) == output1[label].end()){
          output1[label].push_back(outputLookup.back());
          std::cout << "C\t" << outputLookup.back() << endl;
          }
          for(int k = 0; k < loadLookup.size(); k++){
            if(std::find(output1[label].begin(), output1[label].end(), loadLookup[k]) == output1[label].end()){
              if((storeLookup.size() > 0 && storeLookup.back() <= loadLookup[k]) || storeLookup.size() == 0){
                output1[label].push_back(loadLookup[k]);
              }
            }
          }
        }
        case load:{
          if(storeLookup.size()>0){
          if(std::find(output1[label].begin(), output1[label].end(), storeLookup.back()) == output1[label].end()){
              output1[label].push_back(storeLookup.back());
            }
          }
        }
        default:
          break;
      }


    }


      if(line.opcode.op == output){
          outputLookup.push_back(label);
        }
        else if (line.opcode.op == store){
          storeLookup.push_back(label);
        }
        else if (line.opcode.op == load){
          loadLookup.push_back(label);
        }
    }

    for (int i = 1; i < output1.size()-1; ++i) {
        std::cout << "n" << i << " \{";
        sort(output1[i].begin(), output1[i].end());
        for(int j = 0; j < output1[i].size(); j++){
        int num = output1[i][j];
            std::cout << " n" << num;
            if(j + 1 < output1[i].size()){
              std::cout << ",";
            }
        }
        std::cout << " \} \n";
    }


    /*
    output: List[List[Int]] = [[]*number of lines]
    lookup: List[Int] = [-1]*vrName (number of vr's)
    for line in block:
        line_num, label, sr1,sr2 , dest = line's registers
        lookup[dest] = label

        # depends on what type of op it is (assuming arithop)
        if lookup[sr1] == -1 OR lookup[sr2] == -1:
            ERROR

        output[label].append(lookup[sr1])
        output[label].append(lookup[sr2])

    output: [
        [],
        [],
        [2],
        [3,4],
        [],
        [5,6]
    ]
    */


}
void printWeights(std::vector<Inst> &block)
{
    std::cout << "returning all the weighted edges (not needed for milestone)" << std::endl;
}
