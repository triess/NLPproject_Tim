#include <iostream>
#include <cstring>
#include "utils.h"
int main(int argc, char** argv) {
    auto command = argv[1];
    if(std::strcmp(command,"induce")==0){
        //auto x=parseBank(argv[2]);
        std::string line;
        auto* ind = new struct index();
        while (std::getline(std::cin,line)){
            if(!line.empty()){
                for(auto &r:treeToRules(readTree(line))){
                    ind->buildIndexByRule(r);
                }
            }else{
                break;
            }
        }
        ind->finishIndex();
        if(argc==3){
            saveGrammar(*ind);
        }else{
            printGrammar(*ind);
        }
    }else if(std::strcmp(command,"parse")==0){

    }
     //"c:/Users/timri/CLionProjects/NLP_project/material/large/training.txt"
    return 0;
}