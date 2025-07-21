/*
Scheduler Project Usage:
- Place your input in input.txt in the same directory.
- Run the program; it will read from input.txt automatically if present.
- Input format:
  Line 1: 'trace' or 'stats'
  Line 2: algorithm list (e.g. 1,2-3,3)
  Line 3: lastInstant (int)
  Line 4: processCount (int)
  Next lines: processName,arrival,service[,priority]
- Supported algorithms:
  1: FCFS
  2: RR (quantum required)
  3: SPN
  4: SRT
  5: HRRN
  6: FB-1
  7: FB-2i
  8: Aging (quantum required)
*/
#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "parser.h"
#include <fstream>
using namespace std;

class Scheduler {
public:
    struct Proc { string name; int arrival, service, priority; };
    int processCount, lastInstant;
    string operation;
    vector<pair<char,int>> algorithms;
    vector<Proc> processes;

    vector<vector<char>> timeline;
    vector<int> finishTime, turnAroundTime;
    vector<float> normTurn;

    void init() {
        processCount = (int)processes.size();
        timeline.assign(lastInstant, vector<char>(processCount,' '));
        finishTime.assign(processCount,0);
        turnAroundTime.assign(processCount,0);
        normTurn.assign(processCount,0.0f);
    }
    void clearTimeline() {
        for(int i=0;i<lastInstant;i++) for(int j=0;j<processCount;j++) timeline[i][j]=' ';
    }
    void markFinish(int idx,int t){
        finishTime[idx]=t;
        turnAroundTime[idx]=t-processes[idx].arrival;
        normTurn[idx]=(float)turnAroundTime[idx]/processes[idx].service;
    }
    void fillWaitTime(){
        for(int i=0;i<processCount;i++)
            for(int k=processes[i].arrival;k<finishTime[i];k++)
                if(timeline[k][i]!='*') timeline[k][i]='.';
    }

    // ---------------- Algorithms (implementations trimmed for brevity) ----------------
    void FCFS() {
        int time = processes[0].arrival;
        for (int i=0;i<processCount;i++){
            int arr=processes[i].arrival, serv=processes[i].service;
            finishTime[i]=time+serv;
            turnAroundTime[i]=finishTime[i]-arr;
            normTurn[i]= (float)turnAroundTime[i]/serv;
            for(int t=time;t<finishTime[i];t++) timeline[t][i]='*';
            for(int t=arr;t<time;t++) timeline[t][i]='.';
            time+=serv;
        }
    }

    void RR(int quantum){
        if(quantum<=0)return;
        std::queue<std::pair<int,int>> q;
        int j=0;
        if(processes[0].arrival==0){q.push(std::make_pair(0,processes[0].service));j++;}
        int curQ=quantum;
        for(int time=0;time<lastInstant;time++){
            if(!q.empty()){
                std::pair<int,int> front = q.front(); q.pop();
                int idx = front.first;
                int rem = front.second;
                rem--;curQ--;
                timeline[time][idx]='*';
                if(rem==0){
                    finishTime[idx]=time+1;
                    turnAroundTime[idx]=finishTime[idx]-processes[idx].arrival;
                    normTurn[idx]=(float)turnAroundTime[idx]/processes[idx].service;
                    curQ=quantum;
                }else if(curQ==0){
                    q.push(std::make_pair(idx,rem));
                    curQ=quantum;
                }else{
                    q.push(std::make_pair(idx,rem));
                }
            }
            while(j<processCount && processes[j].arrival==time+1){
                q.push(std::make_pair(j,processes[j].service));
                j++;
            }
        }
        fillWaitTime();
    }

    void SPN(){
        using P=std::pair<int,int>;
        std::priority_queue<P,std::vector<P>,std::greater<P>> pq;
        int j=0;
        for(int t=0;t<lastInstant;t++){
            while(j<processCount && processes[j].arrival<=t){
                pq.push({processes[j].service,j});
                j++;
            }
            if(!pq.empty()){
                int idx=pq.top().second;pq.pop();
                int arr=processes[idx].arrival, serv=processes[idx].service;
                for(int k=t;k<t+serv;k++) timeline[k][idx]='*';
                finishTime[idx]=t+serv;
                turnAroundTime[idx]=finishTime[idx]-arr;
                normTurn[idx]=(float)turnAroundTime[idx]/serv;
                t+=serv-1;
            }
        }
        fillWaitTime();
    }

    void SRT(){
        using P=std::pair<int,int>;
        std::priority_queue<P,std::vector<P>,std::greater<P>> pq;
        int j=0;
        for(int t=0;t<lastInstant;t++){
            while(j<processCount && processes[j].arrival==t){
                pq.push({processes[j].service,j});
                j++;
            }
            if(!pq.empty()){
                P top = pq.top(); pq.pop();
                int rem = top.first;
                int idx = top.second;
                timeline[t][idx]='*';
                if(rem-1>0){
                    pq.push({rem-1,idx});
                }else{
                    finishTime[idx]=t+1;
                    turnAroundTime[idx]=finishTime[idx]-processes[idx].arrival;
                    normTurn[idx]=(float)turnAroundTime[idx]/processes[idx].service;
                }
            }
        }
        fillWaitTime();
    }

    void HRRN(){
        int done=0,time=0;
        std::vector<bool> completed(processCount,false);
        while(done<processCount && time<lastInstant){
            double bestRatio=-1;int bestIdx=-1;
            for(int i=0;i<processCount;i++){
                if(!completed[i] && processes[i].arrival<=time){
                    int wait=time-processes[i].arrival;
                    double ratio=(wait+processes[i].service)/(double)processes[i].service;
                    if(ratio>bestRatio){bestRatio=ratio;bestIdx=i;}
                }
            }
            if(bestIdx==-1){time++;continue;}
            int s=processes[bestIdx].service;
            for(int k=time;k<time+s;k++) timeline[k][bestIdx]='*';
            finishTime[bestIdx]=time+s;
            turnAroundTime[bestIdx]=finishTime[bestIdx]-processes[bestIdx].arrival;
            normTurn[bestIdx]=(float)turnAroundTime[bestIdx]/processes[bestIdx].service;
            completed[bestIdx]=true;
            time+=s;done++;
        }
        fillWaitTime();
    }

    void FB1(){
        std::queue<int> q;
        int j=0;
        if(processes[0].arrival==0){q.push(0);j++;}
        std::vector<int> rem(processCount);for(int i=0;i<processCount;i++)rem[i]=processes[i].service;
        for(int t=0;t<lastInstant;t++){
            if(!q.empty()){
                int idx=q.front();q.pop();
                rem[idx]--;
                timeline[t][idx]='*';
                if(rem[idx]==0){
                    finishTime[idx]=t+1;
                    turnAroundTime[idx]=finishTime[idx]-processes[idx].arrival;
                    normTurn[idx]=(float)turnAroundTime[idx]/processes[idx].service;
                }else{
                    q.push(idx);
                }
            }
            while(j<processCount && processes[j].arrival==t+1){q.push(j);j++;}
        }
        fillWaitTime();
    }

    void FB2i(){
        std::queue<std::pair<int,int>> q; // idx,level
        int j=0;
        if(processes[0].arrival==0){q.push(std::make_pair(0,0));j++;}
        std::vector<int> rem(processCount);for(int i=0;i<processCount;i++)rem[i]=processes[i].service;
        for(int t=0;t<lastInstant;){
            if(!q.empty()){
                std::pair<int,int> front = q.front(); q.pop();
                int idx = front.first;
                int lvl = front.second;
                int quantum=std::pow(2,lvl);
                while(quantum-- && rem[idx]>0 && t<lastInstant){
                    rem[idx]--;
                    timeline[t][idx]='*';
                    t++;
                }
                if(rem[idx]==0){
                    finishTime[idx]=t;
                    turnAroundTime[idx]=finishTime[idx]-processes[idx].arrival;
                    normTurn[idx]=(float)turnAroundTime[idx]/processes[idx].service;
                }else{
                    q.push(std::make_pair(idx,lvl+1));
                }
            }else{
                t++;
            }
            while(j<processCount && processes[j].arrival<=t){q.push(std::make_pair(j,0));j++;}
        }
        fillWaitTime();
    }

    void Aging(int q){
        struct P {int pr, idx, wait;};
        std::vector<P> v;
        int j=0;int current=-1;
        for(int t=0;t<lastInstant;t++){
            while(j<processCount && processes[j].arrival<=t){
                v.push_back({processes[j].priority,j,0});
                j++;
            }
            for(auto &p:v){
                if(p.idx==current){p.pr=processes[current].priority;p.wait=0;}
                else{p.pr++;p.wait++;}
            }
            std::sort(v.begin(),v.end(),[](const P&a,const P&b){
                if(a.pr==b.pr)return a.wait>b.wait;
                return a.pr>b.pr;
            });
            if(v.empty())continue;
            current=v[0].idx;
            for(int k=0;k<q && t<lastInstant;k++,t++){
                timeline[t][current]='*';
            }
            t--;
        }
        fillWaitTime();
    }
    // ---------------- Print ----------------
    void printStats(const string &name){
        cout<<"\n== "<<name<<" ==\nProcess:";for(auto&p:processes)cout<<"|"<<p.name;cout<<"|\nFinish:";for(auto f:finishTime)cout<<"|"<<f;cout<<"|\nTAT:";for(auto t:turnAroundTime)cout<<"|"<<t;cout<<"|\nNormTAT:";for(auto n:normTurn)cout<<"|"<<n;cout<<"|\n";
    }
    void printTimeline(const string& algoname = ""){
        if (!algoname.empty()) {
            cout << "ALGO: " << algoname << endl;
        }
        for(int i=0;i<lastInstant;i++)cout<<i%10<<" ";cout<<"\n";
        for(int i=0;i<processCount;i++){
            cerr << "TEST " << processes[i].name << endl;
            cout<<processes[i].name<<" |";
            for(int j=0;j<lastInstant;j++)cout<<timeline[j][i]<<"|";
            cout<<"\n";
        }
    }

    void execute(char id,int q){
        switch(id) {
            case '1': FCFS(); break;
            case '2': RR(q); break;
            case '3': SPN(); break;
            case '4': SRT(); break;
            case '5': HRRN(); break;
            case '6': FB1(); break;
            case '7': FB2i(); break;
            case '8': Aging(q); break;
            default: cout << "Unknown algorithm ID: " << id << endl; break;
        }
    }
};

// Helper to get algorithm name from ID
const char* getAlgoName(char id) {
    switch(id) {
        case '1': return "FCFS";
        case '2': return "RR";
        case '3': return "SPN";
        case '4': return "SRT";
        case '5': return "HRRN";
        case '6': return "FB-1";
        case '7': return "FB-2i";
        case '8': return "Aging";
        default: return "Unknown";
    }
}

int main(){
    // Debug print to confirm output is working
    cout << "[DEBUG] Scheduler started" << endl << flush;
    // Redirect cin to input.txt if it exists
    ifstream infile("input.txt");
    if (infile) {
        cin.rdbuf(infile.rdbuf());
    }
    Scheduler s;
    vector<ParsedProcess> parsed;
    parseInput(parsed,s.algorithms,s.operation,s.lastInstant);
    for(auto &p:parsed) s.processes.push_back({p.name,p.arrival,p.service,p.priority});
    s.init();
    for(auto&a:s.algorithms){
        s.clearTimeline();
        s.execute(a.first,a.second);
        if(s.operation=="trace") { s.printTimeline(getAlgoName(a.first)); cout << endl; }
        else { s.printStats(getAlgoName(a.first)); cout << endl; }
        cout << "------------------------------" << endl << flush;
    }
}
