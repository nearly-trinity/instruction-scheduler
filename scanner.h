#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <climits>
#include <iomanip>

enum Instructions
{
    load,
    loadI,
    store,
    add,
    sub,
    mult,
    lshift,
    rshift,
    output,
    nop
};
enum InstructionCategory
{
    MEMOP,
    LOADI,
    ARITHOP,
    NOP,
    OUTPUT,
    ERROR
};
enum TokenCategory
{
    INST,
    CONST,
    ARROW,
    REG,
    COMMA,
    COMMENT,
    DIGIT
};

void printHelp();

struct OpInst
{
    InstructionCategory cat;
    Instructions op;
};

struct Token
{
    TokenCategory cat;
    OpInst inst;
    int reg;
};

class Op
{
private:
    int padding = 4;

public:
    Op();
    int sr;
    int vr;
    int pr;
    int nextUse;
    int constVal;
    void display(int flag);
    void tablePrint();
};

class Inst
{
public:
    int idx;
    int label;
    OpInst opcode;
    Op op1;
    Op op2;
    Op dest;
    Inst(int line, int label, OpInst opcode, int o1, int o2, int d);
    void tablePrint();
};

void tabularILOC(std::vector<Inst> &ilocLines);
void printILOC(std::vector<Inst> &ilocLines, int flag);
std::vector<Inst> scanner(std::string filename);
int getMaxReg();
int getNumVR();
Inst readLine();
std::unordered_map<std::string, TokenCategory> setupInput();
std::vector<std::unordered_map<std::string, int>> setupTrans();
std::unordered_map<int, Token> setupToken();
void setupTables();

void printNodes(std::vector<Inst> &ilocLines);
void printEdges(std::vector<Inst> &ilocLines);
void printWeights(std::vector<Inst> &ilocLines);
