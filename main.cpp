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
        bool unk = false;
        if(cmdOptionExists(argv,argv+argc,"-u")){
            unk= true;
        }
        bool smooth = false;
        if(cmdOptionExists(argv,argv+argc,"-s")){
            smooth=true;
        }
        bool astar=false;
        if(cmdOptionExists(argv,argv+argc,"-a")){
            astar=true;
        }
        auto r = loadNonTerminal(rules);
        auto l = loadTerminal(lex, unk,smooth);
        auto g = rulesList(rules,false);
        auto more_rules = rulesList(lex,true);
        g.insert(g.end(),more_rules.begin(),more_rules.end());
        std::string line;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                auto bt=deductiveParsing(line,r,l,unk,smooth,astar,root,g);
                printBacktrace(bt,line, root);
            }
        }
    }
    else if(std::strcmp(command,"binarise")==0){
        int h;
        int v;
        if(cmdOptionExists(argv,argv+argc,"-h")){
            h = std::stoi(getCmdOption(argv,argv+argc,"-h"));
        }else{
            h=-1;
        }
        if(cmdOptionExists(argv,argv+argc,"-v")){
            v = std::stoi(getCmdOption(argv,argv+argc,"-v"));
        }else{
            v = 1;
        }
        std::string line;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                printTree(binarise(readTree(line),v,h));
                std::cout<<std::endl;
            }
        }
    }
    else if(std::strcmp(command,"debinarise")==0){
        std::string line;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                printTree(debinarise(readTree(line)));
                std::cout<<std::endl;
            }
        }
    }
    else if(std::strcmp(command,"unk")==0){
        int t = std::stoi(getCmdOption(argv,argv+argc,"-t"));
        std::string line;
        std::vector<node> forest;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                forest.emplace_back(readTree(line));
            }
        }
        forest = unking(forest,t);
        for(const auto& tree:forest){
            printTree(tree);
            std::cout<<std::endl;
        }
    }
    else if(std::strcmp(command,"smooth")==0){
        int t = std::stoi(getCmdOption(argv,argv+argc,"-t"));
        std::string line;
        std::vector<node> forest;
        while(std::getline(std::cin,line)){
            if(!line.empty()){
                forest.emplace_back(readTree(line));
            }
        }
        forest = smoothing(forest,t);
        for(const auto& tree:forest){
            printTree(tree);
            std::cout<<std::endl;
        }
    }
    else if(std::strcmp(command,"outside")==0){
        std::string i;
        if(cmdOptionExists(argv,argv+argc,"-i")){
            i = getCmdOption(argv,argv+argc,"-i");
        }else{
            i = "ROOT";
        }
        std::string last = argv[argc-1];
        std::string lex;
        std::string rules;
        bool save=false;
        if(last.substr(last.length()-8)==".lexicon"){
            lex = last;
            rules = argv[argc-2];

        }else{
            lex = argv[argc-2];
            rules = argv[argc-3];
            save = true;
        }
        auto all_rules= rulesList(lex,true);
        auto more_rules = rulesList(rules,false);
        all_rules.insert(all_rules.end(),more_rules.begin(),more_rules.end());
        std::vector<std::string> lefts;
        for(const auto& wr:all_rules){
            if(std::find(lefts.begin(),lefts.end(),wr.rule.left)==lefts.end()){
                lefts.emplace_back(wr.rule.left);
            }
        }
        if(save){
            std::ofstream file(last);
            for(auto l:lefts){
                file<<l<<" "<<outside(l,all_rules,i)<<std::endl;
            }
        }else{
            for(auto l:lefts){
                std::cout<<l<<" "<<outside(l,all_rules,i)<<std::endl;
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