#include <stdint.h>
#include <termios.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/gmon.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <grp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <ctype.h>
#include <string.h>

struct in_addr {
  unsigned int       s_addr;     /* address in network byte order */
};

struct hostent {
  char *h_name;       /* official name of host */
  char **h_aliases;   /* alias list */
  int h_addrtype;     /* host address type */
  int h_length;       /* length of address */
  char **h_addr_list; /* list of addresses */
};

struct utimbuf {
  time_t actime;       /* access time */
  time_t modtime;      /* modification time */
};

int __compar_fn_t;
int __timezone_ptr_t;
int __gmon_start__;

int _gmon_start__(){
  return __gmon_start__;

}

struct spwd *getspent(){
  char *x;
  return x;
}

int LODWORD(){ return 0;}
int  HIWORD(){ return 0;}
long int  HIDWORD(){ return 0;}
signed long int  SHIDWORD(){ return 0;}


int __rev(long int x){
  return 0;
}

int __OFADD__(int x, int y){
  return 0;
}



int __CFADD__(int x, int y){
  return 0;
}

int __PAIR__(int x, int y){
  return 0;
}

int __OFSUB__(int x, int y){
  return 0;
}

int __RCR__(int x, int y){
  return 0;
}


unsigned int __clz(unsigned int x){
  return x;
}

int BYTE1(int x){
  return 0;
}

int _errno_location(){
  return 0;
}

const uint16_t _ctype_b_loc(){
  return 0;
}


int JUMPOUT(int x){
  return 0;
}



