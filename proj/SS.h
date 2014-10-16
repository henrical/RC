/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "all.h"

#define BASE_PORT 59000

typedef struct CompServerID { char ip[MAX_NAME]; char name[MAX_NAME]; int port ; } CompServerID;
typedef struct SSfiles { char name[MAX_NAME]; } SSfiles;

void SSserverTCP(CompServerID, char[], char[], int*, SSfiles[]);
int getSSfilesList(SSfiles[]);
int appendSSfilesList(SSfiles[], char[]);
