#include "sequence.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;

const string DATA_DIR = "data";
const int SEED = 2026;

string fileName(size_t size) {
    return DATA_DIR + "/sequence_" + to_string(size) + ".txt";
}

vector<int> readFromFile(const string& name) {
    ifstream in(name);
    size_t n = 0;
    in >> n;

    vector<int> data(n);
    for (size_t i = 0; i < n; i++) {
        in >> data[i];
    }
    return data;
}

static void writeToFile(const string& name, const vector<int>& data) {
    ofstream out(name);
    out << data.size() << "\n";
    for (size_t i = 0; i < data.size(); i++) {
        if (i > 0) {
            out << " ";
        }
        out << data[i];
    }
    out << "\n";
}

void generateFiles() {
    filesystem::create_directories(DATA_DIR);

    mt19937 gen(SEED);
    uniform_int_distribution<int> dist(MIN_VALUE, MAX_VALUE);

    for (size_t size : SIZES) {
        if (filesystem::exists(fileName(size))) {
            continue;
        }

        vector<int> data(size);
        for (size_t i = 0; i < size; i++) {
            data[i] = dist(gen);
        }
        writeToFile(fileName(size), data);
        cerr << "Generated " << fileName(size) << "\n"; 
    }
}
