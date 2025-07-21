#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <utility>

struct ParsedProcess {
    std::string name;
    int arrival;
    int service;
    int priority;
};

// Reads input in this format:
// Line1: "trace" or "stats"
// Line2: algorithms like 1,2-3,3
// Line3: lastInstant (int)
// Line4: processCount (int)
// Next lines: processName,arrival,service,priority(optional)
void parseInput(
    std::vector<ParsedProcess> &procs,
    std::vector<std::pair<char,int>> &algos,
    std::string &operation,
    int &lastInstant
);

#endif
