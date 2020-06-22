// LineBasedTime Client - Main C File
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

#include"linebasedtime_client.h"

int main(int argc, char **argv) {
  #ifdef SETLOCALE_ON_STARTUP
  if(setlocale(LC_ALL, "") == NULL)
    crash(NULL, "Locale couldn't be set!");
  #endif


  initialize_options(argc, argv);


  int sockfd = connect_to_server();
  FETCHED_DATA data = fetch_data_from_server(sockfd);

  if(g_options.unix_only) {
    display_fetched_unix_time(&data);
  } else {
    display_fetched_data(&data);
  }

  disconnect_from_server(sockfd);


  return EXIT_SUCCESS;
}



void print_usage(void) {
  fputs(USAGE_STRING, stdout);
  exit(EXIT_SUCCESS);
}

void print_version(void) {
  fputs(PROGRAM_NAME " v" PROGRAM_VERSION "\n", stdout);
  exit(EXIT_SUCCESS);
}

void crash(const char *perror_msg, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  fputs("[ERROR] ", stderr);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);

  if(perror_msg != NULL)
    perror(perror_msg);

  exit(EXIT_FAILURE);
}

void fai_crash(struct addrinfo *gai_result_head, const char *perror_msg, const char *format, ...) {
  freeaddrinfo(gai_result_head);

  // copied code from above to prevent making an extra function
  va_list ap;
  va_start(ap, format);
  fputs("[ERROR] ", stderr);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);

  if(perror_msg != NULL)
    perror(perror_msg);

  exit(EXIT_FAILURE);
}

void print_extra_info(const char *format, ...) {
  if(g_options.extra_info) {
    va_list ap;
    va_start(ap, format);
    fputs("[INFO] ", stderr);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
    va_end(ap);
  }
}



// initializes the global struct g_options using supplied command-line arguments
// (or calls print_usage() or print_version() and quits, if required by user)
void initialize_options(int argc, char **argv) {
  // no need to do this as all global variables must be initialized to 0 by default
  //memset(g_options, 0, sizeof(g_options));

  const static char *opts = "vheuc";
  const static struct option longopts[] = {
    {"version", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {"extra-info", no_argument, NULL, 'e'},
    {"unix-only", no_argument, NULL, 'u'},
    {"compare", no_argument, NULL, 'c'},
    {NULL, no_argument, NULL, '\0'}
  };

  int opt;
  while((opt = getopt_long(argc, argv, opts, longopts, NULL)) >= 0) {
    switch(opt) {
      case 'h': print_usage();
      case 'v': print_version();
      case 'e': g_options.extra_info = true; break;
      case 'u': g_options.unix_only = true; break;
      case 'c': g_options.compare = true; break;
      default: crash(false, "Invalid command-line option entered!");
    }
  }

  if((argc - optind) != 2) {
    fputs(USAGE_STRING "\n", stderr);
    crash(false, "Invalid number of parameters entered!");
  }

  g_options.ip_or_hostname = argv[optind];
  g_options.port_or_service = argv[optind + 1];
}



void connect_print_extra_info(struct addrinfo *gai_result_head, struct addrinfo *gai_result) {
  // doublecheck to save (a little) performance
  if(!g_options.extra_info)
    return;

  char ipaddr_str_buf[INET6_ADDRSTRLEN];
  switch(gai_result->ai_family) {
    case AF_INET:
      if(inet_ntop(AF_INET, &(((struct sockaddr_in *) gai_result->ai_addr)->sin_addr), ipaddr_str_buf, INET6_ADDRSTRLEN) == NULL)
        fai_crash(gai_result_head, "inet_ntop", "Failed to convert IPv4 from binary to text!");
      print_extra_info("Trying to connect to IPv4 host %s:%"PRIu16"...", ipaddr_str_buf, ntohs(((struct sockaddr_in *) gai_result->ai_addr)->sin_port));
      break;

    case AF_INET6:
      if(inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) gai_result->ai_addr)->sin6_addr), ipaddr_str_buf, INET6_ADDRSTRLEN) == NULL)
        fai_crash(gai_result_head, "inet_ntop", "Failed to convert IPv6 from binary to text!");
      print_extra_info("Trying to connect to IPv6 host [%s]:%"PRIu16"...", ipaddr_str_buf, ntohs(((struct sockaddr_in6 *) gai_result->ai_addr)->sin6_port));
      break;

    default:
      fai_crash(gai_result_head, NULL, "Invalid address family detected while trying to display extra info!");
  }
}

int connect_to_server(void) {
  int gai_errcode;
  struct addrinfo gai_hints, *gai_result_head, *gai_result;
  memset(&gai_hints, 0, sizeof(gai_hints));
  gai_hints.ai_family = AF_UNSPEC;
  gai_hints.ai_socktype = SOCK_STREAM;

  print_extra_info("Trying to resolve IP/hostname \"%s\" and port/service \"%s\"...", g_options.ip_or_hostname, g_options.port_or_service);



  if((gai_errcode = getaddrinfo(g_options.ip_or_hostname, g_options.port_or_service, &gai_hints, &gai_result_head)) != 0)
    crash(NULL, "Error occurred while resolving host!\ngetaddrinfo: %s", gai_strerror(gai_errcode));

  int sockfd, last_connstatus = INT_MIN;
  for(gai_result = gai_result_head; gai_result != NULL; gai_result = gai_result->ai_next) {
    if((sockfd = socket(gai_result->ai_family, gai_result->ai_socktype, gai_result->ai_protocol)) < 0)
      fai_crash(gai_result_head, "socket", "Error occurred while initializing socket!");


    connect_print_extra_info(gai_result_head, gai_result);


    if((last_connstatus = connect(sockfd, gai_result->ai_addr, gai_result->ai_addrlen)) < 0) {
      print_extra_info("Error while connecting (%s), trying next if available...", strerror(errno));
      if(close(sockfd) < 0)
        fai_crash(gai_result_head, "close", "Failed to close unused socket!");
    } else {
      print_extra_info("Successfully connected to a server!");
      break;
    }

  }

  freeaddrinfo(gai_result_head);

  switch(last_connstatus) {
    case INT_MIN:
      crash(NULL, "A host resolving error caused that an connection attempt couldn't be performed!");

    case -1:
      print_extra_info("No next server available!");
      crash("connect", "Error occurred while connecting to server!");
  }



  return sockfd;
}

FETCHED_DATA fetch_data_from_server(int sockfd) {
  const uint8_t proto_ver = 1;

  print_extra_info("Receiving data from the server...");

  char recv_buf[103];
  ssize_t recv_size;
  if((recv_size = recv(sockfd, recv_buf, 102, 0)) < 0)
    crash("recv", "Error occurred while receiving data from server!");
  recv_buf[recv_size] = '\0';

  uint8_t server_proto_ver;
  char server_error;
  FETCHED_DATA data;

  switch(sscanf(recv_buf, "%*4s %" SCNu8 " %*5s %c %*5s %" SCNu64 " %*5s %24[^\n] %*5s %24[^\n]", &server_proto_ver, &server_error, &data.unix_time, data.utc_time_str, data.local_time_str)) {
    case 2:
      if(server_error == 'N')
        crash(NULL, "Server sent data in invalid format!");
      crash(NULL, "Server replied, but announced that an error occurred on it's side.");

    case 5:
      break;

    default:
      crash(NULL, "Server sent data in invalid format!");
  }

  if(server_proto_ver != proto_ver)
    crash(NULL, "Server replied, but it's protocol version is different from the client's.");

  if(server_error != 'N')
    crash(NULL, "Server replied, but announced that an error occurred on it's side.");

  return data;
}

void disconnect_from_server(int sockfd) {
  print_extra_info("Disconnecting from the server...");

  if(close(sockfd) < 0)
    crash("close", "Error occurred while closing connection to server.");
}



void display_fetched_data(FETCHED_DATA *data) {
  printf("Server's time:\n   Unix:\t%" PRIu64 "\n   UTC:\t\t%s\n   Local:\t%s\n", data->unix_time, data->utc_time_str, data->local_time_str);

  if(g_options.compare) {
    time_t cl_unix_time;
    struct tm cl_utc_tm, cl_loc_tm;
    char cl_utc_str[26], cl_loc_str[26];

    if(time(&cl_unix_time) < 0 ||  gmtime_r(&cl_unix_time, &cl_utc_tm) == NULL || localtime_r(&cl_unix_time, &cl_loc_tm) == NULL || asctime_r(&cl_utc_tm, cl_utc_str) == NULL || asctime_r(&cl_loc_tm, cl_loc_str) == NULL)
      crash(NULL, "Failed to get your machine's time!");

    uint64_t cl_unix_time_u64 = (uint64_t) cl_unix_time;

    printf("\nYour machine's time:\n   Unix:\t%" PRIu64 "\n   UTC:\t\t%s   Local:\t%s", cl_unix_time_u64, cl_utc_str, cl_loc_str);

    if(cl_unix_time_u64 < data->unix_time) {
      printf("\nYour machine's time is %" PRIu64 " second(s) behind the server's.\n", data->unix_time - cl_unix_time_u64);
    } else if(cl_unix_time_u64 > data->unix_time) {
      printf("\nYour machine's time is %" PRIu64 " second(s) ahead the server's.\n", cl_unix_time_u64 - data->unix_time);
    } else {
      printf("\nYour machine's time is the same as server's!\n");
    }
  }
}

void display_fetched_unix_time(FETCHED_DATA *data) {
  printf("%" PRIu64, data->unix_time);

  if(g_options.compare) {
    time_t cl_unix_time;

    if(time(&cl_unix_time) < 0)
      crash(NULL, "Failed to get your machine's time!");

    printf(" %" PRIu64, (uint64_t) cl_unix_time);
  }

  putchar('\n');
}
