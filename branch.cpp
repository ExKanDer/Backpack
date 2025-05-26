#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <queue>

using namespace std;
using namespace chrono;

struct Item {
    int profit;
    int weight;
    double ratio;
};

struct Node {
    int level;
    int profit;
    int weight;
    double bound;
};

bool compare(Item a, Item b) {
    return a.ratio > b.ratio;
}

double bound(Node u, int n, int W, vector<Item>& arr) {
    if (u.weight >= W) return 0;

    double profit_bound = u.profit;
    int j = u.level + 1;
    int tot_weight = u.weight;

    while ((j < n) && (tot_weight + arr[j].weight <= W)) {
        tot_weight += arr[j].weight;
        profit_bound += arr[j].profit;
        j++;
    }

    if (j < n)
        profit_bound += (W - tot_weight) * arr[j].ratio;

    return profit_bound;
}

int knapsack(int W, vector<Item>& arr, int n) {
    sort(arr.begin(), arr.end(), compare);

    queue<Node> Q;
    Node u, v;

    u.level = -1;
    u.profit = u.weight = 0;
    Q.push(u);

    int max_profit = 0;

    while (!Q.empty()) {
        u = Q.front();
        Q.pop();

        if (u.level == -1) v.level = 0;

        if (u.level == n-1) continue;

        v.level = u.level + 1;
        v.weight = u.weight + arr[v.level].weight;
        v.profit = u.profit + arr[v.level].profit;

        if (v.weight <= W && v.profit > max_profit)
            max_profit = v.profit;

        v.bound = bound(v, n, W, arr);

        if (v.bound > max_profit)
            Q.push(v);

        v.weight = u.weight;
        v.profit = u.profit;
        v.bound = bound(v, n, W, arr);

        if (v.bound > max_profit)
            Q.push(v);
    }

    return max_profit;
}

int main() {
    ifstream infile("ks_30_0.txt");
    if (!infile) {
        cerr << "Failed to open file" << endl;
        return 1;
    }

    int n, W;
    infile >> n >> W;

    vector<Item> items(n);
    for (int i = 0; i < n; i++) {
        infile >> items[i].profit >> items[i].weight;
        items[i].ratio = (double)items[i].profit / items[i].weight;
    }

    auto start = high_resolution_clock::now();
    int result = knapsack(W, items, n);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << result << endl;
    cout << duration.count() << endl;

    infile.close();
    return 0;
}