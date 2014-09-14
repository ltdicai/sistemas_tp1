#include "sched_lottery.h"

using namespace std;

SchedLottery::SchedLottery(vector<int> argn) {
  //suponemos argn[0]=cant_cores; argn[1]=quantum; argn[2]=semilla;

	cout << "la concha de tu madre all boys";
	cout << argn[0] << " " << argn[1] << " " << argn[2];
	cantCpu = argn[0];
	quantumMaximo = argn[1];
	int semilla = argn[2];
	Task idle = Task(IDLE_TASK, -1);
	//inicializo taskList de cores
	for(int i = 0; i < cantCpu; i++){ //uno por cada core
		
		quantumActualDeCpu[i] = 0;//Inicializo todo en 0
		tareaActualDeCpu[i] = idle; //al comienzo todas las cpu están corriendo IDLE
	}
	//randomizo con la semilla recibida
	srand(semilla);
}

SchedLottery::~SchedLottery() {

}

void SchedLottery::load(int pid) {
  load(pid,0);
}

void SchedLottery::load(int pid,int deadline) {
	//Llega una tarea
	//Lo encolo en la lista de tareas
	cout << endl << deadline << endl;
	int cantTickets;
	if(deadline == 0){
		cantTickets = 1;
	}
	else{
		cantTickets = floor(10/deadline) + 1; //La cantidad de tickets otorgados es proporcional al deadline 
	}
	totalTickets += cantTickets; //Aumento la cantidad de tickets
	Task nuevaTarea = Task(pid,cantTickets);//Creo nueva tarea
	readyTasks.push_back(nuevaTarea); //Inserto nueva tarea a la lista de tareas por ejecutar
}

void SchedLottery::unblock(int pid) {
	Task desbloqueada = tareasBloqueadas[pid];
	readyTasks.push_back(desbloqueada);
}

int SchedLottery::tick(int cpu, const enum Motivo m) {
	if(tareaActualDeCpu[cpu].pid == IDLE_TASK){ 
	//si estoy corriendo IDLE
		tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
	}
	else{
		if(m == TICK){
			quantumActualDeCpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(quantumActualDeCpu[cpu] == quantumMaximo){ 
			//si se le terminó el quantum a la tarea actual
				readyTasks.push_back(tareaActualDeCpu[cpu]); //la pongo en ready 
				quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
				tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{ //BLOCK o EXIT
			if(m == BLOCK){
				//Compenso Tickets()
				tareasBloqueadas.insert(std::pair<int, Task>(tareaActualDeCpu[cpu].pid, tareaActualDeCpu[cpu])); 
				//guardo <pid, Task> en tareas_bloqueadas

			}
			quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
			tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
		}
	}
  return tareaActualDeCpu[cpu].pid;
}

Task SchedLottery::lottery(){
	int ticketDorado = floor((rand()/RAND_MAX)*totalTickets + 1); 
	int sumaTickets = 0;
	unsigned int it = 0;
	while(it < readyTasks.size() && sumaTickets < ticketDorado){
		sumaTickets += readyTasks[it].cantTickets;
		it++; 
	}
	return readyTasks[it];
}
