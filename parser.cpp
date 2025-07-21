#include "parser.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;

void parseInput(vector<ParsedProcess> &procs,
                vector<pair<char,int>> &algos,
                string &operation,
                int &lastInstant) {
    // Read first line: operation
    if (!(cin >> operation)) {
        cerr << "[ERROR] Missing operation line (trace/stats) in input." << endl;
        exit(1);
    }
    if (operation != "trace" && operation != "stats") {
        cerr << "[ERROR] Operation must be 'trace' or 'stats'." << endl;
        exit(1);
    }

    // Read second line: algorithms
    string algoLine;
    if (!(cin >> algoLine)) {
        cerr << "[ERROR] Missing algorithm line in input." << endl;
        exit(1);
    }
    stringstream ss(algoLine);
    string token;
    while (getline(ss, token, ',')) {
        if (token.empty()) continue;
        if (!isalpha(token[0]) && !isdigit(token[0])) {
            cerr << "[ERROR] Invalid algorithm ID: " << token << endl;
            exit(1);
        }
        if (token.find('-') != string::npos) {
            char id = token[0];
            int q = 0;
            try {
                q = stoi(token.substr(token.find('-') + 1));
            } catch (...) {
                cerr << "[ERROR] Invalid quantum for algorithm: " << token << endl;
                exit(1);
            }
            if (q <= 0) {
                cerr << "[ERROR] Quantum must be positive for algorithm: " << token << endl;
                exit(1);
            }
            algos.push_back({id, q});
        } else {
            char id = token[0];
            algos.push_back({id, 0});
        }
    }
    if (algos.empty()) {
        cerr << "[ERROR] No algorithms specified." << endl;
        exit(1);
    }

    // Read third line: lastInstant
    if (!(cin >> lastInstant) || lastInstant <= 0) {
        cerr << "[ERROR] Invalid or missing lastInstant (must be positive integer)." << endl;
        exit(1);
    }

    // Read fourth line: process count
    int processCount;
    if (!(cin >> processCount) || processCount <= 0) {
        cerr << "[ERROR] Invalid or missing process count (must be positive integer)." << endl;
        exit(1);
    }

    // Read processes
    for (int i = 0; i < processCount; i++) {
        string line;
        if (!getline(cin >> ws, line) || line.empty()) {
            cerr << "[ERROR] Missing process line for process " << (i+1) << "." << endl;
            exit(1);
        }
        stringstream ss(line);
        string name;
        int arrival, service, priority = 0;
        if (!getline(ss, name, ',')) {
            cerr << "[ERROR] Missing process name on line: " << line << endl;
            exit(1);
        }
        string arrivalStr, serviceStr, priorityStr;
        if (!getline(ss, arrivalStr, ',')) {
            cerr << "[ERROR] Missing arrival time for process '" << name << "'." << endl;
            exit(1);
        }
        if (!getline(ss, serviceStr, ',')) {
            cerr << "[ERROR] Missing service time for process '" << name << "'." << endl;
            exit(1);
        }
        try {
            arrival = stoi(arrivalStr);
            service = stoi(serviceStr);
            if (arrival < 0 || service <= 0) throw 1;
        } catch (...) {
            cerr << "[ERROR] Invalid arrival/service time for process '" << name << "'." << endl;
            exit(1);
        }
        if (getline(ss, priorityStr, ',')) {
            try {
                priority = stoi(priorityStr);
            } catch (...) {
                cerr << "[ERROR] Invalid priority for process '" << name << "'." << endl;
                exit(1);
            }
        }
        procs.push_back({name, arrival, service, priority});
    }
}
