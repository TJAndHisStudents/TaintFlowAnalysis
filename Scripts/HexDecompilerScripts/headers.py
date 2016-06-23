#!/usr/bin/python
# Read compiler errors and determine headers to prepend to C file.

import sys


# List of standard headers missing
STDLIB_HEADER = "#include <stdlib.h>"
UNISTD_HEADER = "#include <unistd.h>"

# List of headers seen in Engagement 2
NET_DB_HEADER = "#include <netdb.h>"
TIME_HEADER = "#include <time.h>"
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

# List of identifiers that need to be defined
REGISTER1 = "int _R1;"
REGISTER2 = "int _R2;"
REGISTER3 = "int _R3;"
ZERO_FLAG = "int _ZF;"
EVP_VAR = "void* EVP_rc4;"
CS_VAR = "void* __CS__;"




def check_header(filename, header):
	print header + " checked"
	c_fd = open(filename)
	
	for line in c_fd:
		if header in line:
			return 1

	return 0


def add_header(filename, header):
	with open(filename, 'r+') as f:
		content = f.read()
		f.seek(0, 0)
		f.write(header + '\n' + content)


def check_standard_headers(filename):
	
	if (check_header(filename, STDLIB_HEADER) == 0):
		add_header(filename, STDLIB_HEADER)

	if (check_header(filename, UNISTD_HEADER) == 0):
		add_header(filename, UNISTD_HEADER)	

def check_need_header(e_filename, header):
	
	error_fd = open(e_filename)

	for line in error_fd:
		if header in line:
			return 1

	return 0

def check_various_headers(c_filename, e_filename):
	
	# Check if we need sys/uio.h using iovec struct
	
	if (check_need_header(e_filename, "struct iovec") == 1):
		if (check_header(c_filename, UIO_HEADER) == 0):
			add_header(c_filename, UIO_HEADER)

	# Check if we need pwd.h using passwd struct

	if (check_need_header(e_filename, "struct passwd") == 1):
		if (check_header(c_filename, PWD_HEADER) == 0):
			add_header(c_filename, PWD_HEADER)

	# Check if we need shadow.h using shadow struct

	if (check_need_header(e_filename, "struct spwd") == 1):
		if (check_header(c_filename, SHADOW_HEADER) == 0):
			add_header(c_filename, SHADOW_HEADER)

	# Check if we need termios.h using termios struct

	if (check_need_header(e_filename, "struct termios") == 1):
		if (check_header(c_filename, TERMIOS_HEADER) == 0):
			add_header(c_filename, TERMIOS_HEADER)

	# Check if we need grp.h using group struct

	if (check_need_header(e_filename, "struct group") == 1):
		if (check_header(c_filename, GRP_HEADER) == 0):
			add_header(c_filename, GRP_HEADER)
	
	# Check if we need netdb.h using hostent struct
	
	if (check_need_header(e_filename, "struct hostent") == 1):
		if (check_header(c_filename, NET_DB_HEADER) == 0):
			add_header(c_filename, NET_DB_HEADER)

	# Check if we need netdb.h using addrinfo struct
	
	if (check_need_header(e_filename, "struct addrinfo") == 1):
		if (check_header(c_filename, NET_DB_HEADER) == 0):
			add_header(c_filename, NET_DB_HEADER)


	# Check if we need sys/epoll.h using epoll_event struct
	
	if (check_need_header(e_filename, "struct epoll_event") == 1):
		if (check_header(c_filename, SYS_EPOLL_HEADER) == 0):
			add_header(c_filename, SYS_EPOLL_HEADER)

	# Check if we need time.h using tm struct
	
	if (check_need_header(e_filename, "struct tm") == 1):
		if (check_header(c_filename, TIME_HEADER) == 0):
			add_header(c_filename, TIME_HEADER)

	# Check if we need sys/socket.h using sockaddr struct

	if (check_need_header(e_filename, "struct sockaddr") == 1):
		if (check_header(c_filename, SYS_SOCKET_HEADER) == 0):
			add_header(c_filename, SYS_SOCKET_HEADER)

	# Check if we need epoll.h using pollfd struct

	if (check_need_header(e_filename, "struct pollfd") == 1):
		if (check_header(c_filename, POLL_HEADER) == 0):
			add_header(c_filename, POLL_HEADER)

	# Check if we need zlib.h using z_streamp identifier

	if (check_need_header(e_filename, "z_streamp") == 1):
		if (check_header(c_filename, ZLIB_HEADER) == 0):
			add_header(c_filename, ZLIB_HEADER)

def check_identifiers(c_filename, e_filename):

	# Check if we are missing zero flag identifier

	if (check_need_header(e_filename, "_ZF") == 1):
		if (check_header(c_filename, ZERO_FLAG) == 0):
			add_header(c_filename, ZERO_FLAG)

	# Check if we are missing register identifiers
	
	if (check_need_header(e_filename, "_R1") == 1):
		if (check_header(c_filename, REGISTER1) == 0):
			add_header(c_filename, REGISTER1)

	if (check_need_header(e_filename, "_R2") == 1):
		if (check_header(c_filename, REGISTER2) == 0):
			add_header(c_filename, REGISTER2)
	
	if (check_need_header(e_filename, "_R3") == 1):
		if (check_header(c_filename, REGISTER3) == 0):
			add_header(c_filename, REGISTER3)

	# Check if we are missing identifier for EVP_rc4

	if (check_need_header(e_filename, "EVP_rc4") == 1):
		if (check_header(c_filename, EVP_VAR) == 0):
			add_header(c_filename, EVP_VAR)
	
	# Check if we are missing identifier for __CS__

	if (check_need_header(e_filename, "__CS__") == 1):
		if (check_header(c_filename, CS_VAR) == 0):
			add_header(c_filename, CS_VAR)


def main():

	if len(sys.argv) < 2:
		print "Usage: " + sys.argv[0] + " <C-file> <Compiler-Output."

	c_filename = sys.argv[1]
	e_filename = sys.argv[2]

	check_identifiers(c_filename, e_filename)
	check_standard_headers(c_filename)
	check_various_headers(c_filename, e_filename)

main()

