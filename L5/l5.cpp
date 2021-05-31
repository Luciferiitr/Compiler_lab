#include <bits/stdc++.h>
using namespace std;

class cfg {
    public:
    char start;
    map<char, string> prod;
    vector<char> terminals;
    vector<char> non_terminals;
    cfg() {
        start = 's';
    }
};

class state {
    public:
    int prod_count;
    int id;
    map<char, string> prod;
    state() {
        prod_count = 0;
        id = 0;
    }
    void print_item(){
        cout<<"I"<<id<<endl;
        for(auto p : prod){
            cout<<p.first<<"->"<<p.second<<endl;
        }
        cout<<endl;
    }
};

class slr {
    public:
    cfg *p;
    map<int, state*> items;
    vector<map<char, int>> goto_table;
    // maps state to id + 1
    unordered_map<string, int> id_state;
    map<char, set<char>> first, follow;
    map<string, int> prod_no;
    map<state* ,int> reduce;

    vector<string> split(string st){
        vector<string> item_prods;
        string temp = "";
        for(int i=0;i<st.length();i++){
            if(st[i] != '|'){
                temp += st[i];
            } else {
                item_prods.push_back(temp);
                temp = "";
            }
        }
        item_prods.push_back(temp);
        return item_prods;
    }

    void closure(state *I) {
        if(!I) return;
        list<char> keys;
        for(auto g : I->prod) keys.push_back(g.first);
        for( auto g : keys){
            vector<string> item_prods = split(I->prod[g]);
            for(auto item_prod : item_prods) {
                int index = item_prod.find('.') + 1;
                if(index < item_prod.length()) {
                    if(p->prod[item_prod[index]] != "") {
                        vector<string> prods = split(p->prod[item_prod[index]]);
                        if(I->prod[item_prod[index]] != ""){
                            vector<string> ps = split(I->prod[item_prod[index]]);
                            string s = "." + prods[0];
                            bool flag = 0;
                            for(auto p1 : ps) if(s == p1) flag = 1;
                            if(flag) continue;
                            I->prod[item_prod[index]] += "|";
                        }
                        for(auto prod : prods){
                            if(prod == "^"){
                                I->prod[item_prod[index]] += ".|";
                                string p = "";
                                p += item_prod[index];
                                p += "->^";
                                reduce[I] = (prod_no[p]);
                            } else {
                                I->prod[item_prod[index]] += "." + prod + "|";
                            }
                            I->prod_count++;
                        }
                        I->prod[item_prod[index]] = I->prod[item_prod[index]].substr(0, I->prod[item_prod[index]].length() - 1);
                        keys.push_back(item_prod[index]);
                    }
                }
            }
        }
    }

    string stringify_state(state *I) {
        string st = "";
        if(!I) return st;
        for(auto g : I->prod) {
            st += g.first + "->" + g.second + " ; ";
        }
        return st;
    }

    // initialize grammer with S' -> S production
    // here '@' is used in place of S'
    // initialize and find the closure of first set of items
    slr(cfg *grammer) {
        reduce.clear();
        prod_no.clear();

        for(auto sym : grammer->terminals){
            first[sym].insert(sym);
        }
        first['^'].insert('^');
        follow[grammer->start].insert('$');

        int p_no = 1;
        for(auto prods : grammer->prod){
            vector<string> prod_arr = split(prods.second);
            for(auto prod : prod_arr){
                string p = "";
                p += prods.first ; 
                p += "->" + prod;
                prod_no[p] = p_no;
                p_no++;
            }
        }

        p = grammer;
        p->prod['@'] += p->start;
        p->start = '@';
        prod_no["@->" + p->prod['@']] = -1;

        state *I0 = new state;
        I0->prod['@'] = "." + p->prod['@'];
        I0->prod_count++;
        closure(I0);
        items[0] = I0;
        id_state[stringify_state(I0)] = 1;
    }

    state* goto_state(state *I, char x){
        state *newI = NULL;
        bool is_mem_allocated = 0;
        for( auto g : I->prod) {
            // cout<<g.second<<endl;
            vector<string> item_prods = split(g.second);
            for(auto item_prod : item_prods) {
                int index = item_prod.find('.') + 1;
                if(index < item_prod.length()) {
                    if(item_prod[index] == x) {
                        string production = item_prod;
                        production[index - 1] = x;
                        production[index] = '.';
                        if(!is_mem_allocated){
                            newI = new state;
                            is_mem_allocated = 1;
                        } 
                        if(newI->prod[g.first] == "") newI->prod[g.first] = production;
                        else newI->prod[g.first] += "|" + production;
                        newI->prod_count++;
                        if(index == production.length() - 1) {
                            string p = "";
                            p += g.first;
                            p += "->" + production.substr(0, production.length() - 1);
                            reduce[newI] = (prod_no[p]);
                        }
                    }
                }
            }
            
        }
        closure(newI);
        return newI;
    }

    void cannonical_LR0_items(){
        int id = 1;
        for(auto item : items) {
            state *I = item.second;
            map<char, int> goto_item;
            for(auto x : p->non_terminals) {
                state *newI = goto_state(I, x);
                if(newI && !id_state[stringify_state(newI)]) {
                    newI->id = id;
                    goto_item[x] = id;
                    items[id] = newI;
                    id_state[stringify_state(newI)] = id + 1;
                    id++;
                } else if(id_state[stringify_state(newI)]) {
                    goto_item[x] = id_state[stringify_state(newI)] - 1;
                }
            }
            for(auto x : p->terminals) {
                state *newI = goto_state(I, x);
                if(newI && !id_state[stringify_state(newI)]) {
                    newI->id = id;
                    goto_item[x] = id;
                    items[id] = newI;
                    id_state[stringify_state(newI)] = id + 1;
                    id++;
                } else if(id_state[stringify_state(newI)]) {
                    goto_item[x] = id_state[stringify_state(newI)] - 1;
                }
            }
            goto_table.push_back(goto_item);
        }
    }

    void calculate_first_sets() {
        bool is_updated = 1;
        while(is_updated){
            is_updated = 0;
            for(auto prods : p->prod){
                int initial_size = first[prods.first].size();
                vector<string> prod_arr = split(prods.second); 
                for(auto prod : prod_arr){
                    int i=-1;
                    do {
                        i++;
                        set<char> Xi = first[prod[i]];
                        Xi.erase('^');
                        first[prods.first].insert(Xi.begin(), Xi.end());
                    } while(first[prod[i]].find('^') != first[prod[i]].end() && i<prod.length()-1);
                    if(i == prod.length() - 1 && first[prod[i]].find('^') != first[prod[i]].end()){
                        first[prods.first].insert('^');
                    }
                }
                if(first[prods.first].size() != initial_size) is_updated = 1;
            }
        }
    }

    void calculate_follow_sets() {
        bool is_updated = 1;
        while(is_updated){
            is_updated = 0;
            for(auto prods : p->prod){
                vector<string> prod_arr = split(prods.second); 
                for(auto prod : prod_arr){
                    int initial_size = follow[prod[prod.length()-1]].size();
                    follow[prod[prod.length()-1]].insert(follow[prods.first].begin(), follow[prods.first].end());
                    if(follow[prod[prod.length()-1]].size() != initial_size) is_updated = 1;
                    set<char> rest = follow[prods.first];
                    for(int i=prod.length()-1; i>0;i--) {
                        initial_size = follow[prod[i-1]].size();
                        if(first[prod[i]].find('^') != first[prod[i]].end()){
                            set<char> Xi = first[prod[i]];
                            Xi.erase('^');
                            follow[prod[i-1]].insert(Xi.begin(), Xi.end());
                            follow[prod[i-1]].insert(rest.begin(), rest.end());
                            rest = follow[prod[i-1]];
                        } else {
                            follow[prod[i-1]].insert(first[prod[i]].begin(), first[prod[i]].end());
                            rest = follow[prod[i-1]];
                        }
                        if(follow[prod[i-1]].size() != initial_size) is_updated = 1;
                    }
                }
            }
        }
    }
    
    void print_parsing_table() {
        cout<<"|    |";
        vector<char> tu$ = p->terminals;
        tu$.push_back('$');
        for(auto t : tu$) cout<<" "<<t<<"  |";
        cout<<"|";
        for(auto nt : p->non_terminals) cout<<" "<<nt<<"  |";
        cout<<endl;
        for(auto item : items){
            if(item.second->id<10) cout<<"| "<<item.second->id<<"  |";
            else cout<<"| "<<item.second->id<<" |";
            for(auto t : tu$){
                if(reduce[item.second] == 0){
                    if(goto_table[item.second->id][t]) {
                        if(goto_table[item.second->id][t] < 10)
                        cout<<" S"<<goto_table[item.second->id][t]<<" |";
                        else cout<<" S"<<goto_table[item.second->id][t]<<"|";
                    }
                    else cout<<"    |";
                } 
                else if(reduce[item.second] == -1 && t == '$'){
                    cout<<" acc|";
                } else {
                    char head;
                    for(auto prod : prod_no){
                        if(prod.second == reduce[item.second]) head = prod.first[0];
                    }
                    bool flag = 0;
                    for(auto sym : follow[head]){
                        if(sym == t) flag = 1;
                    }
                    if(flag) {
                        if(goto_table[item.second->id][t]) cout<<" SR |";
                        else{
                            if(reduce[item.second] < 10) cout<<" R"<<reduce[item.second]<<" |";
                            else cout<<" R"<<reduce[item.second]<<"|";
                        } 
                    } else {
                        if(goto_table[item.second->id][t])
                        cout<<" S"<<goto_table[item.second->id][t]<<" |";
                        else cout<<"    |";
                    }
                }
            }
            cout<<"|";
            for(auto nt : p->non_terminals){
                if(goto_table[item.second->id][nt]) {
                    if(goto_table[item.second->id][nt] < 10)
                    cout<<" "<<goto_table[item.second->id][nt]<<"  |";
                    else cout<<" "<<goto_table[item.second->id][nt]<<" |";
                }
                else cout<<"    |";
            }
            cout<<endl;
        }
    }
};

int main() {
    cfg *p = new cfg;
    p->start = 'E';
    p->non_terminals = {'E', 'T', 'F'};
    p->terminals = {'+', '*', '(', ')', 'i'};
    p->prod['E'] = "E+T|T";
    p->prod['T'] = "T*F|F";
    p->prod['F'] = "(E)|i";
    slr parser(p);
    parser.cannonical_LR0_items();
    cout<<"Cannonical collection of sets of LR(0) items =>"<<endl;
    for(auto item : parser.items) {
        item.second->print_item();
    }
    // int id = 0;
    // for(auto item : parser.goto_table) {
    //     cout<<id++<<endl;
    //     for(auto trans : item){
    //         cout<<trans.first<<"->"<<trans.second<<endl;
    //     }
    // }
    parser.calculate_first_sets();
    parser.calculate_follow_sets();
    // for(auto prod : parser.follow){
    //     cout<<prod.first<<" = { ";
    //     for(auto c : prod.second){
    //         cout<<c<<" ";
    //     }
    //     cout<<"}"<<endl;
    // }
    parser.print_parsing_table();
    cout<<endl;
    cout<<"Production Rules ->"<<endl;
    for(auto p : parser.prod_no){
        cout<<"   "<<p.first<<" __ "<<p.second<<endl;
    }
}