#include <iostream>
#include <cstring>
#include "utils.h"
int main(int argc, char** argv) {
    if(std::strcmp(argv[1],"induce")==0){
        std::cout<<"inducing grammar"<<std::endl;
        auto x=parseBank(argv[2]);
        if(argc==4){
            saveGrammar(*x);
        }else{
            printGrammar(*x);
        }
    }
     //"c:/Users/timri/CLionProjects/NLP_project/material/large/training.txt"
    return 0;
}