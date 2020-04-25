
#include<iostream>
#include<bits/stdc++.h>
#include "Process.cpp"
#include<math.h>
#include <time.h>
using namespace std;

// cd /mnt/d/F/mp/a3/


















#define NQUEUES 4
#define FCFS 1
#define RRB 2
#define PBS 3
#define SJF 4

vector<Process> gantt;
vector<Process> plist;
vector<Process> stats;
deque<Process> q[NQUEUES];
unordered_map<int, int> learned_priorities;


int t = 0;
int totalturnaround=0, totalwait =0;

int quanta[] = {4,8,8,100000};
int timequanta[] = {4,8};
int schedule_configuration[NQUEUES] = {RRB, RRB, RRB, FCFS};





#include "utils.cpp"
void update_queues(deque<Process> (&q)[NQUEUES]){



  static int i=0;


  while(!plist.empty() && t <= plist[i].arrival && i < plist.size() && !queues_not_empty(q)){
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


void schedule_fcfs(deque<Process> (&q)[NQUEUES], int i){


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

void schedule_rrb(deque<Process> (&q)[NQUEUES], int i){

    Process &current = q[i].front();
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
      if (i+1 < NQUEUES ) {
        current.priority++;
        q[i+1].push_back(current);
        // q[i].push_back(current);
      }

    }
    else{
      // process executed
    }




}

void schedule_rrb_prempt(deque<Process> (&q)[NQUEUES], int i){

    Process &current = q[i].front();
    current.execution[0] = t;
    int j;
    if (current.remaining-quanta[i] > 0) {

      // loop for a pre emptive fcfs
      while(current.remaining > 0){

        current.remaining--;
        t++;

        if (t - current.execution[0] >= quanta[i]) {
          break; // up for pop and push back
        }
        update_queues(q);
        j = check_higher_queues(q);
        if (j!=i) {
          current.execution[1] = t;
          // display_stats(current);
          stats.push_back(current);
          return;
        }
      }
    }
    else{

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
      if (i+1 < NQUEUES ) {
        current.priority++;
        q[i+1].push_back(current);
        // q[i].push_back(current);
      }
      else{
        q[i].push_back(current);
      }
    }
    else{
      // process executed
    }




}




void schedule(deque<Process> (&q)[NQUEUES], int i){

  switch (schedule_configuration[i]) {
      case 1:
        schedule_fcfs(q,i);
        break;
      case 2:
        // schedule_rrb(q,i);
        schedule_rrb_prempt(q,i);
        break;
    }

}

void initialize_from_textfile(){


    int pid, arrival, burst, priority;
    ifstream fin;
    fin.open ("input.txt");
    string str;
    getline(fin, str);

    int a;

    int i=0;
    plist.clear();
    while (fin >> a) {
      i++;
      switch (i) {
        case 1:
          pid = a;
          break;
        case 2:
          arrival = a;
          break;
        case 3:
          burst = a;
          break;
        case 4:
          priority = a;
          plist.push_back(Process(pid, arrival, burst, priority));
          i=0;
          break;
        default:
          std::cout << "Invalid" << '\n';
          break;
      }
    }


    fin.close();

    sort(plist.begin(), plist.end(), sort_by_arrival);

    display_stats(plist);


}

void initialize_randomly(int pcount){

    plist.clear();
    srand (time(NULL));
    default_random_engine arrival_generator, burst_generator;
    normal_distribution<double> arrival_distribution(6,4), burst_distribution(5,3);
    // uniform_int_distribution<int> arrival_distribution(20,25), burst_distribution(20,25);

    int pid, arrival, burst, priority;

    for (int i = 0; i < pcount; i++) {
      pid = i;
      // pid = i%20; // for recurring processes
      arrival = (int) arrival_distribution(arrival_generator) ;
      arrival = abs(arrival);

      burst =  (int) burst_distribution(burst_generator);
      burst = abs(burst);

      // added for MLQ
      priority = rand()%NQUEUES;

      Process p(pid, arrival, burst, priority);
      plist.push_back(p);
    }

    sort(plist.begin(), plist.end(), sort_by_arrival);

    // writing to the text file
    ofstream fout;

    fout.open ("input.txt");
    fout << "PID | \tArrival | \tBurst | \tPriority" << '\n';
    for (int i = 0; i < plist.size(); i++) {
      fout << plist[i].pid << '\t' << plist[i].arrival  << '\t' << plist[i].burst <<  '\t' << plist[i].priority << '\n';
    }
    fout.close();

}


int main(){

  srand (time(NULL));
  int pcount = 50;

  // initialize_randomly(pcount);
  initialize_from_textfile();

  std::cout << "With Feed back" << '\n';

  // display_stats(plist);
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

  compute_stdev();
  return 0;
}
