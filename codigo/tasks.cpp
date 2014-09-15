#include "tasks.h"
#include <time.h>
#include <stdlib.h>
#include <cmath>

using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConsola(int pid, vector<int> params){ //int n,int bmin, int bmax
	srand(time(NULL)); //Inicializo RNG
	for(int i = 0; i < params[0]; ++i){
		int random_number = rand() % (params[2]-params[1]+1) + params[1];//Genero numero random entre bmin y bmax
		uso_IO(pid, random_number); //Llamada bloqueante de 'random_number' ciclos 	
	}
}

void TaskBatch(int pid, vector<int> params){ //int total_cpu, int cant_bloqueos
	int total_cpu = params[0];
	int cant_bloqueos = params[1];
	int random_number, nuevo_bloqueo;

	vector<bool> deboBloquear = vector<bool>(total_cpu);
	for(int i = 0; i < total_cpu; i++) deboBloquear.push_back(false); 

	vector<int> pool = vector<int>(total_cpu);
	for(int i = 0; i < total_cpu; i++) pool.push_back(i);
	
	for(int contador = 0; contador < cant_bloqueos; contador++){
		random_number = floor(((double)rand()/RAND_MAX)*pool.size());
		nuevo_bloqueo = pool[random_number];
		deboBloquear[nuevo_bloqueo] = true;
		pool.erase(pool.begin()+random_number);
	}
	for(int i = 0; i < total_cpu; i++){
		if(deboBloquear[i]){
			uso_IO(pid, 1);
		}
		else{
			uso_CPU(pid,1);
		}
	}

}

void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConsola, 3);
	register_task(TaskBatch, 2);
}
