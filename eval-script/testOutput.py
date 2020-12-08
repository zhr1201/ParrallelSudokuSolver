import subprocess
import sys
import getopt

#USAGE: python testOutput.py <numOfEmptyCells>

def main(argv):
	numOfCells = argv[0]
	proc = subprocess.Popen("./test-solver-speed.sh 1 " + str(numOfCells) + " 100", shell=True, stdout = subprocess.PIPE)
	serviceList = proc.communicate()[0]
	output_string = serviceList.decode("utf-8")
	output_arr = output_string.splitlines()
	output_val = []

	counter = 100
	i = 0
	while(counter > 0):
		counter -= 1;
		i += 2
		temp = output_arr[i].split(" ")
		output_val.append(float(temp[1]))
	
	# output_val records the time taken for all the tests
	#avg_val is the average time taken
	avg_val = 0
	for i in range(0,100):
		avg_val += output_val[i]
	avg_val /= 100
	print("the avg time taken for 100 problems is:  " + str(avg_val))

if __name__ == "__main__":
	main(sys.argv[1:])
