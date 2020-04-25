
#include<iostream>
#include<bits/stdc++.h>
#include "Process.cpp"

#include <time.h>
#include<math.h>
using namespace std;

// cd /mnt/d/F/mp/a3/


















#define NQUEUES 4
#define FCFS 1
#define RRB 2
#define PBS 3
#define SJF 4
#define AGFACTOR 20

vector<Process> gantt;
vector<Process> plist;
vector<Process> stats;
deque<Process> q[NQUEUES];

int t = 0;
int totalturnaround=0, totalwait =0;

int quanta[] = {4,8,8,100000};
int timequanta[] = {4,8};
int schedule_configuration[NQUEUES] = {RRB, RRB, RRB, FCFS};






#include "utils.cpp"
void update_queues(deque<Process> (&q)[NQUEUES]){

  static int i=0;
  // !plist.empty() - while some processes are yet to be served
  // t <= plist[i].arrival - global t
  // !queues_not_empty(q) -  all the queues are empty
  while(!plist.empty() && t <= plist[i].arrival && i < plist.size() && !queues_not_empty(q)){
    // cpu idle
    t++;
  }

  // gathering processes under that t
  // i should always point to the process yet to be brought into the queues
  while(i < plist.size()) {
    if (plist[i].arrival < t) {
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


void schedule_fcfs_ag(deque<Process> (&q)[NQUEUES], int i){


    Process &current = q[i].front();
    current.execution[0] = t;
    int j;
    // loop for a pre emptive fcfs
    while(current.remaining > 0){

      current.remaining--;
      t++;


      current.wait = t - current.burst - current.arrival + current.remaining;
      update_queues(q);
      j = check_higher_queues(q);


      if (j!=i) {
        current.execution[1] = t;
        // display_stats(current);


        stats.push_back(current);
        if (current.wait > AGFACTOR) {
          current.priority = 0;
          q[i].pop_front();
          // q[i-1].push_back(current);
          q[0].push_back(current);
        }

        return;
      }

    }

    // t += current.remaining;
    current.remaining = 0;
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
      current.wait = t - current.burst - current.arrival + current.remaining;

    }

    else{
      t += current.remaining;
      current.remaining = 0;

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
      if (i < NQUEUES ) {
        q[i].push_back(current);
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

    while (t - current.execution[0] < quanta[i] && current.remaining > 0) {
      current.remaining--;
      t++;
      current.wait = t - current.burst - current.arrival + current.remaining;

      update_queues(q);
      j = check_higher_queues(q);
      if (j!=i) {
        current.execution[1] = t; // process preempt
        // display_stats(current);
        stats.push_back(current);

        if (current.wait > AGFACTOR) {
          current.priority = 0;
          q[i].pop_front();
          // q[i-1].push_back(current);
          q[0].push_back(current);
        }

        return;

      }

    }


      if (current.remaining == 0) {
        current.remaining = 0;
        current.wait = t - current.burst - current.arrival;
        current.completion = t;
        current.turnaround = current.wait + current.burst;
        totalwait+=current.wait;
        totalturnaround+=current.turnaround;

        current.execution[1] = t;
        // display_stats(current);
        stats.push_back(current);

        q[i].pop_front();
        update_queues(q);
      }



      else if (current.remaining > 0) {
        current.execution[1] = t;
        // display_stats(current);
        stats.push_back(current);

        q[i].pop_front();
        update_queues(q);
        q[i].push_back(current);

      }

    update_queues(q);


}


void schedule(deque<Process> (&q)[NQUEUES], int i){

  switch (schedule_configuration[i]) {
      case 1:
        // schedule_fcfs(q,i);
        schedule_fcfs_ag(q,i);
        break;
      case 2:
        // schedule_rrb(q,i);
        schedule_rrb_prempt(q,i);
        break;
    }

}




void initialize_from_textfile(){

    plist.clear();
    int pid, arrival, burst, priority;
    ifstream fin;
    fin.open ("input.txt");
    string str;
    getline(fin, str);

    int a;

    int i=0;

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

    // int burstsum = 0;
    // for (auto current : plist) {
    //   burstsum += current.burst;
    // }
    // std::cout << "burst sum : " <<  burstsum << '\n';
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


  int pcount = 16;

  initialize_randomly(pcount);
  // initialize_from_textfile();


  // return 0;

  std::cout << "No Feed back" << '\n';



  int  i = 0;


  update_queues(q);  // initial update looking for first few processes


  i = check_higher_queues(q);

  while (queues_not_empty(q)){

    schedule(q,i);

    i = check_higher_queues(q);

  }
  print_final_stats();

  cout << "Average Wait :  " << totalwait/plist.size() << '\n';
  cout << "Average Turnaround :  " << totalturnaround/plist.size() << '\n';
  compute_stdev();
  return 0;

}
