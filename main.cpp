#include <iostream>
#include "utils.h"
int main(int argc, char** argv) {
    auto x=parseBank(argv[0]); //"c:/Users/timri/CLionProjects/NLP_project/material/large/training.txt"
    int rulesCounter=0;
    for(auto &i:x->left_to_rules){
        rulesCounter+=i.second.size();
    }
    std::cout<<rulesCounter<<"size"<<x->left_to_rules.size()<<std::endl;
    return 0;
}