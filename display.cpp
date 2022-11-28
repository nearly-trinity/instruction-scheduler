#include <iostream>
#include "scanner.h"
#include <unordered_map>
#include <string>

using namespace std;

unordered_map<Instructions, string, hash<int>> enumMap;

void printHelp() {
    cout << "Help:" << endl;
    cout << "-t                Prints a list of the tokens, one per line." << endl;
    cout << "-p                Prints legal ILOC code."<< endl;
    cout << "-h                Prints a help summary and exit."<< endl;
    exit(1);
}

void printOpcode(OpInst token) {
    string category;
    string lexeme;
    switch (token.op) {
        case store:
            lexeme = "store  ";
            break;
        case sub:
            lexeme = "sub    ";
            break;
        case loadI:
            lexeme = "loadI  ";
            break;
        case load:
            lexeme = "load   ";
            break;
        case lshift:
            lexeme = "lshift ";
            break;
        case rshift:
            lexeme = "rshift ";
            break;
        case mult:
            lexeme = "mult   ";
            break;
        case add:
            lexeme = "add    ";
            break;
        case nop:
            lexeme = "nop    ";
            break;
        case output:
            lexeme = "output ";
            break;
        default:
            lexeme = "error  ";
            break;
    }
    cout << lexeme;
}

void printILOCLine(Inst &iloc, int regFlag) {
    cout << "    n" << iloc.label << ": ";
    switch (iloc.opcode.cat) {
    case ARITHOP:
        printOpcode(iloc.opcode);
        iloc.op1.display(regFlag);
        cout << ", ";
        iloc.op2.display(regFlag);
        cout << " => ";
        iloc.dest.display(regFlag);
        break;
    case LOADI:
    case MEMOP:
        printOpcode(iloc.opcode);
        iloc.op1.display(regFlag);
        cout << " => ";
        if (iloc.opcode.op == store)
        {
            iloc.op2.display(regFlag);
        }
        else
        {
            iloc.dest.display(regFlag);
        }
        break;
    case NOP:
        printOpcode(iloc.opcode);
        break;
    case OUTPUT:
        printOpcode(iloc.opcode);
        iloc.op1.display(regFlag);
        break;
    default:
        cout << "error" << endl;
    }
    cout << endl;
}


void Inst::tablePrint() {
    int ind = idx;
    cout << "//|" << setw(4) << ind;
    cout << " |";
    printOpcode(opcode);
    op1.tablePrint();
    op2.tablePrint();
    dest.tablePrint();
    cout << endl;
}

void tabularILOC(vector<Inst>  &ilocLines)
{
    cout << "//|index|opcode ||op1                ||op2                ||dest" << endl;
    cout << "//              || SR | VR | PR | NU || SR | VR | PR | NU || SR | VR | PR | NU"<< endl;
    for (auto &elem : ilocLines)
    {
        elem.tablePrint();
    }
}


void printILOC(vector<Inst> &ilocLines, int flag) {
    for(auto row : ilocLines) {
        printILOCLine(row, flag);
    }
}
