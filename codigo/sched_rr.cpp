#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	//for(unsigned int i = 0; i < argn.size(); i++) cout << argn[i] << " ";
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	quantumMaximoDeCpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum por core
	quantumActualDeCpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum actual por core
	tareaActualDeCpu = vector<int>(argn[0]);//Inicializo el arreglo de tarea actual por core

	for(int i = 0; i < argn[0]; i++){ //uno por cada core
		quantumMaximoDeCpu[i] = argn[i+1];//Cargo los quantum que me pasan por parametro
		quantumActualDeCpu[i] = 0;//Inicializo todo en 0
		tareaActualDeCpu[i] = IDLE_TASK; //al comienzo todas las cpu están corriendo IDLE
	}
}

SchedRR::~SchedRR() {

}

void SchedRR::load(int pid) {
	readyTasks.push(pid);//Encolamos la nueva tarea.
}

void SchedRR::unblock(int pid) {
	//La tarea termino de hacer IO
	readyTasks.push(pid);//La encolamos nuevamente
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	if(tareaActualDeCpu[cpu] == IDLE_TASK){ 
	//si estoy corriendo IDLE
		tareaActualDeCpu[cpu] = next(cpu); //prox tarea (si no hay tareas esperando es IDLE)
	}
	else{
		if(m == TICK){ 
		//es un TICK del cpu
			quantumActualDeCpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(quantumActualDeCpu[cpu] == quantumMaximoDeCpu[cpu]){ 
			//si se le terminó el quantum a la tarea actual
				readyTasks.push(tareaActualDeCpu[cpu]); //la encolo 
				quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
				tareaActualDeCpu[cpu] = next(cpu); //prox tarea
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{
		//si es un EXIT o un BLOCK para la vista del cpu es lo mismo
			//no se vuelve a encolar
			quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
			tareaActualDeCpu[cpu] = next(cpu); //prox tarea
		} 
	}
	return tareaActualDeCpu[cpu]; //Devolvemos la siguiente tarea a ejecutar | puede ser: IDLE, prox tarea o actual 
}

int SchedRR::next(int cpu){
	int next = IDLE_TASK; //si no hay prox tarea devuelvo IDLE

	if(readyTasks.size()>0){ //si hay tareas esperando
		next = readyTasks.front(); //devuelvo la primera de la cola
		readyTasks.pop(); //y la desencolo
	}
	return next; 
}
