#include "sched_lottery.h"

using namespace std;

#define CANT_TICKETS_INICIAL 10  

SchedLottery::SchedLottery(vector<int> argn){ //suponemos argn[0]=cant_cores; argn[1]=quantum; argn[2]=semilla;

	cantCpu = argn[0];
	quantumMaximo = argn[1];
	int semilla = argn[2];
	Task idle = Task(IDLE_TASK, -1);

	//Inicializo los vectores
	readyTasks = vector<Task>(0);
	quantumActualDeCpu = vector<int>(cantCpu);
	tareaActualDeCpu = vector<Task>(cantCpu);

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
	int cantTickets = CANT_TICKETS_INICIAL;
	totalTickets += cantTickets; //Aumento la cantidad de tickets
	Task nuevaTarea = Task(pid,cantTickets);//Creo nueva tarea
	readyTasks.push_back(nuevaTarea); //Inserto nueva tarea a la lista de tareas por ejecutar
}

void SchedLottery::unblock(int pid) {
	Task desbloqueada = tareasBloqueadas[pid];
	tareasBloqueadas.erase(pid);
	totalTickets += desbloqueada.cantTickets;

	readyTasks.push_back(desbloqueada);
}

int SchedLottery::tick(int cpu, const enum Motivo m) {
	//cout << "TICK TOC" << endl;
	if(tareaActualDeCpu[cpu].pid == IDLE_TASK){ 
	//si estoy corriendo IDLE
		//cout << green <<"Let's play lottery!" << clearatt << endl;
		tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
	}
	else{
		if(m == TICK){
			quantumActualDeCpu[cpu]++;//Sumo 1 al quantum del cpu actual
			if(quantumActualDeCpu[cpu] == quantumMaximo){ 
			//si se le terminó el quantum a la tarea actual
				readyTasks.push_back(tareaActualDeCpu[cpu]); //la pongo en ready 
				quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
		//		cout << green <<"Let's play lottery!" << clearatt << endl;
				tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
		//		cout << "'Bingo!' shouts " << tareaActualDeCpu[cpu].pid << endl;
			}
			///si no se le terminó el quantum a la tarea actual sigue ejecutando
		}
		else{ //BLOCK o EXIT
			//Disminuyo la cantidad de tickets totales
			totalTickets -= tareaActualDeCpu[cpu].cantTickets;
			if(m == BLOCK){
				quantumActualDeCpu[cpu]++;//Sumo 1 al quantum del cpu actual
				//Agrego tickets compensatorios por no terminar el quantum
				//compensarTickets(cpu);
				tareasBloqueadas.insert(pair<int, Task>(tareaActualDeCpu[cpu].pid, tareaActualDeCpu[cpu])); 
				//guardo <pid, Task> en tareas_bloqueadas

			}
			quantumActualDeCpu[cpu] = 0; //vuelvo el quantum a cero
			tareaActualDeCpu[cpu] = lottery(); //Hacemos la loteria
		}
	}
  return tareaActualDeCpu[cpu].pid;
}

Task SchedLottery::lottery(){
	// for debugging
	// int i=0;
	// while(i < readyTasks.size()){
	// 	cout << "-------------------readyTasks[" << i << "] pid " << readyTasks[i].pid << " cantTickets " << readyTasks[i].cantTickets << endl;
	// 	i++;
	// }
	
	if(readyTasks.size() == 0){
		return Task(IDLE_TASK, -1);
	}
	else{
		double random = (double)rand();
		random = random/RAND_MAX; //ahora random = numero entre 0 y 1
		//cout << "-------------------totalTickets" << totalTickets << endl;
		random = random*totalTickets;
		int ticketDorado = floor(random);
		//cout << "-------------------The winning number is " << ticketDorado << endl;
		int sumaTickets = 0;
		unsigned int it = 0;
		//Itero la lista de tareas sumando los tickets
		//Me detengo uno antes de la ultima tarea, porque
		//en ese caso ya se que la ultima es la ganadora
		while(it < readyTasks.size()-1 && sumaTickets < ticketDorado){
			sumaTickets += readyTasks[it].cantTickets;
			if (sumaTickets > ticketDorado){
				//entonces esta es la tarea ganadora
				break;
			}
			it++; 
		}
		Task nueva = readyTasks[it];
		readyTasks.erase(readyTasks.begin()+it);
		//cout << "# WINNER " << nueva.pid << endl;
		return nueva;
	}
}

void SchedLottery::compensarTickets(int cpu){
	//quantumActualDeCpu[cpu] nunca es cero porque como minimo usan 1 cpu para pedir el bloqueo
	double fraccionQuantum = double(quantumActualDeCpu[cpu])/double(quantumMaximo);
	/*si consumio una fracción F del quantum
	* la compensacion es de CANT_TICKETS_INICIAL/F
	*/
	if (fraccionQuantum != 0 ){
		tareaActualDeCpu[cpu].cantTickets += floor(CANT_TICKETS_INICIAL/fraccionQuantum);
	}
}
