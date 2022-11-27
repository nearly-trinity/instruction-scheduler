#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <unordered_map>
#include "scanner.h"

using namespace std;

vector<string> alphabet; 

unordered_map<string,TokenCategory> setupInput() {
    alphabet = {"s","u","r","a","b","d","e","h","i","l","m","n","o","p","I","/",",","="};
    unordered_map<string,TokenCategory> res;
    for(auto c : alphabet) {
        if(c == "r") {
           res[c] = REG;
        } else if(islower(c[0]) || c == "I") {
            res[c] = INST;
        } else if(c == "/") {
            res[c] = COMMENT;
        } else if(c == ",") {
            res[c] = COMMA;
        } else {
            res[c] = ARROW;
        }
    }
    for(int i = 0; i < 10; ++i) {
        res[to_string(i)] = CONST;
    }
    return res;
}

unordered_map<int,Token> setupToken() {
    unordered_map<int,Token> res;
    res[5] = {.cat=INST, .inst={.cat=MEMOP,.op=store}, -1};
    res[7] = {.cat=INST, .inst={.cat=ARITHOP,.op=sub}, -1};
    res[11] = {.cat=INST, .inst={.cat=MEMOP,.op=load}, -1};
    res[12] = {.cat=INST, .inst={.cat=LOADI,.op=loadI}, -1};
    res[18] = {.cat=INST, .inst={.cat=ARITHOP,.op=rshift}, -1};
    res[24] = {.cat=INST, .inst={.cat=ARITHOP,.op=add}, -1};
    res[27] = {.cat=INST, .inst={.cat=NOP,.op=nop}, -1};
    res[33] = {.cat=INST, .inst={.cat=OUTPUT,.op=output}, -1};
    res[34] = {.cat=INST, .inst={.cat=ARITHOP,.op=mult}, -1};
    res[37] = {.cat=INST, .inst={.cat=ARITHOP,.op=lshift}, -1};
    return res;
}

vector<unordered_map<string,int>> setupTrans() {
    vector<unordered_map<string,int>> res;
    for(int i=0; i<38; ++i) {
        unordered_map<string,int> row;
        switch(i) {
            case 0:
                row["s"] = 1; 
                row["l"] = 8;
                row["r"] = 13;
                row["m"] = 19;
                row["a"] = 22;
                row["n"] = 25;
                row["o"] = 28;
                break;
            case 1:
                row["t"] = 2;
                row["u"] = 6;
                break;
            case 2:
                row["o"] = 3;
                break;
            case 3:
                row["r"] = 4;
                break;
            case 4:
                row["e"] = 5;
                break;
            case 5:
                break;
            case 6:
                row["b"] = 7;
                break;
            case 7:
                break;
            case 8:
                row["o"] = 9;
                row["s"] = 33;
                break;
            case 9:
                row["a"] = 10;
                break;
            case 10:
                row["d"] = 11;
                break;
            case 11:
                row["I"] = 12;
                break;
            case 12:
                break;
            case 13:
                row["s"] = 14;
                break;
            case 14:
                row["h"] = 15;
                break;
            case 15:
                row["i"] = 16;
                break;
            case 16:
                row["f"] = 17;
                break;
            case 17:
                row["t"] = 18;
                break;
            case 18:
                break;
            case 19:
                row["u"] = 20;
                break;
            case 20:
                row["l"] = 21;
                break;
            case 21:
                row["t"] = 34;
                break;
            case 22:
                row["d"] = 23;
                break;
            case 23:
                row["d"] = 24;
                break;
            case 24:
                break;
            case 25:
                row["o"] = 26;
                break;
            case 26:
                row["p"] = 27;
            case 27:
                break;
            case 28:
                row["u"] = 29;
                break;
            case 29:
                row["t"] = 30;
                break;
            case 30:
                row["p"] = 31;
                break;
            case 31:
                row["u"] = 32;
                break;
            case 32:
                row["t"] = 33;
                break;
            case 33:
                row["h"] = 34;
                break;
            case 34:
                row["i"] = 35;
                break;
            case 35:
                row["f"] = 36;
                break;
            case 36:
                row["t"] = 37;
                break;
            case 37:
                break;
        }
        res.push_back(row);
    }
    return res;
}



