#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <queue>

struct node{

    std::string data;
    std::vector<node> children;
    bool operator==(const node& other)const{
        return data==other.data && children==other.children;
    }
    explicit node(std::string data) : data(std::move(data)){}
    node(std::string d, std::vector<node> c):data(std::move(d)),children(std::move(c)){}
};
struct rule{
    std::string left;
    std::vector<std::string> right;
    int count;
    bool lexical;
    rule(std::string  l,int c,bool lex):left(std::move(l)),count(c),lexical(lex){}
    rule(std::string l, int c, bool lex, std::vector<std::string> r):left(std::move(l)),right(std::move(r)),lexical(lex), count(c){}
    bool operator==(const rule& other)const{
        return (left==other.left && right==other.right);
    }
};

struct weighted_rule{
    struct rule rule;
    double weight;
    weighted_rule(struct rule r, double w):rule(std::move(r)),weight(w){}
    bool operator==(const weighted_rule& other) const{
        return (rule==other.rule);
    }
};

struct queue_element{
    int left;
    int right;
    struct weighted_rule rule;
    double prob;
    std::vector<struct rule> backtrace;
    bool operator<(const queue_element& other) const{
        return prob<other.prob;
    }
    bool operator==(const queue_element& other) const{
        return (left==other.left && right==other.right && rule==other.rule);
    }
    queue_element(int l, int r, weighted_rule wr,double p):left(l),right(r), rule(std::move(wr)),prob(p){}
    queue_element(int l, int r, weighted_rule wr,double p,std::vector<struct rule> bt):left(l),right(r), rule(std::move(wr)),prob(p), backtrace(std::move(bt)){}
};

struct weightedRulesComparator{
    bool operator()(const weighted_rule& wr1, const weighted_rule& wr2) const{
        return wr1.weight < wr2.weight;
    }
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
            static rule default_rule = {"", 0, false,{}};
            return default_rule;
        }
        return it->second.front();
    }
};

node readTree(const std::string& treeString);
struct index* parseBank(const std::string& path);
std::vector<std::string> split(const std::string& s, const std::string& delimiter);
std::vector<std::string> split_sexpr(std::string sexpr);
void saveGrammar(const struct index& grammar, const std::string& name);
void printGrammar(const struct index& grammar);
std::vector<rule> treeToRules(const node& tree);
struct weighted_rule readNonLexical(const std::string& line);
struct weighted_rule readLexical(const std::string& line);
std::map<std::string, std::set<weighted_rule,weightedRulesComparator>> loadNonTerminal(const std::string& rules_path);
std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> loadTerminal(const std::string& lex);
void addQueueElements(std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> rules,const queue_element& qe,std::priority_queue<queue_element> * queue, int word_count,std::vector<queue_element> c);
std::string treeToSExpression(const node& root);
std::vector<rule> deductiveParsing(const std::string& sentence,const std::map<std::string, std::set<weighted_rule, weightedRulesComparator>>& rules, std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> lex);
void printBacktrace(const std::vector<rule>& bt, const std::string& sentence, const std::string& root);

