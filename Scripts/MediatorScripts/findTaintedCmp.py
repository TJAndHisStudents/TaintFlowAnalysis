import sys
import re

if len(sys.argv) < 2:
	print "Usage: " + sys.argv[0] + " <input file> "

filename1 = sys.argv[1]
mediator = sys.argv[2]
f_c = open(filename1)
found_mediator = 0

print mediator

for line in f_c:
	string = "define i32 @" + mediator
	if string in line:
		found_mediator = 1
	
	cmp_string = "icmp"
	strcmp_string = "strcmp"
	store_string = "store"
	load_string = "load"
	ret_string = "ret"
	taint_string = "!TAINT"

	if cmp_string in line:
		if (found_mediator == 1):
			if taint_string in line:
				print line

	if strcmp_string in line:
		if (found_mediator == 1):
			if taint_string in line:
				print line

	if store_string in line:
		if (found_mediator == 1):
			if taint_string in line:
				print line

	if load_string in line:
		if (found_mediator == 1):
			if taint_string in line:
				print line

	if ret_string in line:
		if (found_mediator == 1):
			if taint_string in line:
				print line

	

	


	string2 = "define i8* @sub_51D48"
	if string2 in line:
		found_mediator = 0
