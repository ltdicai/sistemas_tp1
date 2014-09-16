#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cantCpu = argn[0];
	quantumMaximoDeCpu = vector<int>(cantCpu);//Inicializo el arreglo de quantum por core
	quantumActualDeCpu = vector<int>(cantCpu);//Inicializo el arreglo de quantum actual por core
	tareaActualDeCpu = vector<TareasCpu>(cantCpu);//Inicializo el arreglo de tareas por cpu

	for(int i = 0; i < cantCpu; i++){ //uno por cada core
		quantumMaximoDeCpu[i] = argn[i+1];//Cargo los quantum que me pasan por parametro
		quantumActualDeCpu[i] = 0;//Inicializo todo en 0
		tareaActualDeCpu[i].tareaActual = IDLE_TASK; //al comienzo todas las cpu están corriendo IDLE
	}
}

SchedRR2::~SchedRR2() {

}


void SchedRR2::load(int pid) {
	//recorro todos los cpu y agrego al que tiene menos procesos (Running + Blocked + Ready)
	int cpu_min = 0;
	for(int i=1; i< cantCpu; i++){
		if(tareaActualDeCpu[i].cantProcesos() < tareaActualDeCpu[cpu_min].cantProcesos()){
			cpu_min = i;
		}
	}
	tareaActualDeCpu[cpu_min].readyTasks.push(pid);
}

void SchedRR2::unblock(int pid) {
	int cpu = tareas_bloqueadas[pid]; //retorna el cpu en la que se estaba ejecutando
	tareaActualDeCpu[cpu].readyTasks.push(pid); //la encolo
	tareaActualDeCpu[cpu].cantBloqueadas--; //resto a la cant de bloqueadas del cpu
}

int SchedRR2::tick(int cpu, const enum Motivo m) {
	if(tareaActualDeCpu[cpu].tareaActual == IDLE_TASK){
	//si estoy corriendo IDLE
		tareaActualDeCpu[cpu].tareaActual = next(cpu); //prox tarea (si no hay tareas esperando es IDLE)
	}
	else{
		if(m == TICK){
		//es un TICK del cpu
			quantumActualDeCpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(quantumActualDeCpu[cpu] == quantumMaximoDeCpu[cpu]){ 
			//si se le terminó el quantum a la tarea actual
				tareaActualDeCpu[cpu].readyTasks.push(tareaActualDeCpu[cpu].tareaActual); //la encolo 
				quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
				tareaActualDeCpu[cpu].tareaActual = next(cpu); //prox tarea
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{ //BLOCK o EXIT
			if(m == BLOCK){
				tareas_bloqueadas.insert(std::pair<int, int>(tareaActualDeCpu[cpu].tareaActual, cpu)); //guardo <pid, cpu> en tareas_bloqueadas
				tareaActualDeCpu[cpu].cantBloqueadas++; //sumo a la cant de bloqueadas del cpu
			}
			quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
			tareaActualDeCpu[cpu].tareaActual = next(cpu); //prox tarea
		}
	}

  return tareaActualDeCpu[cpu].tareaActual;
}

int SchedRR2::next(int cpu) {
	int next = IDLE_TASK; //si no hay prox tarea devuelvo IDLE

	if(tareaActualDeCpu[cpu].readyTasks.size()>0){ //si hay tareas esperando
		next = tareaActualDeCpu[cpu].readyTasks.front(); //devuelvo la primera de la cola
		tareaActualDeCpu[cpu].readyTasks.pop(); //y la desencolo
	}
	return next; 
}

int TareasCpu::cantProcesos(){
	//(tareaActual!=IDLE_TASK) si no está ejecutando IDLE devuelve 1
	return cantBloqueadas + (tareaActual!=IDLE_TASK) + readyTasks.size(); 
}
