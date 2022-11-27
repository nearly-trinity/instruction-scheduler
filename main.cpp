#include <iostream>
#include "allocator.h"
#include <getopt.h>
#include <string>
#include <vector>

using namespace std;

vector<Inst> internals;

int main(int argc, char *argv[]) {
    bool help; bool print; bool tokens; 
    print = false;
    help = false;
    string file;
    int opt;
    int num_regs;
    int print_regs = 2;
    file = argv[1];
    while((opt = getopt(argc, argv, "hpt:k:")) != -1) {
        switch(opt) {
            case 'h':
                help = true;
                break;
            case 'p':
                print = true;
                break;
            case 't':
                print_regs = atoi(optarg);
                break;
            case 'k':
                num_regs = atoi(optarg);
                break;
            default:
                cout << "invalid flag/flag arguments" << endl;
                exit(1);
                break;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
    }
    file = argv[optind];
    if(help) {
        printHelp();
    } else {
        internals = scanner(file);
        computeLastUse(internals);

        std::vector<Inst> newRegs = allocate(internals, num_regs);
        if(print) {
            tabularILOC(newRegs);
                        }
        printILOC(newRegs, print_regs);
    }
    
}
