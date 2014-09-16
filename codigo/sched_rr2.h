#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include <map>
#include "basesched.h"

struct TareasCpu {
	std::queue<int> readyTasks;
	int cantBloqueadas;
	int tareaActual;

	int cantProcesos();
};

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        ~SchedRR2();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int next(int cpu);
		int cantCpu;
		std::vector<int> quantumMaximoDeCpu;
		std::vector<int> quantumActualDeCpu;
		std::vector<TareasCpu> tareaActualDeCpu;
		std::map<int,int> tareas_bloqueadas; //map<pid, cpu> for blocked tasks
};

#endif
