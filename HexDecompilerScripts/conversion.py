#!/usr/bin/python
# Convert Hex-Rays decompilation output to standard C

import sys
import re

if len(sys.argv) < 1:
    print "Usage: " + sys.argv[0] + " <input file>"

filename1 = sys.argv[1]

f_c = open(filename1)

p = re.compile(r'[\s]*[A-Z0-9]*\(([a-z0-9]*)\) =')

for line in f_c:

	#Integer based types
	line = re.sub("__int8", r"char", line)
   	line = re.sub("__int16", r"int", line)
  	line = re.sub("__int32", r"long int", line)
    	line = re.sub("__int64", r"long long int", line)

	#Various other changes
	line = re.sub("FILE *", r"char *", line)

	line = re.sub("const char", r"char", line)
	line = re.sub("_BOOL4", r"int", line)
	line = re.sub("DIR ", r"char*", line)

	# Various Line & Substring Removals
	line = re.sub("__fastcall", r"", line)
	line = re.sub("__noreturn", r"", line)

	if "#error" in line:
		line = ""

	
	# Un-comment out declaration of errno_location call.
	string = "// int *_errno_location(void);"
	if string in line:
		line = "int *_errno_location(void);"

	string = "// const unsigned int **_ctype_b_loc(void);"
	if string in line:
		line = "const unsigned int **_ctype_b_loc(void);"

	string = "// extern _UNKNOWN __gmon_start__; weak"
	if string in line:
		line = "extern _UNKNOWN __gmon_start__;"



	m  = re.match(p, line)
    	if m is not None:
       		line = re.sub(p, m.group(1)+" =", line)

    	print line,
	

#Add in code here to attempt to compile the program using the clang command
# $ clang -w -S -emit-llvm -ferror-limit=100000 sample_save.c -o sample.bc



