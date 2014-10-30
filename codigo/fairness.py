#!/usr/bin/env python
import sys

class Task(object):
	''' Task object for storing values related to task '''
	def __init__(self, load_time):
		self.load_time = load_time
		self.cpu_time = 0
		self.blocked_list = []
		self.response_times = []
		self.running_time = 0
		self.blocked = False
		self.last_block = -1

	def exit(self, cycle):
		self.running_time = cycle - self.load_time

	def unblock(self, cycle):
		self.blocked_list.append(cycle - self.last_block)

	def response(self, cycle):
		self.response_times.append(cycle - self.last_block)

	@property
	def io_time(self):
		return sum(self.blocked)

	def __print__(self):
		return "hola"


def main(argv):
	fin = sys.stdin
	fout = sys.stdout
	# Chequear si hay archivo de entrada
	if "-i" in argv:
		index = argv.index("-i")
		# Extraer "-i"
		argv.pop(index)
		# Extraer nombre de archivo
		file_in_name = argv.pop(index)
		try:
			fin = open(file_in_name, 'r')
		except Exception, exc:
			print "Archivo '%s' no encontrado" % file_in_name
			return -1
	else:
		sys.stderr.write("Using standard input\n")
	if "-o" in argv:
		index = argv.index("-o")
		# Extraer "-o"
		argv.pop(index)
		# Extraer nombre de archivo
		file_out_name = argv.pop(index)
		fout = open(file_out_name, 'w')


	tasks = {-1:Task(0)}
	line_n = 0
	last_exit = 0;
	cpuPerCicleTask = {-1: {}, 0: {}, 1: {}}
	total_simu = 0
	for line in fin:
		line_n += 1
		words = line.split()
		# Extraer primera palabra
		command = words.pop(0) 
		if command != "#" and command != "UNBLOCK":
			cycle = int(words[0]) 
			if (cycle % 10 == 0 or cycle == 21 or cycle == 51) and cycle != 0:
				# print "-----------Cycle: {}-----------".format(cycle)
				for oid, value in tasks.iteritems():
					if cycle not in cpuPerCicleTask[oid]:
						cpuPerCicleTask[oid][cycle] = 0
					cpuPerCicleTask[oid][cycle] += value.cpu_time
					# taskname = oid
					# if taskname == -1:
					# 	taskname = "Idle"
					# print "Task {}:".format(taskname)
					# print "\tTime CPU: {}".format(value.cpu_time)
		if command == "#":
			if words[0] == "SETTINGS":
				total_simu+= 1
				#reinicio para el nuevo simulador
				tasks = {-1:Task(0)}

		elif command == "LOAD":
			# Cargo nueva tarea
			cycle = int(words[0]) 
			task = int(words[1])
			tasks[task] = Task(cycle)
		elif command == "CPU":
			# En el ciclo cycle se ejecuta la tarea task
			cycle = int(words[0]) 
			task = int(words[1])
			if tasks[task].blocked:
				 tasks[task].blocked = False
				 tasks[task].response(cycle)
			#if task != -1:
			tasks[task].cpu_time += 1

		elif command == "BLOCK":
			# En el ciclo cycle la tarea task esta bloqueada
			cycle = int(words[0]) 
			task = int(words[1])
			# Si no estaba bloqueada, recien empezo
			if not tasks[task].blocked:
				tasks[task].blocked = True
				tasks[task].last_block = cycle

		elif command == "UNBLOCK":
			# En el ciclo cycle la tarea task se desbloqueo
			cycle = int(words[0])
			task = int(words[1])
			#tasks[task].blocked = False
			tasks[task].unblock(cycle)

		elif command == "EXIT":
			# En el ciclo cycle la tarea fue desalojada
			cycle = int(words[0]) 
			task = int(words[1])
			last_exit = int(cycle)
			tasks[task].exit(cycle)

		else:
			pass
		#	sys.stderr.write("Skip line {} '{}'".format(line_n, line))

	print "Total CPU time:", last_exit
	# total_cpu = float(last_exit)
	for oid, value in tasks.iteritems():
		if oid != -1:
			taskname = oid
			ciclos = ""
			cpuPorCiclo = ""
			for key in sorted(cpuPerCicleTask[oid]):
				ciclos += "{} & ".format(key)
				#divido por la cant de simulaciones
				cpuPerCicleTask[oid][key] /= total_simu
				cpuPorCiclo += "{} & ".format(cpuPerCicleTask[oid][key])
			if oid == 0:
				print "\n Ciclo & " + ciclos
			print "\n Tarea {} & ".format(taskname) + cpuPorCiclo
		# print "\tTime CPU: {}".format(value.cpu_time)

		# print "\tTime CPU(%): {}".format(100*(value.cpu_time/total_cpu))
		# print "\tTurn-around: {}".format(value.running_time)
		# print "\tTime IO: {}".format(value.blocked_list)
		# print "\tResponse times: {}".format(value.response_times)
		# if value.response_times:
		# 	response_time_avg = sum(value.response_times)/float(len(value.response_times))
		# else:
		# 	response_time_avg = -1
		# print "\tResponse times(AVG): {}".format(response_time_avg)

		

	fin.close()


	fout.write("\n")
	fout.close()
	return 0

if __name__ == "__main__":
	main(sys.argv)
