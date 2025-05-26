#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

struct Item {
    int cur_v;
    int cur_w;
    double ratio;
};

int main() {
    ifstream input("ks_30_0.txt");
    if (!input) {
        cerr << "Error: could not open file" << endl;
        return 1;
    }

    int N, W;
    input >> N >> W;

    vector<Item> items(N);
    for (int i = 0; i < N; ++i) {
        input >> items[i].cur_v >> items[i].cur_w;
        items[i].ratio = (double)items[i].cur_v / items[i].cur_w;
    }

    const int repeats = 10000;
    int total_value = 0;
    auto start = high_resolution_clock::now();
    
    for (int r = 0; r < repeats; ++r) {
        sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
            return a.ratio > b.ratio;
        });

        int current_value = 0;
        int remaining = W;
        
        for (const auto& item : items) {
            if (item.cur_w <= remaining) {
                current_value += item.cur_v;
                remaining -= item.cur_w;
            }
        }
        
        if (r == 0) total_value = current_value;
    }
    
    auto end = high_resolution_clock::now();
    auto total_time = duration_cast<nanoseconds>(end - start).count() / 10000.;

    cout << total_value << endl;
    cout << total_time / repeats << endl;

    input.close();
    return 0;
}