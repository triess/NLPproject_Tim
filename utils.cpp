#include "utils.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <set>
#include <queue>

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
        rule r = {tree.data,1,true,{tree.children[0].data}};
        return {r};
    }
    std::vector<rule> rules ={};
    rule r = {tree.data,1,false,{}};
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
    }
    ind->finishIndex();
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

void saveGrammar(const struct index& grammar, std::string name){
    std::ofstream lexi(name+".lexicon",std::ios::trunc);
    std::ofstream words(name+".words",std::ios::trunc);
    std::ofstream rules(name+".rules",std::ios::trunc);
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
                rules<< r.left << " -> ";
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

std::vector<rule> deductiveParsing(const std::string& sentence,const std::map<std::string, std::set<weighted_rule, weightedRulesComparator>>& rules, std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> lex, std::string root){
    std::priority_queue<queue_element> queue;
    std::vector<queue_element> c;
    auto words = split(sentence, " ");
    int counter=0;
    //initialize queue
    for(const auto& w:words){
        auto qe= new struct queue_element(counter,counter+1,*lex[w].rbegin(),0.0,{});
        qe->prob=qe->rule.weight;
        qe->backtrace.emplace_back(qe->rule.rule);
        queue.push(*qe);
        counter++;
    }
    //main loop
    while(queue.top().prob>0){
        auto qe = queue.top();
        queue.pop();
        auto it = std::find(c.begin(), c.end(),qe);
        if(it==c.end()){
            c.emplace_back(*new struct queue_element(*it));
            //add q elements
            addQueueElements(rules,qe,queue,words.size(),c);
        } else if(it->prob==0){
            it->prob=qe.prob;
            addQueueElements(rules,qe,queue,words.size(),c);
        }
    }
    //find solution
    auto final = std::find_if(c.begin(),c.end(),[&words, &root](const struct queue_element& x){
      return x.left==0 && x.right==words.size() && x.rule.rule.left==root;
    });
    if(final==c.end())return {};
    return final->backtrace;
}

void addQueueElements(std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> rules,const queue_element& qe,std::priority_queue<queue_element> queue, int word_count,std::vector<queue_element> c){
    for(auto r:rules[qe.rule.rule.left]){
        if(r.rule.right.size()==1){
            auto nqe = new struct queue_element(qe);
            nqe->rule = r;
            nqe->prob = qe.prob*r.weight;
            nqe->backtrace.emplace_back(r.rule);
            queue.push(*nqe);
        }else if(r.rule.right[0]==qe.rule.rule.left){
            for(int j=qe.right;j<word_count;j++){
                auto finder = std::find(c.begin(),c.end(),*new struct queue_element(qe.left,j,r,0.0));
                if(finder!=c.end() && finder->prob!=0.0){
                    auto x =new struct queue_element(qe.left,j,r,finder->prob*qe.prob*r.weight,{});
                    std::copy(qe.backtrace.begin(), qe.backtrace.end(),x->backtrace.end());
                    x->backtrace.emplace_back(r.rule);
                    queue.push(*x);
                }
            }
        }else if(r.rule.right[1]==qe.rule.rule.left){
            for(int i=0;i<qe.left;i++){
                auto finder = std::find(c.begin(),c.end(),*new struct queue_element(i,qe.right,r,0.0));
                if(finder!=c.end() && finder->prob!=0.0){
                    auto x = new struct queue_element(i,qe.right,r,finder->prob*qe.prob*r.weight,{});
                    std::copy(qe.backtrace.begin(), qe.backtrace.end(),x->backtrace.end());
                    x->backtrace.emplace_back(r.rule);
                    queue.push(*x);
                }
            }
        }
    }
}

void loadGrammar(const std::string& rules, const std::string& lex){
    auto non_terminal = loadNonTerminal(rules);
    auto terminal = loadTerminal(lex);
}

std::map<std::string, std::set<weighted_rule, weightedRulesComparator>> loadTerminal(const std::string& lex){
    std::map<std::string, std::set<weighted_rule,weightedRulesComparator>> terminal;
    std::ifstream rules_data(lex);
    std::string line;
    while(std::getline(rules_data, line)){
        if(line.empty()){
            break;
        }else{
            auto wr = readLexical(line);
            terminal[wr.rule.right[0]].insert(wr);
        }
    }
    return terminal;
}

std::map<std::string, std::set<weighted_rule,weightedRulesComparator>> loadNonTerminal(const std::string& rules){
    std::map<std::string, std::set<weighted_rule,weightedRulesComparator>> non_terminal;
    std::ifstream rules_data(rules);
    std::string line;
    while(std::getline(rules_data, line)){
        if(line.empty()){
            break;
        }else{
            auto wr = readNonLexical(line);
            for(auto nt:wr.rule.right){
                non_terminal[nt].insert(wr);
            }
        }
    }
    return non_terminal;
}

struct weighted_rule readLexical(const std::string& line){
    auto split_line = split(line, " ");
    auto r = new struct rule(split_line[0],0, true,{split_line[1]});
    auto wr = new struct weighted_rule(*r,std::stod(split_line[2]));
    return *wr;
}

struct weighted_rule readNonLexical(const std::string& line){
    auto split_line = split(line, " ");
    auto r = new struct rule(split_line[0],0,false);
    for(int i=2;i<split_line.size()-1;i++){
        r->right.push_back(split_line[i]);
    }
    auto wr = new struct weighted_rule(*r,std::stod(split_line[split_line.size()-1]));
    return *wr;
}

void printGrammar(const struct index& grammar){
    //std::cout<<"printing grammar"<<std::endl;
    for(const auto& kvp:grammar.left_to_rules){
        int counter=0;
        for(const auto& r:kvp.second){
            counter += r.count;
        }
        for(const auto& r:kvp.second){
            double p = static_cast<double>(r.count)/static_cast<double>(counter);
            std::cout<<r.left<<" ";
            if(!r.lexical)std::cout<<"-> ";
            for(const auto& s:r.right){
                std::cout<<s<<" ";
            }
            std::cout<<p<<std::endl;
        }
    }
}