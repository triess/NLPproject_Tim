#include <iostream>
#include <cstring>
#include "utils.h"
#include "main.h"

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
        if(argc==3){
            saveGrammar(*ind,argv[2]);
        }else{
            printGrammar(*ind);
        }
    }else if(std::strcmp(command,"parse")==0){
        std::string rules = argv[argc-2];
        std::string lex = argv[argc-1];
        auto root = getCmdOption(argv,argv+argc,"-i");
        if(root==nullptr)std::strcpy(root,"ROOT");
        auto r = loadNonTerminal(rules);
        auto l = loadTerminal(lex);
        std::string line;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                auto bt=deductiveParsing(line,r,l);
                printBacktrace(bt,line, root);
            }
        }
    }
    else {
        return 22;
    }
     //"c:/Users/timri/CLionProjects/NLP_project/material/large/training.txt"
    return 0;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return nullptr;
}
bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}