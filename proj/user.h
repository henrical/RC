/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "all.h"

#define BASE_PORT 58000

typedef struct ServerID {char name[MAX_NAME]; int port ;} ServerID;

void usClientUDP(ServerID, ServerID*, int*, char[], char[]);
void usClientTCP(ServerID, char[], char[], int);











