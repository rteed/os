#include<iostream>
#include<bits/stdc++.h>
#include <time.h>
#include<math.h>
using namespace std;

bool sort_by_arrival (Process i,Process j) { return (i.arrival<j.arrival); }

void display_stats(std::deque<Process> &v){

  for (int i = 0 ; i < v.size() ; i++ ) {
      Process &current = v[i];
      cout << current.pid << '\t' << current.arrival  << '\t' << current.burst <<  '\t' << current.priority <<
      '\t' <<  current.remaining << '\n';
  }

}

void display_stats(std::vector<Process> &v){

  cout << "PID\tArrival\tBurst\tPriority" << '\n';
  for (int i = 0; i < v.size(); i++) {
    Process &current = v[i];
    std::cout << current.pid << '\t' << current.arrival  << '\t' << current.burst <<  '\t' << current.priority << '\n';
  }

}


void display_stats(Process &v){
  //(v.remaining == 0)
  //v.pid == 3;
  if (1) {
     cout << v.pid << '\t' << v.priority  << "\t\t" << v.arrival <<  "\t\t" << v.burst << "\t\t"<< v.completion <<  "\t\t"
     << v.remaining << "\t\t" << v.turnaround << "\t\t"  <<  v.wait << '\n';

     cout << v.execution[0] << " - " << v.execution[1] << '\n';
  }
  cout <<'\n';

}

bool queues_not_empty(deque<Process> (&q)[NQUEUES]){

  return (!q[0].empty()  || !q[1].empty() || !q[2].empty() || !q[3].empty());

}


void print_queues_status(deque<Process> (&q)[NQUEUES]){

  cout << "Printing deque status " << '\n';
  for (int i = 0; i < NQUEUES; i++) {
    display_stats(q[i]);
  }

}


int check_higher_queues(deque<Process> (&q)[NQUEUES]){

  for (int j = 0; j < NQUEUES; j++) {
    if (!q[j].empty()) {
      return j;
    }
  }
  return 0;
}




void compute_stdev(){
  int stdevwait = 0 , tempw = 0, temp = 0, stdevturnaround = 0;
  int meanwait = totalwait/plist.size(), meanturaround = totalturnaround/plist.size();
  for (int i = 0 ; i < stats.size() ; i++) {
    if (stats[i].remaining == 0) {
       tempw = pow((stats[i].wait - meanwait),2);
       temp = pow((stats[i].turnaround - meanturaround),2);
       stdevwait+=tempw;
       stdevturnaround+=temp;
    }
  }

  stdevwait = stdevwait / plist.size();
  stdevwait = sqrt(stdevwait);

  stdevturnaround = stdevturnaround / plist.size();
  stdevturnaround = sqrt(stdevturnaround);

  std::cout << "Wait standard deviation : " << stdevwait << '\n';
  std::cout << "Turnaround standard deviation : " << stdevturnaround << '\n';

}


void print_final_stats(){
  // stable_sort(stats.begin(), stats.end(), [](Process a, Process b){return a.pid < b.pid;});
  int id=stats[0].pid;
  cout << "PID\tPriority\tArrival\t\tBurst\t\tCompletion\tRemaining\tTurnaround\tWait" << '\n';
  for (int i = 0 ; i < stats.size() ; i++) {

    // to see finished processes states
    // if (stats[i].remaining == 0) {
    //    display_stats(stats[i]);
    // }

    // to see all execution states
    if (id!=stats[i].pid) {
      id=stats[i].pid;
      cout << "-----------------------------------------------------------------------" << '\n';
    }
    display_stats(stats[i]);

  }

}
