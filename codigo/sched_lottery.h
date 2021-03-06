#ifndef __SCHED_LOT__
#define __SCHED_LOT__

#include "basesched.h"
#include <map>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "iocolors.h"

struct Task
{
  Task():pid(0),cantTickets(-1){};
	Task(int pid, int cantTickets): pid(pid), cantTickets(cantTickets){};
	int pid;
	int cantTickets;
};

typedef std::map<int,Task> mapaIntTask;

class SchedLottery : public SchedBase {
  public:
    SchedLottery(std::vector<int> argn);
        ~SchedLottery();
    virtual void load(int pid);
    virtual void load(int pid, int deadline);
    virtual void unblock(int pid);
    virtual int tick(int cpu, const enum Motivo m);

  private:

  	std::vector<Task> readyTasks;
  	std::vector<Task> tareaActualDeCpu;
  	std::vector<int> quantumActualDeCpu;
  	
  	int quantumMaximo;
  	int totalTickets;
  	int cantCpu;
    mapaIntTask tareasBloqueadas; //map<pid, Task> for blocked tasks

    Task lottery();

    void compensarTickets(int cpu);

};

#endif
