#include "tasks.h"
#include <time.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>

using namespace std;

int global = 0;

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
	double aux;

	//Inicizalizamos vector de flags que indiquen si la tarea se bloqueará o no
	vector<bool> deboBloquear = vector<bool>();
	for(int i = 0; i < total_cpu; i++) deboBloquear.push_back(false); 

	//Inicializamos el pool de donde extraer momentos
	vector<int> pool = vector<int>();
	for(int i = 0; i < total_cpu; i++) pool.push_back(i);

	//Elegimos pseudoaleatoriamente cant_bloqueos momentos en los cuales bloquear
	for(int contador = 0; contador < cant_bloqueos; contador++){
		//Calculamos una posicion aleatoria del pool de momentos.
		aux = ((double)rand()/RAND_MAX)*pool.size();
		random_number = floor(aux);

		nuevo_bloqueo = pool[random_number]; // La tarea se bloqueará en el tick nuevo_bloqueo
		//Marcamos que la tarea debe bloquearse en el tick nuevo_bloqueo
		deboBloquear[nuevo_bloqueo] = true;
		//Eliminamos el nuevo_bloqueo del pool de momentos, ya que no lo
		// debemos elegir nuevamente
		pool.erase(pool.begin()+random_number);
	}
	//Ciclo principal de la tarea
	for(int i = 0; i < total_cpu; i++){
		if(deboBloquear[i]){
			//Hacemos una llamada bloqueante de 1 ciclo
			// Esto consume en total 2 ciclos (1 por la llamada, otro por
			// el bloqueo en si)
			uso_IO(pid, 1);
		}
		else{
			//Utilizamos el cpu durante un ciclo.
			uso_CPU(pid, 1);
		}
	}
	//Se consume un ciclo mas por el exit
	//Tiempo total = 1*total_cpu + 1*cant_bloqueos + 1*exit
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
