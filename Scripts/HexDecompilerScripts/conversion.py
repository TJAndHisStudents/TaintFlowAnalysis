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

	#Initial change of <defs.h> to "defs.h"
	line = re.sub("#include <defs.h>", r'#include "defs.h"', line)

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

	# Get rid of #error lines.
	if "#error" in line:
		line = ""

	if "using guessed type" in line:
		line = ""

	line = re.sub('__spoils<.+>', "", line)

	# Un-comment out declaration of errno_location call.
	string = "// int *_errno_location(void);"
	if string in line:
		line = "int *_errno_location(void);"

	# Un-comment out declaration of _h_errno_location call.
	string = "// int *_h_errno_location(void);"
	if string in line:
		line = "int *_h_errno_location(void);"

	# Un-comment out declaration of _ctype_b_loc call.
	string = "// const unsigned int **_ctype_b_loc(void);"
	if string in line:
		line = "const unsigned int **_ctype_b_loc(void);"

	# Un-comment out declaration of ctype_to_lower call.
	string = "// const unsigned int **_ctype_tolower_loc(void);"
	if string in line:
		line = "const unsigned int **_ctype_tolower_loc(void);"	

	# Un-comment out gmon_start declaration.
	string = "// extern _UNKNOWN __gmon_start__; weak"
	if string in line:
		line = "extern _UNKNOWN __gmon_start__;"

	
	
	##### BEGIN C++ BASED FIXES #####

	line = re.sub("::", r"", line)
	line = re.sub("~", r"", line)
	line = re.sub("operator ", r"", line)


	##### Take care of std::basic_ios #####

	string = "// int  stdbasic_ios<char,stdchar_traits<char>>init(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdbasic_ios<char,stdchar_traits<char>>init(_DWORD, _DWORD);\n"
	
	string = "// int  stdbasic_ios<char,stdchar_traits<char>>clear(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdbasic_ios<char,stdchar_traits<char>>clear(_DWORD, _DWORD);\n"
	
	string = "// int  stdbasic_ios<char,stdchar_traits<char>>basic_ios(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_ios<char,stdchar_traits<char>>basic_ios(_DWORD);\n"

	string = "stdbasic_ios<char,stdchar_traits<char>>init"
	if string in line:
		line = re.sub("stdbasic_ios<char,stdchar_traits<char>>init", r"stdbasic_iosinit", line)

	string = "stdbasic_ios<char,stdchar_traits<char>>clear"
	if string in line:
		line = re.sub("stdbasic_ios<char,stdchar_traits<char>>clear", r"stdbasic_iosclear", line)
	
	string = "stdbasic_ios<char,stdchar_traits<char>>basic_ios"
	if string in line:
		line = re.sub("stdbasic_ios<char,stdchar_traits<char>>basic_ios", r"stdbasic_iosbasic_ios", line)
	
	
	##### Take care of std::operator #####

	string = "// int  stdoperator<<<stdchar_traits<char>>(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdoperator<<<stdchar_traits<char>>(_DWORD, _DWORD);\n"

	string = "// int  stdoperator<<<char,stdchar_traits<char>,stdallocator<char>>(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdoperator<<<char,stdchar_traits<char>,stdallocator<char>>(_DWORD, _DWORD);\n"

	string = "stdoperator<<<stdchar_traits<char>>"
	if string in line:
		line = re.sub("stdoperator<<<stdchar_traits<char>>", r"stdoperator", line)

	string = "stdoperator<<<char,stdchar_traits<char>,stdallocator<char>>"
	if string in line:
		line = re.sub("stdoperator<<<char,stdchar_traits<char>,stdallocator<char>>", r"stdoperator", line)

	##### Take care of std:endl #####

	string = "// int  stdendl<char,stdchar_traits<char>>(_DWORD); weak"
	if string in line:
		line = "int  stdendl<char,stdchar_traits<char>>(_DWORD);\n"

	string = "stdendl<char,stdchar_traits<char>>"
	if string in line:
		line = re.sub("stdendl<char,stdchar_traits<char>>", r"stdendl", line)

	##### Take care of std::locale::locale #####

	string = "// _DWORD stdlocalelocale(stdlocale *this); idb"
	if string in line:
		line = "_DWORD stdlocalelocale(stdlocale *this);\n"

	##### Take care of std::ios_baseInit #####

	string = "// _DWORD stdios_baseInitInit(stdios_baseInit *this); idb"
	if string in line:
		line = "_DWORD stdios_baseInitInit(stdios_baseInit *this);\n"

	##### Take care of std::ios_base::ios_base #####

	string = "// _DWORD stdios_baseios_base(stdios_base *this); idb"
	if string in line:
		line = "_DWORD stdios_baseios_base(stdios_base *this);\n"

	##### Take care of std::basic_filebuf #####


	string = "// int  stdbasic_filebuf<char,stdchar_traits<char>>open(_DWORD, _DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdbasic_filebuf<char,stdchar_traits<char>>open(_DWORD, _DWORD, _DWORD);\n"

	string = "// int  stdbasic_filebuf<char,stdchar_traits<char>>basic_filebuf(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_filebuf<char,stdchar_traits<char>>basic_filebuf(_DWORD);\n"
	
	string = "// int  stdbasic_filebuf<char,stdchar_traits<char>>close(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_filebuf<char,stdchar_traits<char>>close(_DWORD);\n"

	string = "stdbasic_filebuf<char,stdchar_traits<char>>open"
	if string in line:
		line = re.sub("stdbasic_filebuf<char,stdchar_traits<char>>open", r"stdbasic_filebufopen", line)

	string = "stdbasic_filebuf<char,stdchar_traits<char>>basic_filebuf"
	if string in line:
		line = re.sub("stdbasic_filebuf<char,stdchar_traits<char>>basic_filebuf", r"stdbasic_filebufbasic_filebuf", line)

	string = "stdbasic_filebuf<char,stdchar_traits<char>>close"
	if string in line:
		line = re.sub("stdbasic_filebuf<char,stdchar_traits<char>>close", "stdbasic_filebufclose", line)

	##### Take care of std::ostream::operator #####

	string = "int  stdostreamoperator<<(_DWORD, _DWORD);"
	if string in line:
		line = "int  stdostreamoperator(_DWORD, _DWORD);\n"

	string = "stdostreamoperator<<"
	if string in line:
		line = re.sub("stdostreamoperator<<", r"stdostreamoperator", line)

	##### Take care of std::__basic_file #####

	string = "// int  std__basic_file<char>__basic_file(_DWORD); weak"
	if string in line:
		line = "int  std__basic_file<char>__basic_file(_DWORD);\n"

	string = "// int  std__basic_file<char>is_open(_DWORD); weak"
	if string in line:
		line = "int  std__basic_file<char>is_open(_DWORD);\n"
	
	string = "std__basic_file<char>is_open"
	if string in line:
		line = re.sub("std__basic_file<char>is_open", "std__basic_fileis_open", line)


	string = "std__basic_file<char>__basic_file"
	if string in line:
		line = re.sub("std__basic_file<char>__basic_file", r"std__basic_file__basic_file", line)

	##### Take care of std::__ostream_insert #####

	string = "// int  std__ostream_insert<char,stdchar_traits<char>>(_DWORD, _DWORD, _DWORD); weak"
	if string in line:
		line = "int  std__ostream_insert<char,stdchar_traits<char>>(_DWORD, _DWORD, _DWORD);\n"

	string = "std__ostream_insert<char,stdchar_traits<char>>"
	if string in line:
		line = re.sub("std__ostream_insert<char,stdchar_traits<char>>", r"std__ostream_insert", line)

	##### Take care of std::ostream::put #####
	
	string = "// _DWORD __cdecl stdostreamput(stdostream *this, char); idb"
	if string in line:
		line = "_DWORD __cdecl stdostreamput(stdostream *this, char);\n"

	##### Take care of std::istream::operator #####
	
	string = "// int  stdistreamoperator>>(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdistreamoperator>>(_DWORD, _DWORD);\n"

	string = "stdistreamoperator"
	if string in line:
		line = re.sub("stdistreamoperator>>", r"stdistreamoperator", line)

	##### Take care of std::ctype #####
	
	string = "// int  stdctype<char>_M_widen_init(_DWORD); weak"
	if string in line:
		line = "int  stdctype<char>_M_widen_init(_DWORD);\n"

	string = "stdctype<char>_M_widen_init"
	if string in line:
		line = re.sub("stdctype<char>_M_widen_init", r"stdctype_M_widen_init", line)

	##### Take care of std::ostream::_M_insert #####
	
	string = "// int  stdostream_M_insert<void const*>(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdostream_M_insert<void const*>(_DWORD, _DWORD);\n"

	string = "stdostream_M_insert<void const*>"
	if string in line:
		line = re.sub("stdostream_M_insert<void const\*>", r"stdostream_M_insert", line)

	string = "stdostream_M_insert<long>"
	if string in line:
		line = re.sub("stdostream_M_insert<long>", r"stdostream_M_insert", line)



	##### Take care of OPEN::DBX functions #####

	string = "// _DWORD OpenDBXResultfinish(OpenDBXResult *this); idb"
	if string in line:
		line = "_DWORD OpenDBXResultfinish(OpenDBXResult *this);\n"

	string = "// int  OpenDBXConnoperator=(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  OpenDBXConnoperator(_DWORD, _DWORD);\n"

	string = "OpenDBXConnoperator="
	if string in line:
		line = re.sub("OpenDBXConnoperator=", r"OpenDBXConnoperator", line)


	##### Take care of std::basic_string::stream #####

	string = "// int  stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>basic_stringstream(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>basic_stringstream(_DWORD);\n"

	string = "stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>basic_stringstream"
	if string in line:
		line = re.sub("stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>basic_stringstream", r"stdbasic_stringstreambasic_stringstream", line)

	##### Take care of std::basic_string::buf #####

	string = "// int  stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>_M_sync(_DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>_M_sync(_DWORD, _DWORD);\n"

	string = "stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>_M_sync"
	if string in line:
		line = re.sub("stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>_M_sync", r"stdbasic_stringbuf_M_sync", line)

	##### Take care of std::getline #####

	string = "// int  stdgetline<char,stdchar_traits<char>,stdallocator<char>>(_DWORD, _DWORD, _DWORD); weak"
	if string in line:
		line = "int  stdgetline<char,stdchar_traits<char>,stdallocator<char>>(_DWORD, _DWORD, _DWORD);\n"

	string = "stdgetline<char,stdchar_traits<char>,stdallocator<char>>"
	if string in line:
		line = re.sub("stdgetline<char,stdchar_traits<char>,stdallocator<char>>", r"stdgetline", line)


	##### Take care of std::basic_ifstream #####

	string = "// int  stdbasic_ifstream<char,stdchar_traits<char>>basic_ifstream(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_ifstream<char,stdchar_traits<char>>basic_ifstream(_DWORD);\n"

	string = "stdbasic_ifstream<char,stdchar_traits<char>>basic_ifstream"
	if string in line:
		line = re.sub("stdbasic_ifstream<char,stdchar_traits<char>>basic_ifstream", r"stdbasic_ifstreambasic_ifstream", line)

	##### Take care of std::istream::getline #####

	string = "// _DWORD __cdecl stdistreamgetline(stdistream *this, char *, int, char); idb"
	if string in line:
		line = "_DWORD __cdecl stdistreamgetline(stdistream *this, char *, int, char);\n"



	##### Take care of IO_putc and IO_getc #####

	string = "// int IO_getc(_IO_char **fp);"
	if string in line:
		line = "int IO_getc(_IO_char **fp);\n"

	string = "// int IO_putc(_IO_char **fp);"
	if string in line:
		line = "int IO_getc(_IO_char **fp);\n"

	##### Take care of pthread_cancel
	string = "// int pthread_cancel(pthread_t th);"
	if string in line:
		line = "int pthread_cancel(pthread_t th);\n"

	
	##### Take care of OpenDBXStmt #####

	string = "// _DWORD __cdecl OpenDBXStmtStmt(OpenDBXStmt *this); idb"
	if string in line:
		line = "_DWORD __cdecl OpenDBXStmtStmt(OpenDBXStmt *this);\n"

	##### Take care of stdbasic_ofstream #####

	string = "// int  stdbasic_ofstream<char,stdchar_traits<char>>basic_ofstream(_DWORD); weak"
	if string in line:
		line = "int  stdbasic_ofstream<char,stdchar_traits<char>>basic_ofstream(_DWORD);\n"
	string = "stdbasic_ofstream<char,stdchar_traits<char>>basic_ofstream"
	if string in line:
		line = re.sub("stdbasic_ofstream<char,stdchar_traits<char>>basic_ofstream", "stdbasic_ofstreambasic_ofstream", line)
	
	
	##### Handle std::_Destroy_aux #####

	string = "std_Destroy_aux<false>__destroy<stdstring *>"
	if string in line:
		line = re.sub(r"std_Destroy_aux<false>__destroy<stdstring \*>", r"std_Destroy_aux__destroy", line)


	string = "// int  cgiccCgiccoperator()(_DWORD, _DWORD, _DWORD); weak"
	if string in line:
		line = "int cgiccCgiccoperator(_DWORD, _DWORD, _DWORD);\n"

	string = "cgiccCgiccoperator()"
	if string in line:
		line = re.sub("cgiccCgiccoperator\(\)", r"cgiccCgiccoperator", line)

		
	##### Handle std::basic_string::stream #####

	string = "// int  stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>str(_DWORD, _DWORD); weak"
	if string in line:
		line = "// int  stdbasic_stringstreamstr(_DWORD, _DWORD);\n"


	string = "stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>str"
	if string in line:
		line = re.sub(r"stdbasic_stringstream<char,stdchar_traits<char>,stdallocator<char>>str", r"stdbasic_stringstreamstr", line)


	string = "stdvector<stdstring,stdallocator<stdstring>>vector"
	if string in line:
		line = re.sub(r"stdvector<stdstring,stdallocator<stdstring>>vector", r"stdvectorvector", line)

	string = "stdvector<stdstring,stdallocator<stdstring>>_M_insert_aux"
	if string in line:
		line = re.sub(r"stdvector<stdstring,stdallocator<stdstring>>_M_insert_aux", r"stdvector_M_insert_aux", line)


	# MISC
	
	string = "stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>basic_stringbuf"
	if string in line:
		line = re.sub("stdbasic_stringbuf<char,stdchar_traits<char>,stdallocator<char>>basic_stringbuf", r"stdbasic_stringbufbasic_stringbuf", line)

	string = "stdstring_S_construct<char *>"
	if string in line:
		line = re.sub("stdstring_S_construct<char \*>", r"stdstring_S_construct", line)



	m  = re.match(p, line)
    	if m is not None:
       		line = re.sub(p, m.group(1)+" =", line)
	

    	print line,
	

#Add in code here to attempt to compile the program using the clang command
# $ clang -w -S -emit-llvm -ferror-limit=100000 sample_save.c -o sample.bc



