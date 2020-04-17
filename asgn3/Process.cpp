#include<iostream>
using namespace std;



class Process{
public:
  int pid;
  int priority;
  int burst;
  int arrival;
  int completion;
  int wait;
  int turnaround;
  int remaining;
  int execution[2];



  Process(int a, int b, int c){
    pid = a;
    arrival = b;
    burst = c;
    priority = 0;
    completion = 0;
    wait = -1;
    turnaround = 0;
    remaining = burst;
  }

  Process(int a, int b, int c, int d){
    pid = a;
    arrival = b;
    burst = c;
    priority = d;
    completion = 0;
    wait = -1;
    turnaround = 0;
    remaining = burst;
  }


};
