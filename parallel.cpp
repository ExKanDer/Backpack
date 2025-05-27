#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

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

    vector<Item> original_items(N);
    for (int i = 0; i < N; ++i) {
        input >> original_items[i].cur_v >> original_items[i].cur_w;
        original_items[i].ratio = static_cast<double>(original_items[i].cur_v) / original_items[i].cur_w;
    }

    const int repeats = 10000;
    atomic<int> total_value(0);
    vector<thread> threads;
    unsigned num_threads = thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    auto start = high_resolution_clock::now();

    for (unsigned t = 0; t < num_threads; ++t) {
        threads.emplace_back(thread([&original_items, &total_value, W, repeats, num_threads, t]() {
            vector<Item> items = original_items;
            int local_max = 0;

            for (int r = t; r < repeats; r += num_threads) {
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
                
                if (r == t) {
                    local_max = current_value;
                }
            }

            int current_val = total_value.load();
            while (local_max > current_val && 
                  !total_value.compare_exchange_weak(current_val, local_max)) {}
        }));
    }

    for (auto& th : threads) {
        th.join();
    }

    auto end = high_resolution_clock::now();
    auto total_time = duration_cast<nanoseconds>(end - start).count() / repeats;

    cout << total_value << endl;
    cout << total_time << endl;

    input.close();
    return 0;
}