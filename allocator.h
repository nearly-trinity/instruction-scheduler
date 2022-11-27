#include "scanner.h"
#include <vector>

int fetchPR(int vr);
void computeLastUse(std::vector<Inst> &block);
std::vector<Inst> allocate(std::vector<Inst> &block, int regs);