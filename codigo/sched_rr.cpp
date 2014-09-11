#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	//for(unsigned int i = 0; i < argn.size(); i++) cout << argn[i] << " ";
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	//cola_task.push(IDLE_TASK);//Encolamos la tarea IDLE para comenzar
	max_quantum_por_cpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum por core
	actual_quantum_por_cpu = vector<int>(argn[0]);//Inicializo el arreglo de quantum actual por core
	for(int i = 0; i < argn[0]; i++){
		max_quantum_por_cpu[i] = argn[i+1];//Cargo los quantum que me pasan por parametro
		//cout << argn[i+1] << endl;
		actual_quantum_por_cpu[i] = 0;//Inicializo todo en 0
	}
	cola_task.push(IDLE_TASK);
}

SchedRR::~SchedRR() {

}

void SchedRR::load(int pid) {
	if(cola_task.size() == 1){//Si hay una sola tarea
		int tarea = cola_task.front();
		if(tarea == IDLE_TASK){//Es la tarea IDLE
			cola_task.pop();//La desencolo
		}
	}
	cola_task.push(pid);//Encolamos la nueva tarea.
}

void SchedRR::unblock(int pid) {
	//La tarea termino de hacer IO
	//this.load(pid);//La 'cargamos' nuevamente
	if(cola_task.size() == 1){//Si hay una sola tarea
		int tarea = cola_task.front();
		if(tarea == IDLE_TASK){//Es la tarea IDLE
			cola_task.pop();//La desencolo
		}
	}
	cola_task.push(pid);//Encolamos la nueva tarea.
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	int pausa;
	//cin>> pausa;
	//cout << "//// Quantum del cpu actual: " << actual_quantum_por_cpu[cpu] << endl;
	//cout << "Nuevo quantum del cpu actual: " << actual_quantum_por_cpu[cpu] << endl;
	int tarea = cola_task.front();//Veo la tarea que acaba de ejecutarse
	if(m == TICK){//Es un tick del cpu
		if(tarea != IDLE_TASK){
			//cout << "////actual vs max" << actual_quantum_por_cpu[cpu] << " , " << max_quantum_por_cpu[cpu] <<endl;
			if(actual_quantum_por_cpu[cpu] == max_quantum_por_cpu[cpu]){
				actual_quantum_por_cpu[cpu] = 0;//Reseteo su quantum
				cola_task.pop();//Quito a la tarea del tope de la cola
				cola_task.push(tarea);//La tarea debe ir al fondo de la cola
			}
			actual_quantum_por_cpu[cpu]++;//Sumo 1 al quantum del cpu actual
		} 
	}
	else if(m == EXIT){
		//La tarea termino, no vuelve a la cola
		cola_task.pop();//Quito a la tarea del tope de la cola
		actual_quantum_por_cpu[cpu] = 0;//Reseteo el quantum del cpu
		if(cola_task.empty()){//Si ya no hay mas tareas
			cola_task.push(IDLE_TASK);//Encolamos la tarea IDLE	
		}
	}
	else if(m == BLOCK){
	//La tarea realizo una llamada bloqueante
	//Debemos sacar la tarea de la cola y 
	//esperar su unblock donde volvera a ser encolada
		cola_task.pop();
		if(cola_task.empty()){ //era la única tarea, debemos ejecutar IDLE
			cola_task.push(IDLE_TASK);
		}
	}
	
	return cola_task.front();//Devolvemos la siguiente tarea a ejecutar 
}
