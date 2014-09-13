#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include <map>
#include "basesched.h"

struct TareasCpu {
	std::queue<int> cola_task;
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
		std::vector<int> max_quantum_por_cpu;
		std::vector<int> actual_quantum_por_cpu;
		std::vector<TareasCpu> tareas_por_cpu;
		std::map<int,int> tareas_bloqueadas; //map<pid, cpu> for blocked tasks
};

#endif
