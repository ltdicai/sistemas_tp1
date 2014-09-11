#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	//for(unsigned int i = 0; i < argn.size(); i++) cout << argn[i] << " ";
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	max_quantum_por_cpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum por core
	actual_quantum_por_cpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum actual por core
	tarea_actual_por_cpu = vector<int>(argn[0]);//Inicializo el arreglo de tarea actual por core

	for(int i = 0; i < argn[0]; i++){ //uno por cada core
		max_quantum_por_cpu[i] = argn[i+1];//Cargo los quantum que me pasan por parametro
		actual_quantum_por_cpu[i] = 0;//Inicializo todo en 0
		tarea_actual_por_cpu[i] = IDLE_TASK; //al comienzo todas las cpu están corriendo IDLE
	}
}

SchedRR::~SchedRR() {

}

void SchedRR::load(int pid) {
	cola_task.push(pid);//Encolamos la nueva tarea.
}

void SchedRR::unblock(int pid) {
	//La tarea termino de hacer IO
	cola_task.push(pid);//La encolamos nuevamente
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	if(tarea_actual_por_cpu[cpu] == IDLE_TASK){ 
	//si estoy corriendo IDLE
		tarea_actual_por_cpu[cpu] = next(cpu); //prox tarea (si no hay tareas esperando es IDLE)
	}
	else{
		if(m == TICK){ 
		//es un TICK del cpu
			actual_quantum_por_cpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(actual_quantum_por_cpu[cpu] == max_quantum_por_cpu[cpu]){ 
			//si se le terminó el quantum a la tarea actual
				cola_task.push(tarea_actual_por_cpu[cpu]); //la encolo 
				actual_quantum_por_cpu[cpu] = 0; //vuelvo el quantum a cero
				tarea_actual_por_cpu[cpu] = next(cpu); //prox tarea
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{
		//si es un EXIT o un BLOCK para la vista del cpu es lo mismo
			//no se vuelve a encolar
			actual_quantum_por_cpu[cpu] = 0; //vuelvo el quantum a cero
			tarea_actual_por_cpu[cpu] = next(cpu); //prox tarea
		} 
	}
	return tarea_actual_por_cpu[cpu]; //Devolvemos la siguiente tarea a ejecutar | puede ser: IDLE, prox tarea o actual 
}

int SchedRR::next(int cpu){
	int next = IDLE_TASK; //si no hay prox tarea devuelvo IDLE

	if(cola_task.size()>0){ //si hay tareas esperando
		next = cola_task.front(); //devuelvo la primera de la cola
		cola_task.pop(); //y la desencolo
	}
	return next; 
}
