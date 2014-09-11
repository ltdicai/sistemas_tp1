#ifndef __SCHED_RR__
#define __SCHED_RR__

#include <vector>
#include <queue>
#include "basesched.h"

class SchedRR : public SchedBase {
	public:
		SchedRR(std::vector<int> argn);
        ~SchedRR();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

private:
		int next(int cpu);
		std::queue<int> cola_task;
		std::vector<int> tarea_actual_por_cpu;
		std::vector<int> max_quantum_por_cpu;
		std::vector<int> actual_quantum_por_cpu;
};

#endif
