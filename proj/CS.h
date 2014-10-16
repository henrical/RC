/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "all.h"

#define BASE_PORT 58000

typedef struct SSservers { char name[MAX_NAME]; int port;} SSservers;
typedef struct SSfiles { char name[MAX_NAME]; } SSfiles;

void csServerUDP(int, char[], char[], int*, int*, SSfiles[], SSservers[]);
void csServerTCP(int, char[], char[], int*, SSfiles[], int, SSservers[]);
int csClientTCP(SSservers, char[], char[], char*, unsigned long, char[], int*, SSfiles[]);
int getSSserversList(SSservers[]);
int getSSfilesList(SSfiles[]);
int appendSSfilesList(SSfiles[], char[]);
int selectSS(int);
