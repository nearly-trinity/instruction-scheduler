#include <iostream>
#include "allocator.h"
#include <getopt.h>
#include <string>
#include <vector>

using namespace std;

vector<Inst> internals;

int main(int argc, char *argv[])
{
    bool help;
    bool print;
    bool tokens;
    bool allocate_flag;
    print = false;
    help = false;
    string file;
    int opt;
    int num_regs;
    int print_regs = 2;
    file = argv[1];
    while ((opt = getopt(argc, argv, "hpt:k:")) != -1)
    {
        switch (opt)
        {
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
            allocate_flag = true;
            num_regs = atoi(optarg);
            break;

        default:
            cout << "invalid flag/flag arguments" << endl;
            exit(1);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "Expected argument after options\n");
    }
    file = argv[optind];
    if (help)
    {
        printHelp();
    }
    else if (allocate_flag)
    {
        internals = scanner(file);
        computeLastUse(internals);

        std::vector<Inst> newRegs = allocate(internals, num_regs);
        if (print)
        {
            tabularILOC(newRegs);
        }
        printILOC(newRegs, print_regs);
    }
    else
    {
        // scan file contents
        internals = scanner(file);
        // this will populate the IR with vr's and nu (not needed)
        computeLastUse(internals);
        printILOC(internals,1);
        printEdges(internals);


        // for NODES: need to write a print function and increment the index by one
    }
}
