#include "utils.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

node readTree(const std::string& treeString){
    std::vector<std::string> elements = split_sexpr(treeString);
    std::vector<std::string> sliced = split(treeString, " ");
    node root {sliced[0].substr(1)};
    if(elements.empty()){
        root.children.emplace_back(sliced[1].substr(0,sliced[1].size()-1));
        return root;
    }
    root.children.reserve(elements.size());
    for (const auto & element : elements) {
        root.children.emplace_back(readTree(element));
    }
    return root;
}

std::vector<rule> treeToRules(const node& tree){
    if(tree.children.size()==1 && tree.children[0].children.empty()){
        rule r = {tree.data,{tree.children[0].data},1,true};
        return {r};
    }
    std::vector<rule> rules ={};
    rule r = {tree.data,{},1,false};
    for(auto &n:tree.children){
        r.right.push_back(n.data);
        std::vector<rule> recur = treeToRules(n);
        rules.insert(rules.end(),recur.begin(),recur.end());
    }
    rules.push_back(r);
    return rules;
}

struct index* parseBank(const std::string& path){
    std::ifstream treeBank (path);
    //treeBank.open("material/large/training.mrg");
    std::string oneTree;
    auto* ind = new struct index();
    while (std::getline(treeBank,oneTree)){
        for(auto &r:treeToRules(readTree(oneTree))){
            //std::cout<<r.left<<std::endl;
            ind->buildIndexByRule(r);
        }
        ind->finishIndex();
    }
    treeBank.close();
    return ind;
}

bool compareRulesByCount(const rule& r1, const rule& r2){
    return r1.count > r2.count;
}
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end = 0, delim_len = delimiter.length();
    std::vector<std::string> res;
    res.reserve(std::count(s.begin(), s.end(), delimiter[0]) + 1);

    while (pos_start <= s.length()) {
        pos_end = s.find(delimiter, pos_start);
        if (pos_end == std::string::npos) {
            pos_end = s.length();
        }
        res.emplace_back(s.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
    }

    return res;
}

std::vector<std::string> split_sexpr(std::string sexpr){
    std::vector<std::string> parts;
    int start = -1;
    int depth = 0;
    for (int i = 1; i < sexpr.size(); ++i) {
        char c = sexpr.at(i);
        if(c=='('){
            if(depth==0){
                start = i;
            }
            depth++;
        }
        if(c==')'){
            depth--;
            if (depth==0 and start != -1){
                parts.push_back(sexpr.substr(start,i-start+1));
                start = -1;
            }
        }
    }
    if(start != -1){
        parts.push_back(sexpr.substr(start));
    }
    return parts;
}

void saveGrammar(const index& grammar){
    std::ofstream lexi("grammar.lexicon",std::ios_base::app);
    std::ofstream words("grammar.words",std::ios_base::app);
    std::ofstream rules("grammar.rules",std::ios_base::app);
    for(const auto& kvp:grammar.left_to_rules){
        int counter=0;
        for(const auto& r:kvp.second){
            counter += r.count;
        }
        for(const auto& r:kvp.second){
            double p = static_cast<double>(r.count)/static_cast<double>(counter);
            if(r.lexical){
                lexi << r.left << " " << r.right[0] << " " << p << "\n";
                words << r.right[0] << "\n";
            }else{
                rules<< r.left << " ";
                for(const auto& s:r.right){
                    rules << s << " ";
                }
                rules << p << "\n";
            }

        }
    }
    lexi.close();
    words.close();
    rules.close();
}

void printGrammar(const index& grammar){
    std::cout<<"printing grammar"<<std::endl;
    for(const auto& kvp:grammar.left_to_rules){
        int counter=0;
        for(const auto& r:kvp.second){
            counter += r.count;
        }
        for(const auto& r:kvp.second){
            double p = static_cast<double>(r.count)/static_cast<double>(counter);
            std::cout<<r.left<<" ";
            for(const auto& s:r.right){
                std::cout<<s<<" ";
            }
            std::cout<<p<<std::endl;
        }
    }
}