#include "utils.h"
#include <fstream>
#include <algorithm>

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
    if(tree.children.empty())return {};
    std::vector<rule> rules ={};
    rule r = {tree.data,{},0};
    for(auto &n:tree.children){
        r.right.push_back(n.data);
        std::vector<rule> recur = treeToRules(n);
        rules.insert(rules.end(),recur.begin(),recur.end());
    }
    rules.push_back(r);
    return rules;
}

struct index* parseBank(std::string path){
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