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
#define MAX_TRY 100
#define PACK 512
#define MIN(a,b) (a<b?a:b)


char* readFile(char*name, unsigned long*fileLen) {
  int received=0; FILE *file; char *buffer;
  file = fopen(name, "rb");
  if (file==NULL) return NULL;
  fseek(file, 0, SEEK_END);
  *fileLen=ftell(file);  
  fseek(file, 0, SEEK_SET);
  buffer=(char*)malloc(*fileLen+1);
  if (!buffer) return NULL;
  while(!received) received=fread(buffer, *fileLen, 1, file);
  fclose(file); return buffer;
}


void writeFile(char*name, char*buffer, unsigned long fileLen) {
  int sent=0;FILE *file;
  file = fopen(name, "wb");
  while(!sent) sent=fwrite(buffer, fileLen, 1, file);
  fclose(file);
}


int Write(int fd, char*buffer, int len) {
   int c=0, d=0, done=0;
   for(;done<len && c<MAX_TRY;) {
     d=write(fd,buffer+done,MIN(PACK,len-done));
     if (d>0) { done+=d; c=0; }
     else if (!d) c++;
     else break;
   } return !!(done==len);
}


int Read(int fd, char*buffer, int len) {
  int c=0, d=0, done=0;
  for(;done<len && c<MAX_TRY;) {
    d=read(fd,buffer+done,MIN(PACK,len-done));
    if (d>0) { done+=d; c=0; }
      else if (!d) c++;
      else break;
  } return !!(done==len);
}


unsigned long atoul(char*s) {   
  unsigned long  a;   
  while(*s == ' '||*s == '\t') s++;
  a = 0; while(*s >='0'&&*s <='9') a=a*10L+(*s++-'0');
  return a;
}


int clean(char*name) {
  int last=strlen(name)-1, n;
  for (n=last; n>=0&&name[n]<=' '; n--);
  name[n+1]='\0'; return !!name[0];
}


char* clear(char*buffer) {  int n=0;for (; buffer[n]!='\n'; n++); buffer[n]='\0'; return buffer;}


void reset(char*buffer) { buffer[0]='\0'; }


int explode2(char*line, int*s1, int*s2, int*e1, int*e2) {
  int n=0, len=strlen(line), ok=0; *e1=*e2=len;
  if (!len) return ok;
  for (; n<len&&line[n]<=' '&&n<len; n++); *s1=n;
  for (; n<len&&line[n]>' '&&n<len; n++); *e1=n;
  for (; n<len&&line[n]<=' '&&n<len; n++); *s2=n;
  for (; n<len&&line[n]>' '&&n<len; n++); *e2=n;
  if (*s1<len) { line[*e1]='\0';
  if (*s2<len) { line[*e2]='\0'; ok=1; }
  }  return ok;
}
