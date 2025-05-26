#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

const int GEN_MAX = 500;
const double MUT_RATE = 0.01;
const int POP_NUM = 500;

struct Item {
    int val;
    int wt;
};

mt19937 rng(time(nullptr));

vector<bool> init_ind(const vector<Item>& objs, int cap) {
    vector<bool> ind(objs.size(), false);
    int cw = 0;
    for (size_t i = 0; i < objs.size(); ++i) {
        if (uniform_real_distribution<>(0, 1)(rng) < 0.5 && cw + objs[i].wt <= cap) {
            ind[i] = true;
            cw += objs[i].wt;
        }
    }
    return ind;
}

int eval(const vector<bool>& ind, const vector<Item>& objs, int cap) {
    int tw = 0, tv = 0;
    for (int i = 0; i < ind.size(); i++) {
        if (ind[i]) {
            tw += objs[i].wt;
            tv += objs[i].val;
        }
    }
    return (tw > cap) ? tv - (tw - cap)*1000 : tv;
}

vector<int> select_parents(const vector<int>& scores) {
    discrete_distribution<int> dist(scores.begin(), scores.end());
    vector<int> parents;
    for (int i = 0; i < POP_NUM; i++) {
        parents.push_back(dist(rng));
    }
    return parents;
}

void crossover(const vector<bool>& p1, const vector<bool>& p2, vector<bool>& c1, vector<bool>& c2) {
    uniform_int_distribution<int> point(1, p1.size()-1);
    int cp = point(rng);
    
    c1 = vector<bool>(p1.begin(), p1.begin() + cp);
    c1.insert(c1.end(), p2.begin() + cp, p2.end());
    
    c2 = vector<bool>(p2.begin(), p2.begin() + cp);
    c2.insert(c2.end(), p1.begin() + cp, p1.end());
}

void mutate(vector<bool>& ind) {
    for (size_t i = 0; i < ind.size(); i++) {
        if (uniform_real_distribution<>(0, 1)(rng) < MUT_RATE) {
            ind[i] = !ind[i];
        }
    }
}

vector<bool> ga_solve(const vector<Item>& objs, int n, int cap) {
    vector<vector<bool>> pop(POP_NUM);
    for (auto& ind : pop) {
        ind = init_ind(objs, cap);
    }

    for (int gen = 0; gen < GEN_MAX; gen++) {
        vector<int> scores(POP_NUM);
        for (int i = 0; i < POP_NUM; i++) {
            scores[i] = eval(pop[i], objs, cap);
        }

        auto parents = select_parents(scores);
        vector<vector<bool>> new_pop;
        
        for (int i = 0; i < POP_NUM; i += 2) {
            vector<bool> c1, c2;
            crossover(pop[parents[i]], pop[parents[i+1]], c1, c2);
            mutate(c1);
            mutate(c2);
            new_pop.push_back(c1);
            new_pop.push_back(c2);
        }

        int elite = max_element(scores.begin(), scores.end()) - scores.begin();
        new_pop[0] = pop[elite];
        
        pop = new_pop;
    }

    int best_score = 0;
    vector<bool> solution;
    for (const auto& ind : pop) {
        int current = eval(ind, objs, cap);
        if (current > best_score) {
            best_score = current;
            solution = ind;
        }
    }
    return solution;
}

int main() {
    ifstream fin("ks_30_0.txt");
    if (!fin) {
        cerr << "File error" << endl;
        return 1;
    }

    int n, cap;
    fin >> n >> cap;

    vector<Item> objs(n);
    for (auto& item : objs) {
        fin >> item.val >> item.wt;
    }

    auto start = high_resolution_clock::now();
    auto solution = ga_solve(objs, n, cap);
    auto end = high_resolution_clock::now();
    
    cout << eval(solution, objs, cap) << endl;
    cout << duration_cast<microseconds>(end - start).count() << endl;

    fin.close();
    return 0;
}