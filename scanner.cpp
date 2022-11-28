// scanner.cpp
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "scanner.h"
#include <vector>
#include <unordered_map>

using namespace std;

unordered_map<string,TokenCategory> inputTable;
vector<unordered_map<string,int>> transTable;
unordered_map<int,Token> tokenTable;
vector<Inst> internalRep;
std::ifstream in;
char c; int line_num;
int maxRegValue = -1;

Inst::Inst(int line, int lbl, OpInst opc, int o1, int o2, int d) {
    idx = line;
    label = lbl;
    opcode = opc;
    switch(opc.cat) {
        case LOADI:
        case OUTPUT:
            op1.constVal = o1;
            break;
        default:
            op1.sr = o1;
    }
    op2.sr = o2;
    dest.sr=d;
}

Op::Op() {
    sr = vr=pr=nextUse=constVal=-1;
}

void Op::display(int flag) {
    if(sr == -1) {
        if(constVal != -1) {
            cout << setw(padding+1) << constVal;
        }
        else {
            cout << setw(padding+1) << " ";
        }
        return;
    } 
    cout << "r";
    switch(flag) {
        case 0:
            cout << left << setw(padding) << sr;
            break;
        case 1:
            cout << left << setw(padding) << vr;
            break;
        case 2:
            cout << left << setw(padding) << pr;
            break;
    }
}

void Op::tablePrint() {
    if(sr == -1) {
        cout << "||"<<  left << setw(3+4*padding) << constVal;
        return;
    } else {
        cout << "||" << setw(padding) << sr;
        cout << "|"<< setw(padding) << vr;
        cout << "|"<< setw(padding) << pr;
        if(nextUse==INT_MAX) {
            cout << "|"<< setw(padding) << "inf";
        } else {
            cout << "|"<< setw(padding) << nextUse;
        }
    }
}

void setupTables() {
    inputTable = setupInput();
    transTable = setupTrans();
    tokenTable = setupToken();
}

void skipSpaces() {
    // cout << "in spaces is : " << (int)c << endl;
    while((c == ' ' || c == '\t') && c != EOF)
        in.get(c);
}

int handleNumber() {
    // cout << "in handle number: " << c << endl;
    int res = 0; 
    while(c != ' ' && c != '\n' && c != ',' && c != '=' && c != '\t') {
        res *= 10;
        res += (c-'0');
        in.get(c);
    }
    // cout << "end of handle number: " << c << " number is: " << res << endl; 
    return res;
}

void endLine() {
    while(c != '\n' && c != EOF && in.peek() != EOF)
        in.get(c);
    // in.get(c);
}

Token getToken() {
    skipSpaces();
    // cout << "getToken is " << c << endl;
    // in.get(c);
    string s(1,c);
    TokenCategory type = inputTable[s];
    switch(type) {
        case COMMENT:
            endLine();
            break;
        case REG: {
            in.get(c);
            // cout << "is reg: " << c << endl;
            Token tmp = {.cat=REG, .inst={NOP,nop}, handleNumber()};
            maxRegValue = max(tmp.reg, maxRegValue);
            // cout << "defined register" << endl;
            return tmp;
        }
        case COMMA:
            in.get(c);
            return {.cat=COMMA, .inst={NOP,nop}, -1};
        case ARROW:
            // cout << "is arrow: " << c << endl;
            in.get(c);
            if(c == '>') {
                in.get(c);
                return {.cat=ARROW, .inst={NOP,nop}, -1};
            } else {
                cerr << "error in getToken" << endl;
                exit(1);
            }
        case CONST:
            // cout << "is number " << endl;
            return {.cat=CONST, .inst={NOP,nop}, handleNumber()};
        default:
            break;
    }
}

vector<Token> handleRemainder() {
    vector<Token> res;
    skipSpaces();
    // cout << c << endl;
    while(c != '\n' && c != EOF) {
        res.push_back(getToken());
        skipSpaces();
        // cout << (int)c << endl;
    }
    return res;
}
bool isEnd = false;
Token getInstruction() {
    int state = 0;
    // in.get(c);
    while(c != EOF && in.peek() != EOF) {
        skipSpaces();
        // cout << "c is: " << c << endl;
        // cout << c << endl;
        // in.get(c);
        string s(1,c);
        if(transTable[state].find(s) != transTable[state].end()) {
            // has some connecting edge
            // cout << "edge is: " << c << endl;
            state = transTable[state][s];
            in.get(c);
            if(c == ' ' || c == '\t') {
                Token result = tokenTable[state];
                // cout << "reached accept state" << endl;
                return result;
            }
        } else if((c == '/' && in.peek() == '/') || (c == '\n')) {
            // cout << "called end line" << endl;
            endLine();
            in.get(c);
        } else {
            // no edge exists for this character
            cout << c << endl;
            cout << "error!!!" << endl;
            exit(1);
        }
    }
    isEnd = true;
}

Token emptyToken = {COMMENT, {NOP, nop}, -1};

void assertTrue(bool b, string message) {
    if(!b) {
        cout << "fatal error: " << message << endl;
        exit(1);
    }
}

void readLine(int line_num) {
    //ILOC line;
    skipSpaces();
    //cout << "read line is: " << c << endl;
    Token instruction = getInstruction();
    // cout << "end of getInstruction is " << (int)c << endl;
    vector<Token> tokens = handleRemainder();
    // cout << "remainder handled" << endl;
    if(!isEnd) {
    switch(instruction.inst.cat) {
        case MEMOP:
            assertTrue(tokens[0].cat == REG && tokens[1].cat == ARROW, "reading memop");
            if(instruction.inst.op == load)  
                internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, tokens[0].reg, -1, tokens[2].reg));
            else // if store
                internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, tokens[0].reg, tokens[2].reg, -1));
            break;
        case LOADI:
            internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, tokens[0].reg, -1, tokens[2].reg));
            break;
        case ARITHOP:
            internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, tokens[0].reg, tokens[2].reg, tokens[4].reg));
            break;
        case NOP:
            internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, -1,-1,-1));
            break;
        case OUTPUT:
            internalRep.push_back(Inst(line_num, line_num+1, instruction.inst, tokens[0].reg,-1,-1));
            break;
        default:
            cout << "SOMETHING VERY WRONG\n";
            break;
    }
    }
    //return line;
}

vector<Inst> scanner(string filename) {
    setupTables();
    line_num = 0;
    in.open(filename);
    in.get(c);
    if(in.fail()) {
        cerr << "unable to open file" << endl;
        exit(1);
    }
    while(in.good() && c != EOF && in.peek() != EOF) {
        readLine(line_num++);
        //cout << "Line number: " << line_num << " size " << internalRep.size() << endl;
        //tabularILOC(internalRep);
    }
    in.close();
    return internalRep;
}

int getMaxReg() {
    return maxRegValue;
}
