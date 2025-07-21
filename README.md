# Scheduler Project

This project simulates classic CPU scheduling algorithms (FCFS, RR, SPN, SRT, HRRN, FB-1, FB-2i, Aging) and outputs either a timeline (Gantt chart) or statistics for each process.

## Input Format
- Place your input in a file named `input.txt` in the project directory.
- The format is:
  1. `trace` or `stats` (first line: output mode)
  2. Algorithm list (e.g., `1,2-2,3`)
  3. Last time unit to simulate (integer)
  4. Number of processes (integer)
  5. Each process: `name,arrival,service[,priority]`

## Supported Algorithms
- `1`: FCFS
- `2`: RR (quantum required, e.g., `2-2` for quantum 2)
- `3`: SPN
- `4`: SRT
- `5`: HRRN
- `6`: FB-1
- `7`: FB-2i
- `8`: Aging (quantum required)

## Usage
1. Edit `input.txt` with your test case.
2. Build and run the program:
   ```
   g++ -std=c++11 -o scheduler scheduler.cpp parser.cpp
   ./scheduler
   ```
3. Output will be printed to the terminal.

## Example: Timeline Output (`trace`)
**input.txt:**
```
trace
1,2-2,3,4
20
3
P1,0,3
P2,2,6
P3,4,4
```
**Output (snippet):**
```
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
P1 |*|*|*| | | | | | | | | | | | | | | | | |
P2 | | |.|*|*|*|*|*|*| | | | | | | | | | | |
P3 | | | | |.|.|.|.|.|*|*|*|*| | | | | | | |
```

## Example: Statistics Output (`stats`)
**input.txt:**
```
stats
1,2-2,3
15
3
X,0,5
Y,2,3
Z,4,2
```
**Output (snippet):**
```
== FCFS ==
Process:|X|Y|Z|
Finish:|5|8|10|
TAT:|5|6|6|
NormTAT:|1|2|3|
```

## More Test Cases
See the provided test files for more examples. 