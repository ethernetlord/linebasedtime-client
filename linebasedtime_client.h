// LineBasedTime Client - C Header File
// Open source software licensed under MIT License
// Author: EthernetLord
// - website: https://ethernetlord.eu
// - GitHub: https://github.com/ethernetlord/
// - Twitter: https://twitter.com/ethernetlordcz

/*
MIT License

Copyright (c) 2020 EthernetLord (https://ethernetlord.eu/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __LINEBASEDTIME_CLIENT_H__
#define __LINEBASEDTIME_CLIENT_H__

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdint.h>
#include<inttypes.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<locale.h>
#include<unistd.h>
#include<errno.h>
#include<getopt.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<limits.h>



#define SETLOCALE_ON_STARTUP



#define PROGRAM_NAME "LineBasedTime Client"
#define PROGRAM_VERSION "1.0"

#define USAGE_STRING " --- " PROGRAM_NAME " v" PROGRAM_VERSION " --- \n\
\n\
Usage - arguments:\n\
   1. IPv4/v6 address or hostname to connect to\n\
   2. TCP port number or service name to connect to\n\
\n\
Usage - parameters:\n\
   -v, --version\tprint version and quit\n\
   -h, --help\t\tprint usage and quit\n\
   -e, --extra-info\tprint info about server connection\n\
   -u, --unix-only\tprint only unix time\n\
   -c, --compare\tcompare remote time with your machine's\n\
"

typedef struct {
  uint64_t unix_time;
  char utc_time_str[25];
  char local_time_str[25];
} FETCHED_DATA;



struct {
  char *ip_or_hostname;
  char *port_or_service;
  bool extra_info;
  bool unix_only;
  bool compare;
} g_options;



void print_usage(void);
void print_version(void);
void crash(const char *, const char *, ...);
void fai_crash(struct addrinfo *, const char *, const char *, ...);
void print_extra_info(const char *, ...);

void initialize_options(int, char **);

void connect_print_extra_info(struct addrinfo *, struct addrinfo *);
int connect_to_server();
FETCHED_DATA fetch_data_from_server(int);
void disconnect_from_server(int);

void display_fetched_data(FETCHED_DATA *);
void display_fetched_unix_time(FETCHED_DATA *);



#endif
