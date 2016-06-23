#!/usr/bin/python
# Convert C++ library functions into dummy functions that will pass C compilation.
# Remove all "::" scope resolution operators.
# Declare typedefs for library sructure types.

import sys
import re

# List of identifiers that need to be declared.
REGISTER1 = "int _R1;"
REGISTER2 = "int _R2;"
REGISTER3 = "int _R3;"
ZERO_FLAG = "int _ZF;"

  ## These are arbitrary identifiers input by hex-rays ##
EVP_VAR = "void* EVP_rc4;"
CS_VAR = "void* __CS__;"
V1C_VAR = "int v1C;"
V1D_VAR = "int v1D;"

# List of C++ types that need to be defined.
STDSTRING = "typedef char* stdstring;"
STDLOCALE = "typedef char* stdlocale;"
STDIOS_BASEINIT = "typedef char* stdios_baseInit;"
STDIOSBASE = "typedef char* stdios_base;"
STDOSTREAM = "typedef char* stdostream;"
STDISTREAM = "typedef char* stdistream;"
STDRUNTIMEERR = "typedef char* stdruntime_error;"
CGITHING = "typedef char* cgiccCgicc;"

OPNDBXCONN = "typedef char* OpenDBXConn;"
OPNDBXSTMT = "typedef char* OpenDBXStmt;"
OPNDBXRESULT = "typedef char* OpenDBXResult;"

IO_TYPE = "typedef char* _IO_char;"

# List of standard headers missing
STDLIB_HEADER = "#include <stdlib.h>"
UNISTD_HEADER = "#include <unistd.h>"

# List of headers seen in Engagement 2
NET_DB_HEADER = "#include <netdb.h>"
TIME_HEADER = "#include <time.h>"
SYSTIME_HEADER = "#include <sys/time.h>"
SYS_EPOLL_HEADER = "#include <sys/epoll.h>"
PWD_HEADER = "#include <pwd.h>"
GRP_HEADER = "#include <grp.h>"
UIO_HEADER = "#include <sys/uio.h>"
SHADOW_HEADER = "#include <shadow.h>"
SYS_SOCKET_HEADER = "#include <sys/socket.h>"
TERMIOS_HEADER = "#include <termios.h>"
ZLIB_HEADER = "#include <zlib.h>"
DIRENT_HEADER = "#include <dirent.h>"
POLL_HEADER = "#include <poll.h>"
SIGNAL_HEADER = "#include <signal.h>"
MNTENT_HEADER = "#include <mntent.h>"
GETOPT_HEADER = "#include <getopt.h>"
UTSNAME_HEADER = "#include <sys/utsname.h>"
UTMP_HEADER = "#include <utmp.h>"
REGEX_HEADER = "#include <regex.h>"

# List of identifiers that need to be defined


# Takes in the error file and checks if 'entity' is needed based on error.
def check_need_entity(e_filename, entity):
	
	error_fd = open(e_filename)
	
	for line in error_fd:
		if entity in line:
			# Need for entity found in error file		
			return 1
	# Not needed.
	return 0

# Takes in proper declaration and checks if it has already been added.
def check_entity_added(c_filename, entity):
	print entity + " checked"
	
	c_fd = open(c_filename)

	for line in c_fd:
		# Entity declaration found.		
		if entity in line:
			return 1

	# Not found.
	return 0

# Writes the entity declaration or definition to file.
def add_entity(c_filename, entity):
	with open(c_filename, 'r+') as f:
		content = f.read()
		f.seek(0, 0)
		f.write(entity + '\n' + content)


def check_headers(c_filename, e_filename):
	
	# Check if we need sys/uio.h using iovec struct
	
	if (check_need_entity(e_filename, "struct iovec") == 1):
		if (check_entity_added(c_filename, UIO_HEADER) == 0):
			add_entity(c_filename, UIO_HEADER)

	# Check if we need pwd.h using passwd struct

	if (check_need_entity(e_filename, "struct passwd") == 1):
		if (check_entity_added(c_filename, PWD_HEADER) == 0):
			add_entity(c_filename, PWD_HEADER)

	# Check if we need shadow.h using shadow struct

	if (check_need_entity(e_filename, "struct spwd") == 1):
		if (check_entity_added(c_filename, SHADOW_HEADER) == 0):
			add_entity(c_filename, SHADOW_HEADER)

	# Check if we need termios.h using termios struct

	if (check_need_entity(e_filename, "struct termios") == 1):
		if (check_entity_added(c_filename, TERMIOS_HEADER) == 0):
			add_entity(c_filename, TERMIOS_HEADER)

	# Check if we need grp.h using group struct

	if (check_need_entity(e_filename, "struct group") == 1):
		if (check_entity_added(c_filename, GRP_HEADER) == 0):
			add_entity(c_filename, GRP_HEADER)
	
	# Check if we need netdb.h using hostent struct
	
	if (check_need_entity(e_filename, "struct hostent") == 1):
		if (check_entity_added(c_filename, NET_DB_HEADER) == 0):
			add_entity(c_filename, NET_DB_HEADER)

	# Check if we need netdb.h using addrinfo struct
	
	if (check_need_entity(e_filename, "struct addrinfo") == 1):
		if (check_entity_added(c_filename, NET_DB_HEADER) == 0):
			add_entity(c_filename, NET_DB_HEADER)


	# Check if we need sys/epoll.h using epoll_event struct
	
	if (check_need_entity(e_filename, "struct epoll_event") == 1):
		if (check_entity_added(c_filename, SYS_EPOLL_HEADER) == 0):
			add_entity(c_filename, SYS_EPOLL_HEADER)

	# Check if we need time.h using tm struct
	
	if (check_need_entity(e_filename, "struct tm") == 1):
		if (check_entity_added(c_filename, TIME_HEADER) == 0):
			add_entity(c_filename, TIME_HEADER)

	# Check if we need sys/socket.h using sockaddr struct

	if (check_need_entity(e_filename, "struct sockaddr") == 1):
		if (check_entity_added(c_filename, SYS_SOCKET_HEADER) == 0):
			add_entity(c_filename, SYS_SOCKET_HEADER)

	# Check if we need epoll.h using pollfd struct

	if (check_need_entity(e_filename, "struct pollfd") == 1):
		if (check_entity_added(c_filename, POLL_HEADER) == 0):
			add_entity(c_filename, POLL_HEADER)

	# Check if we need zlib.h using z_streamp identifier

	if (check_need_entity(e_filename, "z_streamp") == 1):
		if (check_entity_added(c_filename, ZLIB_HEADER) == 0):
			add_entity(c_filename, ZLIB_HEADER)

	# Check if we need signal.h using __sighandler_t struct
	
	if (check_need_entity(e_filename, "__sighandler_t") == 1):
		if (check_entity_added(c_filename, SIGNAL_HEADER) == 0):
			add_entity(c_filename, SIGNAL_HEADER)
	
	# Check if we need mntent.h using mntent struct

	if (check_need_entity(e_filename, "struct mntent") == 1):
		if (check_entity_added(c_filename, MNTENT_HEADER) == 0):
			add_entity(c_filename, MNTENT_HEADER)

	# Check if we need sys/time.h using tms struct

	if (check_need_entity(e_filename, "struct tms") == 1):
		if (check_entity_added(c_filename, SYSTIME_HEADER) == 0):
			add_entity(c_filename, SYSTIME_HEADER)

	# Check if we need sys/utsname.h using utsname struct

	if (check_need_entity(e_filename, "struct utsname") == 1):
		if (check_entity_added(c_filename, UTSNAME_HEADER) == 0):
			add_entity(c_filename, UTSNAME_HEADER)

	# Check if we need getopt.h using option struct

	if (check_need_entity(e_filename, "struct option") == 1):
		if (check_entity_added(c_filename, GETOPT_HEADER) == 0):
			add_entity(c_filename, GETOPT_HEADER)

	# Check if we need utmp.h using utmp struct

	if (check_need_entity(e_filename, "struct utmp") == 1):
		if (check_entity_added(c_filename, UTMP_HEADER) == 0):
			add_entity(c_filename, UTMP_HEADER)

	# Check if we need regex.h using regex_t type

	if (check_need_entity(e_filename, "regex_t") == 1):
		if (check_entity_added(c_filename, REGEX_HEADER) == 0):
			add_entity(c_filename, REGEX_HEADER)


def check_standard_headers(filename):
	
	if (check_entity_added(filename, STDLIB_HEADER) == 0):
		add_entity(filename, STDLIB_HEADER)

	if (check_entity_added(filename, UNISTD_HEADER) == 0):
		add_entity(filename, UNISTD_HEADER)

def check_identifiers(c_filename, e_filename):

	# Check if we are missing std::locale identifier.

	if (check_need_entity(e_filename, "undeclared identifier \'stdlocale\'") == 1):
		if (check_entity_added(c_filename, STDLOCALE) == 0):
			add_entity(c_filename, STDLOCALE)


	# Check if we are missing zero flag identifier

	if (check_need_entity(e_filename, "_ZF") == 1):
		if (check_entity_added(c_filename, ZERO_FLAG) == 0):
			add_entity(c_filename, ZERO_FLAG)

	# Check if we are missing register identifiers
	
	if (check_need_entity(e_filename, "_R1") == 1):
		if (check_entity_added(c_filename, REGISTER1) == 0):
			add_entity(c_filename, REGISTER1)

	if (check_need_entity(e_filename, "_R2") == 1):
		if (check_entity_added(c_filename, REGISTER2) == 0):
			add_entity(c_filename, REGISTER2)
	
	if (check_need_entity(e_filename, "_R3") == 1):
		if (check_entity_added(c_filename, REGISTER3) == 0):
			add_entity(c_filename, REGISTER3)




	# Check if we are missing identifier for EVP_rc4

	if (check_need_entity(e_filename, "EVP_rc4") == 1):
		if (check_entity_added(c_filename, EVP_VAR) == 0):
			add_entity(c_filename, EVP_VAR)
	
	# Check if we are missing identifier for __CS__

	if (check_need_entity(e_filename, "__CS__") == 1):
		if (check_entity_added(c_filename, CS_VAR) == 0):
			add_entity(c_filename, CS_VAR)

	# Check if we are missing identifier for v1C
	
	if (check_need_entity(e_filename, "undeclared identifier \'v1C\'") == 1):
		if (check_entity_added(c_filename, V1C_VAR) == 0):
			add_entity(c_filename, V1C_VAR)

	# Check if we are missing identifier for v1D

	if (check_need_entity(e_filename, "undeclared identifier \'v1D\'") == 1):
		if (check_entity_added(c_filename, V1D_VAR) == 0):
			add_entity(c_filename, V1D_VAR)	


def check_typedefs(c_filename, e_filename):

	# Check if we are missing std::string typedef.
	
	if (check_need_entity(e_filename, "unknown type name \'stdstring\'") == 1):
		if (check_entity_added(c_filename, STDSTRING) == 0):
			add_entity(c_filename, STDSTRING)


	# Check if we are missing std::locale typedef.

	if (check_need_entity(e_filename, "unknown type name \'stdlocale\'") == 1):
		if (check_entity_added(c_filename, STDLOCALE) == 0):
			add_entity(c_filename, STDLOCALE)

	
	# Check if we are missing std::ios::base_Init typedef.

	if (check_need_entity(e_filename, "unknown type name \'stdios_baseInit\'") == 1):
		if (check_entity_added(c_filename, STDIOS_BASEINIT) == 0):
			add_entity(c_filename, STDIOS_BASEINIT)

	# Check if we are missing std::ios_base typedef.

	if (check_need_entity(e_filename, "unknown type name \'stdios_base\'") == 1):
		if (check_entity_added(c_filename, STDIOSBASE) == 0):
			add_entity(c_filename, STDIOSBASE)

	# Check if we are missing std::ostream typedef.	

	if (check_need_entity(e_filename, "unknown type name \'stdostream\'") == 1):
		if (check_entity_added(c_filename, STDOSTREAM) == 0):
			add_entity(c_filename, STDOSTREAM)

	# Check of we are missing std::istream typedef.

	if (check_need_entity(e_filename, "unknown type name \'stdistream\'") == 1):
		if (check_entity_added(c_filename, STDISTREAM) == 0):
			add_entity(c_filename, STDISTREAM)

	# Check if we are missing std::runtime_error typedef.

	if (check_need_entity(e_filename, "unknown type name \'stdruntime_error\'") == 1):
		if (check_entity_added(c_filename, STDRUNTIMEERR) == 0):
			add_entity(c_filename, STDRUNTIMEERR)

	# Check if we are missing _IO_Char typedef.
	
	if (check_need_entity(e_filename, "unknown type name \'_IO_char\'") == 1):
		if (check_entity_added(c_filename, IO_TYPE) == 0):
			add_entity(c_filename, IO_TYPE)


	# Check if we are missing OpenDBXConn typedef.

	if (check_need_entity(e_filename, "unknown type name \'OpenDBXConn\'") == 1):
		if (check_entity_added(c_filename, OPNDBXCONN) == 0):
			add_entity(c_filename, OPNDBXCONN)

	# Check if we are missing OpenDBXStmt typedef.

	if (check_need_entity(e_filename, "unknown type name \'OpenDBXStmt\'") == 1):
		if (check_entity_added(c_filename, OPNDBXSTMT) == 0):
			add_entity(c_filename, OPNDBXSTMT)

	# Check if we are missing OpenDBXResult typedef.
	
	if (check_need_entity(e_filename, "unknown type name \'OpenDBXResult\'") == 1):
		if (check_entity_added(c_filename, OPNDBXRESULT) == 0):
			add_entity(c_filename, OPNDBXRESULT)
	
	# Check if we are missing cgiccCgicc typedef.

	if (check_need_entity(e_filename, "undeclared identifier \'cgiccCgicc\'") == 1):
		if (check_entity_added(c_filename, CGITHING) == 0):
			add_entity(c_filename, CGITHING)

def check_unks(c_filename, e_filename):
	e_fd = open(e_filename)

	r = re.compile('undeclared identifier \'unk_.+\'')

	for line in e_fd:
		if r.search(line):
			string = line.split(" ")
			unk = re.sub('[\']', "", string[6])
			unk = re.sub('[\\n]', "", unk)
			entity = "void* " + unk + ";"
			#print header			
			if (check_entity_added(c_filename, entity) == 0):
				add_entity(c_filename, entity)

def check_offs(c_filename, e_filename):
	e_fd = open(e_filename)

	r = re.compile('undeclared identifier \'off_.+\'')

	for line in e_fd:
		if r.search(line):
			string = line.split(" ")
			off = re.sub('[\']', "", string[6])
			off = re.sub('[\\n]', "", off)
			entity = "void* " + off + ";"
			#print header			
			if (check_entity_added(c_filename, entity) == 0):
				add_entity(c_filename, entity)

def check_dwords(c_filename, e_filename):
	e_fd = open(e_filename)

	r = re.compile('undeclared identifier \'dword_.+\'')

	for line in e_fd:
		if r.search(line):
			string = line.split(" ")
			dword = re.sub('[\']', "", string[6])
			dword = re.sub('[\\n]', "", dword)
			entity = "int " + dword + ";"
			#print header			
			if (check_entity_added(c_filename, entity) == 0):
				add_entity(c_filename, entity)


def usage():
	print "Usage: " + sys.argv[0] + " <c-file> <error-file>"

def main():
	
	if (len(sys.argv) < 2):
		usage()

	c_filename = sys.argv[1]
	e_filename = sys.argv[2]

	p = re.compile(r'[\s]*[A-Z0-9]*\(([a-z0-9]*)\) =')

	check_unks(c_filename, e_filename)
	check_offs(c_filename, e_filename)
	check_dwords(c_filename, e_filename)
	check_identifiers(c_filename, e_filename)
	check_typedefs(c_filename, e_filename)
	check_standard_headers(c_filename)
	check_headers(c_filename, e_filename)

main()
	
	
