#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

struct node{
    explicit node(std::string data) : data(std::move(data)){}
    std::string data;
    std::vector<node> children;
};
struct rule{
    std::string left;
    std::vector<std::string> right;
    int count;
    bool lexical;
};

bool compareRulesByCount(const rule& r1, const rule& r2);

struct index{
    std::map<std::string, std::vector<rule>> left_to_rules;

    void buildIndexByRule(const rule& r){
        auto it = left_to_rules.find(r.left);
        if(it != left_to_rules.end()){
            auto& rules = it->second;
            for(auto& rule:rules){
                if(rule.left==r.left && rule.right==r.right){
                    rule.count++;
                    return;
                }
            }
            rules.push_back(r);
            rules.back().count = 1;
        }else{
            left_to_rules.insert({r.left,{r}});
            left_to_rules[r.left][0].count = 1;
        }
    }

    void finishIndex(){
        for(auto& p: left_to_rules){
            std::sort(p.second.begin(), p.second.end(), compareRulesByCount);
        }
    }

    [[nodiscard]] const rule& getBestMatch(const std::string& left) const{
        auto it = left_to_rules.find(left);
        if(it == left_to_rules.end() || it->second.empty()){
            static rule default_rule = {"", {}, 0};
            return default_rule;
        }
        return it->second.front();
    }
};

node readTree(const std::string& treeString);
struct index* parseBank(const std::string& path);
std::vector<std::string> split(const std::string& s, const std::string& delimiter);
std::vector<std::string> split_sexpr(std::string sexpr);
void saveGrammar(const index& grammar);