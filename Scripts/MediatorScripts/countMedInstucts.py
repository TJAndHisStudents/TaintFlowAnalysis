import sys
import re

def med_contains(med_function_list, med_function):
	


	for i in range(0, med_function_list.__len__()):

		if (med_function_list[i] == med_function):
			return 1
	return 0



if len(sys.argv) < 1:
	print "Usage: " + sys.argv[0] + " <input file> "

filename1 = sys.argv[1]
f_c = open(filename1)
med_function_list=[]



for line in f_c:
	string = "Function: "
	if string in line:
		med_string = line.split(" ")
		med_function = med_string[2]
		
		
		
		if(med_contains(med_function_list, med_function) == 0):
			med_function_list.append(med_function)


med_function_count=[0]
for i in range(1, med_function_list.__len__()):
	med_function_count.append(0)

f_c = open(filename1)
for line in f_c:
	for i in range(0, med_function_list.__len__()):
		function_string = "Function: " + med_function_list[i]
		if function_string in line:
			med_function_count[i] = med_function_count[i] + 1	


for i in range(0, med_function_list.__len__()):
	print med_function_list[i], med_function_count[i]







