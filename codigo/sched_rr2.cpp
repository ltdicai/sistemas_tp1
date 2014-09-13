#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cantCpu = argn[0];
	max_quantum_por_cpu = vector<int>(cantCpu);//Inicializo el arreglo de quantum por core
	actual_quantum_por_cpu = vector<int>(cantCpu);//Inicializo el arreglo de quantum actual por core
	tareas_por_cpu = vector<TareasCpu>(cantCpu);//Inicializo el arreglo de tareas por cpu

	for(int i = 0; i < cantCpu; i++){ //uno por cada core
		max_quantum_por_cpu[i] = argn[i+1];//Cargo los quantum que me pasan por parametro
		actual_quantum_por_cpu[i] = 0;//Inicializo todo en 0
		tareas_por_cpu[i].tareaActual = IDLE_TASK; //al comienzo todas las cpu están corriendo IDLE
	}
}

SchedRR2::~SchedRR2() {

}


void SchedRR2::load(int pid) {
	//recorro todos los cpu y agrego al que tiene menos procesos (Running + Blocked + Ready)
	int cpu_min = 0;
	for(int i=1; i< cantCpu; i++){
		if(tareas_por_cpu[i].cantProcesos() < tareas_por_cpu[cpu_min].cantProcesos()){
			cpu_min = i;
		}
	}
	tareas_por_cpu[cpu_min].cola_task.push(pid);
}

void SchedRR2::unblock(int pid) {
	int cpu = tareas_bloqueadas[pid]; //retorna el cpu en la que se estaba ejecutando
	tareas_por_cpu[cpu].cola_task.push(pid); //la encolo
	tareas_por_cpu[cpu].cantBloqueadas--; //resto a la cant de bloqueadas del cpu
}

int SchedRR2::tick(int cpu, const enum Motivo m) {
	if(tareas_por_cpu[cpu].tareaActual == IDLE_TASK){
	//si estoy corriendo IDLE
		tareas_por_cpu[cpu].tareaActual = next(cpu); //prox tarea (si no hay tareas esperando es IDLE)
	}
	else{
		if(m == TICK){
		//es un TICK del cpu
			actual_quantum_por_cpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(actual_quantum_por_cpu[cpu] == max_quantum_por_cpu[cpu]){ 
			//si se le terminó el quantum a la tarea actual
				tareas_por_cpu[cpu].cola_task.push(tareas_por_cpu[cpu].tareaActual); //la encolo 
				actual_quantum_por_cpu[cpu] = 0; //vuelvo el quantum a cero
				tareas_por_cpu[cpu].tareaActual = next(cpu); //prox tarea
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{ //BLOCK o EXIT
			if(m == BLOCK){
				tareas_bloqueadas.insert(std::pair<int, int>(tareas_por_cpu[cpu].tareaActual, cpu)); //guardo <pid, cpu> en tareas_bloqueadas
				tareas_por_cpu[cpu].cantBloqueadas++; //sumo a la cant de bloqueadas del cpu
			}
			actual_quantum_por_cpu[cpu] = 0; //vuelvo el quantum a cero
			tareas_por_cpu[cpu].tareaActual = next(cpu); //prox tarea
		}
	}

  return tareas_por_cpu[cpu].tareaActual;
}

int SchedRR2::next(int cpu) {
	int next = IDLE_TASK; //si no hay prox tarea devuelvo IDLE

	if(tareas_por_cpu[cpu].cola_task.size()>0){ //si hay tareas esperando
		next = tareas_por_cpu[cpu].cola_task.front(); //devuelvo la primera de la cola
		tareas_por_cpu[cpu].cola_task.pop(); //y la desencolo
	}
	return next; 
}

int TareasCpu::cantProcesos(){
	//(tareaActual!=IDLE_TASK) si no está ejecutando IDLE devuelve 1
	return cantBloqueadas + (tareaActual!=IDLE_TASK) + cola_task.size(); 
}
