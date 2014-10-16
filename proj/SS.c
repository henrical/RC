/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "SS.h"


int main(int argc, char*argv[]) {
  int files=0; char buffer[BUFFER_SIZE], buf[BUF_SIZE]; struct hostent *h; struct in_addr *a;
  CompServerID id; SSfiles ssFiles[MAX_FILES]; files=getSSfilesList(ssFiles);
  id.port=BASE_PORT; gethostname(buffer, BUF_SIZE); strcpy(id.name,buffer); 
  h=gethostbyname(id.name); a=(struct in_addr*)h->h_addr_list[0]; strcpy(id.ip,inet_ntoa(*a));
  SSserverTCP(id, buffer, buf, &files, ssFiles);
  return 0;
}

void SSserverTCP(CompServerID id, char buffer[], char buf[], int*files, SSfiles ssFiles[]) {
  int fd=-1, pid, newfd, clientlen, n, len, ok; unsigned long fileSize; char name[MAX_NAME], *fileData=NULL;
  struct sockaddr_in serveraddr, clientaddr; struct in_addr *a; char ipfrom[MAX_NAME];
  for(;;) {
    while(fd==-1) {
      if ((fd=socket(AF_INET, SOCK_STREAM, 0))>-1) {
        memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr)); serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); serveraddr.sin_port = htons ((u_short)id.port);
        if (bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))) { close(fd); fd=-1; }
        else { listen (fd, 5); clientlen = sizeof(clientaddr); newfd = accept(fd, (struct sockaddr*)&clientaddr, &clientlen); }
      }} len=strlen(DOWNLOAD); reset(buffer); a=(struct in_addr*)&clientaddr; strcpy(ipfrom,inet_ntoa(*a));
      if (Read(newfd, buffer, len)>0&&!(pid=fork())) {
        if (!strncmp(buffer, DOWNLOAD, len)) { *files=getSSfilesList(ssFiles);
    /* -------------------------------------------------- REQ Fn -------------------------------------------------- */
          reset(buffer); read(newfd, buffer, BUF_SIZE); printf("[-> userTCP:%s] \t", ipfrom); printf(DOWNLOAD); printf("%s\n",clear(buffer));
          strcpy(name, buffer+1); clean(name); reset(buffer); strcpy(buffer,REQ_ANSWERS); strcat(buffer,SP); ok=0;
          for (n=0;n<*files&&!ok;n++) if (!strcmp(ssFiles[n].name,name)) ok=1;
          strcpy(buf, buffer); strcat(buf,ERROR);
          if (ok) {
  	    if ((fileData=readFile(name, &fileSize))) {
	      strcat(buffer,STATUS); strcat(buffer,SP); sprintf(buf,"%lu",fileSize); strcat(buffer,buf);strcat(buffer,SP);
	      write(newfd, buffer, strlen(buffer)); printf("[<- userTCP: %s] \t%s\n", id.name,clear(buffer));
	      write(newfd, fileData, fileSize); free(fileData); strcpy(buffer,END); write(newfd, buffer, strlen(buffer)); reset(buffer);
	    } else strcpy(buffer, buf);
          } else strcpy(buffer, buf);
          if (buffer[0]) printf("[<-userTCP] \t%s", buffer );
        } else if (!strncmp(buffer, UP_SEND2, len)) {
    /* -------------------------------------------------- UPS Fn size data -------------------------------------------------- */
	  Read(newfd, buffer, 1);reset(buffer); reset(buf);  printf("[-> csTCP: %s] \t", ipfrom); printf(UP_SEND2);
	  while(buffer[0]!=' ') { Read(newfd, buffer, 1); buffer[1]='\0'; strcat(buf,buffer); }
	  clean(buf); strcpy(name,buf); printf(" %s", name); reset(buffer); reset(buf);
	  while(buffer[0]!=' ') { Read(newfd, buffer, 1); buffer[1]='\0'; strcat(buf,buffer); } fileSize=atoul(buf); printf(" %lu\n",fileSize);
	  fileData=(char*)malloc(fileSize+1); Read(newfd, fileData, fileSize); read(newfd, buffer, 1) ; reset(buffer);
	  writeFile(name, fileData, fileSize); free(fileData); strcpy(buffer,UP_ANSWER3); strcat(buffer,SP);
	  strcat(buffer,STATUS); strcat(buffer,END); write(newfd, buffer, strlen(buffer));
	  if (appendSSfilesList(ssFiles, name)) { strcpy(ssFiles[(*files)++].name, name); printf("[<- csTCP: %s] \t%s\n", ipfrom, clear(buffer)); }
	  else { strcpy(buffer,UP_ANSWER3); strcat(buffer,SP); strcat(buffer,NSTATUS);
	    write(newfd, buffer, strlen(buffer));  printf("[<- csTCP: %s] \t%s\n", id.name,clear(buffer)); }
	} else { strcpy(buffer,ERROR); write(newfd, buffer, strlen(buffer));  printf("[<- csTCP: %s] \t%s\n", id.name,clear(buffer)); }
	 if (buffer) write (newfd, buffer, strlen(buffer));
        close(fd); close (newfd); exit(0);
      } else { close(fd); close (newfd); fd=-1;}
 }}

 
int getSSfilesList(SSfiles ssFiles[]) {
   int lines=0; char line[MAX_NAME]; FILE *fr;
   if (!(fr=fopen(SSFILES, "r"))) return 0;
   while(fgets(line, MAX_NAME, fr)) {
     clean(line); strcpy(ssFiles[lines++].name, line);
   } fclose(fr); return lines;
}


int appendSSfilesList(SSfiles ssFiles[], char name[]) {
   FILE *fr;
   if (!(fr=fopen(SSFILES, "a"))) return 0;
   fprintf(fr, "\r\n%s", name);
   fclose(fr); return 1;
}
