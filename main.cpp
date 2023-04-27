#include <iostream>
#include "utils.h"
int main(int argc, char** argv) {
    if(argv[0]=="induce"){
        auto x=parseBank(argv[1]);
        saveGrammar(*x);
    }
     //"c:/Users/timri/CLionProjects/NLP_project/material/large/training.txt"
    return 0;
}