/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_SERVERS   100
#define BUFFER_SIZE     1024
#define BUF_SIZE          1024
#define MAX_LINE          1024
#define MAX_NAME        20
#define MAX_COMM       (2*MAX_NAME)
#define MAX_FILES        30
#define CON_TRY           10
#define CON_STOP_TRY  CON_TRY
#define NG 59

#define PORT_COM        "-p"
#define NAME_COM       "-n"
#define LIST_COM         "list\n"
#define RETRIEVE_COM "retrieve"
#define _RETRIEVE 1
#define UPLOAD_COM   "upload"
#define _UPLOAD 2
#define EXIT_COM         "exit\n"

#define SP                      " "
#define END                    "\n"
#define NOFILES             "EOF\n"
#define STATUS              "ok"
#define NSTATUS            "nok"
#define ERROR               "ERR\n"
#define LIST_SERVER     "LST\n"
#define LIST_ANSWERS  "AWL"
#define REQ_ANSWERS  "REP"
#define DOWNLOAD        "REQ"
#define UPLOAD              "UPR"
#define UP_ANSWER       "AWR"
#define UP_ANSWER2     "AWC"
#define UP_ANSWER3     "AWS"
#define UP_SEND            "UPC"
#define UP_SEND2          "UPS"
#define UP_DUP              " dup"
#define UP_NEW             " new"

#define ERROR_MSG      "\nerror\n"
#define DL_MSG            "\ndownload succeeded\n"
#define DL_ERROR         "\ndownload failure\n"
#define TIMEOUT_MSG  "\nno answer\n"
#define NOSERV_MSG   "\nno servers\n"

#define CSSERVERS        "ssServers.txt"
#define SSFILES             "ssFiles.txt"

int Write(int, char*, int);
int Read(int, char*, int);
char* readFile(char*, unsigned long*);
void writeFile(char*, char*, unsigned long);
unsigned long atoul(char*);
int clean(char*);
char* clear(char*);
void reset(char*);
int explode2(char*, int*, int*, int*, int*);
char* ipFrom(struct sockaddr_in);
