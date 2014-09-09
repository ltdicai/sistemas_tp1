#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cola_task.push(IDLE_TASK);//Encolamos la tarea IDLE para comenzar
}

SchedRR::~SchedRR() {

}

void SchedRR::load(int pid) {
	if(cola_task.size() == 1){//Si hay una sola tarea
		int tarea = cola_task.pop();
		if(tarea != IDLE_TASK){//No es la tarea IDLE
			cola_task.push(tarea);//La devuelvo a la cola
		}
	}
	cola_task.push(pid);//Encolamos la nueva tarea.
}

void SchedRR::unblock(int pid) {
	//La tarea termino de hacer IO
	cola_task.push(pid);//La encolamos nuevamente
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	int tarea = cola_task.pop();
	if(m == TICK){
	//La tarea debe ir al fondo de la cola
		cola_task.push(tarea);
	}
	else if(m == EXIT){
	//La tarea termino, se saca de la cola.
		if(cola_task.empty()){//Si ya no hay mas tareas
			cola_task.push(IDLE_TASK);//Encolamos la tarea IDLE
		}
	}
	//if(m == BLOCK){
	//La tarea realizo una llamada bloqueante
	//Debemos sacar la tarea de la cola y 
	//esperar su unblock donde volvera a ser encolada
	
	return cola_task.front();//Devolvemos la siguiente tarea a ejecutar 
}
