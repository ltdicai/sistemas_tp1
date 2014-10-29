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
	last_exit = 0
	totalTasks = {-1:Task(0)}
	simuTotal=0
	for line in fin:
		line_n += 1
		words = line.split()
		# Extraer primera palabra
		command = words.pop(0) 
		if command == "#":
			if words[0] == "SETTINGS":
				simuTotal += 1
				for oid, value in tasks.iteritems():
					#cpu time
					totalTasks[oid].cpu_time += value.cpu_time
					#turn around
					totalTasks[oid].running_time += value.running_time
					#time IO
					totalTasks[oid].blocked_list += value.blocked_list
					#Response time
					totalTasks[oid].response_times += value.response_times
		elif command == "LOAD":
			# Cargo nueva tarea
			cycle = int(words[0]) 
			task = int(words[1])
			if task not in totalTasks:
				totalTasks[task] = Task(cycle);
			tasks[task] = Task(cycle)
		elif command == "CPU":
			# En el ciclo cycle se ejecuta la tarea task
			cycle = int(words[0]) 
			task = int(words[1])
			if tasks[task].blocked:
				 tasks[task].blocked = False
				 tasks[task].response(cycle)
			if task != -1:
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
		

	fin.close()

	for key, value in totalTasks.iteritems():
		if key != -1:
			#tourn-around
			value.running_time = float(value.running_time) / simuTotal
			#cpu time
			value.cpu_time = float(value.cpu_time) / simuTotal
			#time IO
			meanTimeIO = 0
			for timeIO in value.blocked_list:
				meanTimeIO += timeIO
			if len(value.blocked_list) !=0:
				meanTimeIO = float(meanTimeIO) / len(value.blocked_list)
			#response time
			meanResponseTime = 0
			for responseTime in value.response_times:
				meanResponseTime += responseTime
			if len(value.response_times) !=0:
				meanResponseTime = float(meanResponseTime) / len(value.response_times)
			print "Task {}: ".format(key)
			print "mean Tourn-around: {}".format(value.running_time)
			print "mean CPU time: {}".format(value.cpu_time)
			print "mean time IO: {}".format(meanTimeIO)
			print "mean response time: {}".format(meanResponseTime) + "\n"


	fout.write("\n")
	fout.close()
	return 0

if __name__ == "__main__":
	main(sys.argv)

#BASH PARA EJECUTAR MUCHAS VECES SIMUSCHED
#for semilla in $(seq 1 100); do ./simusched lote_ej10_1.tsk 1 1 2 SchedLottery 3 $semilla >> output.txt; done
