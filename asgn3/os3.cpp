#include<iostream>
#include<bits/stdc++.h>
#include "Process.cpp"
#include <time.h>
using namespace std;

// cd /mnt/d/F/mp/a3/

bool myfunction (Process i,Process j) { return (i.arrival<j.arrival); }
















#define nqueues 4
#define FCFS 1
#define RRB 2
#define PBS 3
#define SJF 4
vector<Process> gantt;
vector<Process> plist;
int t = 0;
int timequanta[] = {4,8};
int totalturnaround=0, totalwait =0;
int quanta[] = {4,8,8,100000};
int schedule_configuration[nqueues] = {RRB, RRB, RRB, FCFS};
vector<Process> stats;
unordered_map<int, int> learned_priorities;

deque<Process> q[nqueues];

void display_stats(std::deque<Process> v){

  int front_pid = v.front().pid;
  Process temp = v.front();

  do {

    std::cout << temp.pid << '\t' << temp.arrival  << '\t' << temp.burst <<  '\t' << temp.priority <<
    '\t' <<  temp.remaining << '\n';
    v.pop_front();
    v.push_back(temp);
    temp = v.front();
  } while(temp.pid != front_pid);


}

void display_stats(std::vector<Process> v){
  std::cout << "PID\tArrival\tBurst\tPriority" << '\n';
  for (int i = 0; i < v.size(); i++) {
    std::cout << v[i].pid << '\t' << v[i].arrival  << '\t' << v[i].burst <<  '\t' << v[i].priority << '\n';
  }
}
void display_stats(Process v){
  //(v.remaining == 0)
  //v.pid == 3;
  if (1) {
    std::cout << v.pid << '\t' << v.priority  << "\t\t" << v.arrival <<  "\t\t" << v.burst << "\t\t"<< v.completion <<  "\t\t"
     << v.remaining << "\t\t" << v.turnaround << "\t\t"  <<  v.wait << '\n';
     std::cout << v.execution[0] << " - " << v.execution[1] << '\n';
  }
  std::cout <<'\n';
}


bool queues_not_empty(deque<Process> (&q)[nqueues]){
  return (!q[0].empty()  || !q[1].empty() || !q[2].empty() || !q[3].empty());
}
void update_queues(deque<Process> (&q)[nqueues]){

  // check if the cpu is idle also

  static int i=0;


  while(!plist.empty() && t <=plist[i].arrival && i < plist.size() && !queues_not_empty(q)){
    // cpu idle
    t++;
  }


  //gathering processes under that t
  while(i < plist.size()) {
    if (plist[i].arrival < t) {
      plist[i].priority = learned_priorities[plist[i].pid]; //defaults to 0
      q[plist[i].priority].push_back(plist[i]);
      i++;
    }
    else{
      break;
    }
  }

}


void print_queues_status(deque<Process> (&q)[nqueues]){
  std::cout << "Printing deque status " << '\n';
  for (int i = 0; i < nqueues; i++) {
    display_stats(q[i]);
  }


}


int check_higher_queues(deque<Process> (&q)[nqueues]){
  for (int j = 0; j < nqueues; j++) {
    if (!q[j].empty()) {
      return j;
    }
  }
  return 0;
}




void schedule_fcfs(deque<Process> (&q)[nqueues], int i){


    Process &current = q[i].front();
    current.execution[0] = t;
    int j;
    // loop for a pre emptive fcfs
    while(current.remaining > 0){

      current.remaining--;
      t++;


      update_queues(q);
      j = check_higher_queues(q);
      if (j!=i) {
        current.execution[1] = t;
        // display_stats(current);
        stats.push_back(current);
        return;
      }
    }

    // t += current.remaining;
    current.remaining = 0;
    learned_priorities[current.pid] = current.priority;
    current.wait = t - current.burst - current.arrival;
    current.completion = t;
    current.turnaround = current.wait + current.burst;
    totalwait+=current.wait;
    totalturnaround+=current.turnaround;

    current.execution[1] = t;
    // display_stats(current);
    stats.push_back(current);
    q[i].pop_front();
    update_queues(q); // add new processes

}

void schedule_rrb(deque<Process> (&q)[nqueues], int i){

    Process current = q[i].front();
    current.execution[0] = t;

    if (current.remaining-quanta[i] > 0) {
      t += quanta[i];
      current.remaining-=quanta[i];
    }
    else{
      t += current.remaining;
      current.remaining = 0;
      learned_priorities[current.pid] = current.priority;
      current.wait = t - current.burst - current.arrival;
      current.completion = t;
      current.turnaround = current.wait + current.burst;
      totalwait+=current.wait;
      totalturnaround+=current.turnaround;
    }
    current.execution[1] = t;
    // display_stats(current);
    stats.push_back(current);

    q[i].pop_front();
    update_queues(q);


    // for changing the priortities
    if (current.remaining > 0) {
      if (i+1 < nqueues ) {
        current.priority++;
        q[i+1].push_back(current);

        // q[i].push_back(current);
      }

    }
    else{
      // process executed
    }




}




void schedule(deque<Process> (&q)[nqueues], int i){

  switch (schedule_configuration[i]) {
      case 1:
        schedule_fcfs(q,i);
        break;
      case 2:
        schedule_rrb(q,i);
        break;
    }

}


void print_final_stats(){
  // stable_sort(stats.begin(), stats.end(), [](Process a, Process b){return a.pid < b.pid;});
  int id=stats[0].pid;
  std::cout << "PID\tPriority\tArrival\t\tBurst\t\tCompletion\tRemaining\tTurnaround\tWait" << '\n';
  for (int i = 0 ; i < stats.size() ; i++) {
    if (id!=stats[i].pid) {
      id=stats[i].pid;
      std::cout << "-----------------------------------------------------------------------" << '\n';
    }
     display_stats(stats[i]);
  }

}

void initialize_processes(int pcount){

    default_random_engine arrival_generator, burst_generator;
    normal_distribution<double> arrival_distribution(5.0,1.0), burst_distribution(10,6);
    // uniform_int_distribution<int> distribution2(1,6);
    int pid, arrival, burst;

    for (int i = 0; i < pcount; i++) {
      pid = i;
      arrival = (int) arrival_distribution(arrival_generator) ;
      arrival = abs(arrival);

      burst =  (int) burst_distribution(burst_generator);
      burst = abs(burst);

      Process p(pid, arrival, burst);
      plist.push_back(p);
    }

    sort(plist.begin(), plist.end(), myfunction);


}


int main(){

  srand (time(NULL));
  int pcount = 7;

  initialize_processes(pcount);


  display_stats(plist);
  // return 0;







  int  i = 0;


  update_queues(q);  // initial update looking for first few processes


  i = check_higher_queues(q);

  while (queues_not_empty(q)){

    schedule(q,i);

    i = check_higher_queues(q);

  }
  print_final_stats();

  std::cout << "Average Wait :  " << totalwait/plist.size() << '\n';
  std::cout << "Average Turnaround :  " << totalturnaround/plist.size() << '\n';
  return 0;
}
