/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "CS.h"


int main(int argc, char*argv[]){
  int CSport=BASE_PORT+NG, servers=0, files=0, pid; char buffer[BUFFER_SIZE], buf[BUF_SIZE];
  SSservers sSservers[MAX_SERVERS]; SSfiles ssFiles[MAX_FILES];
  if (argc>2 && !strcmp(argv[1],PORT_COM)) CSport=atoi(argv[2]);
  files=getSSfilesList(ssFiles);
  if ((servers=getSSserversList(sSservers))) { 
    if (!(pid=fork())) csServerUDP(CSport, buffer, buf, &files, &servers, ssFiles, sSservers);
    else  if (pid>0) csServerTCP(CSport, buffer, buf, &files, ssFiles, servers, sSservers);
    else exit(0);
  } else printf(NOSERV_MSG);
  return 0;
}


void csServerUDP(int CSport, char buffer[], char buf[], int*files, int*servers, SSfiles ssFiles[], SSservers sSservers[]) {
  int fd=-1, t, addrlen, n, len; char buf2[MAX_NAME]; struct sockaddr_in serveraddr, clientaddr;
  while(fd==-1) {
    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))>-1) {
      memset((void*)&serveraddr,(int)'\0', sizeof(serveraddr)); serveraddr.sin_family=AF_INET;
      serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); serveraddr.sin_port = htons((u_short)CSport);
      if (bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))) { close(fd); fd=-1; }
      else addrlen = sizeof(clientaddr);
  } for(t=0;t<CON_TRY;t++) {
    reset(buffer); len=recvfrom(fd, buffer, BUF_SIZE, 0, (struct sockaddr*)&clientaddr,&addrlen); buffer[len]='\0';
    if (len>0) { t=CON_STOP_TRY;
      strcpy(buf,inet_ntoa(clientaddr.sin_addr)); printf("[-> userUDP: %s] \t%s", buf, buffer);
      if (!strcmp(buffer, LIST_SERVER)) { *files=getSSfilesList(ssFiles);
       /* -------------------------------------------------- LST -------------------------------------------------- */
        if (*files) { strcpy(buffer, LIST_ANSWERS); strcat(buffer,SP); n=selectSS(*servers);
	  strcat(buffer,sSservers[n].name); strcat(buffer,SP); sprintf(buf2,"%d",sSservers[n].port);
	  strcat(buffer,buf2); strcat(buffer,SP); sprintf(buf2,"%d",*files); strcat(buffer,buf2); 
          for (n=0;n<*files;n++) { strcat(buffer,SP); strcat(buffer,ssFiles[n].name); } strcat(buffer,END);
        } else strcpy(buffer,NOFILES);	
      } else strcpy(buffer,ERROR);
      sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientaddr,addrlen);
      printf("[<- userUDP: %s] \t%s\n",buf,buffer); close(fd);  fd=-1; 
    }} close(fd);  fd=-1;
}}


void csServerTCP(int CSport, char buffer[], char buf[], int*files, SSfiles ssFiles[], int servers, SSservers sSservers[]) {
  int fd=-1, pid, newfd, clientlen, n, len, exits; unsigned long fileSize;
  char name[MAX_NAME], comm[MAX_NAME], *fileData=NULL;
  struct sockaddr_in serveraddr, clientaddr; struct in_addr *a; char ipfrom[MAX_NAME];
  for(;;) {
    while(fd==-1) {
      if ((fd=socket(AF_INET, SOCK_STREAM, 0))>-1) {
        memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons ((u_short)CSport);
        if (bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))) { close(fd); fd=-1; }
        else {
	  listen (fd, 5); clientlen = sizeof(clientaddr);
	  newfd = accept(fd, (struct sockaddr*)&clientaddr, &clientlen);
	  if ((pid=fork())) { close(fd); close (newfd); fd=-1; }
	}
    }} reset(buffer); a=(struct in_addr*)&clientaddr; strcpy(ipfrom,inet_ntoa(*a));
    if (read(newfd, buffer, BUF_SIZE)>0) { clean(buffer); *files=getSSfilesList(ssFiles);
      if (strlen(buffer)>(len=strlen(UPLOAD))+1 && !strncmp(buffer,UPLOAD,len)) {
    /* -------------------------------------------------- UPC Fn size data -------------------------------------------------- */
        printf("[-> userTCP: %s] \t%s\n", ipfrom, clear(buffer));
        strcpy(name, buffer+len+1); clean(name); reset(buffer);
	strcpy(buffer,REQ_ANSWERS); strcat(buffer,SP); exits=0;
        for (n=0;n<*files&&!exits;n++) if (!strcmp(ssFiles[n].name,name)) exits=1;
        if (exits) {
	  strcpy(buffer,UP_ANSWER); strcat(buffer,UP_DUP); strcat(buffer,END);
	  Write(newfd, buffer, strlen(buffer)); printf("[<- userTCP: %s] \t%s", ipfrom, buffer);
	} else {
	  strcpy(buffer,UP_ANSWER); strcat(buffer,UP_NEW); strcat(buffer,END);
	  printf("[<- userTCP: %s] \t%s", ipfrom, buffer);
	  strcpy(buf,UP_SEND); strcat(buf,SP);
          if (Write(newfd, buffer, strlen(buffer)) && Read(newfd, buffer, (len=strlen(buf)))) {   
            buffer[len]=0;strcpy(comm,buffer);
             if (!strcmp(buffer, buf)) {
               reset(buf);buffer[0]='\0';
	       while (buffer[0]!=' ') { read(newfd, buffer, 1); buffer[1]='\0'; strcat(buf, buffer); } fileSize=atoul(buf);
               fileData=(char*)malloc(fileSize+1); Read(newfd, fileData, fileSize); read(newfd, buffer, 1);
	       strcpy(buf,UP_ANSWER2); strcat(buf,SP); strcat(buf,STATUS); strcat(buf,END); Write(newfd, buf, strlen(buf));
	       printf("[<- userTCP: %s] \t%s", ipfrom, buf);
	       for (n=0; n<servers; n++) csClientTCP(sSservers[n], buffer, buf, fileData, fileSize, name, files, ssFiles);
	       free(fileData); appendSSfilesList(ssFiles, name); strcpy(ssFiles[(*files)++].name, name);
             } else printf("[-> ssTPCs: %s] \t%s\n", ipfrom, comm);
          } else printf(TIMEOUT_MSG);
      }} else { strcpy(buffer,ERROR); write(newfd, buffer, strlen(buffer)); printf("[<- userTCP: %s] \t%s\n", ipfrom, buffer); }
      close(fd); close (newfd); fd=-1; exit(0);
  }}}

  
int csClientTCP(SSservers SSid, char buffer[], char buf[], char*fileData, unsigned long fileSize, char name[], int*files, SSfiles ssFiles[]) {
  int fd=-1, len, ok=0; struct hostent *hostptr;struct sockaddr_in serveraddr; char buf2[BUF_SIZE];
  while (fd==-1) {
  if ((fd=socket (AF_INET, SOCK_STREAM, 0))) {
    hostptr = gethostbyname(SSid.name);
    memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    serveraddr.sin_port = htons((u_short)SSid.port);
    if (connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))) { close(fd); fd=-1; } 
  }} /* -------------------------------------------------- UPC size data -------------------------------------------------- */
  len=strlen(UP_ANSWER3)+strlen(STATUS)+2;
  strcpy(buf,UP_SEND2); strcat(buf,SP); strcat(buf,name); strcat(buf,SP);
  sprintf(buf2,"%lu",fileSize); strcat(buf,buf2); strcat(buf,SP);
  printf("[<- ssTCP:   %s port:%d] \t%s\n", SSid.name, SSid.port, buf);
  if (Write(fd, buf, strlen(buf)) && Write(fd, fileData, fileSize) && Write(fd, END, strlen(END)) && Read(fd, buffer, len)) {
    buffer[len-1]='\0'; strcpy(buf,UP_ANSWER3); strcat(buf,SP); strcat(buf,STATUS);
    if (!strcmp(buffer,buf)) { printf("[-> ssTCP:   %s] \t\t%s\n", SSid.name, buffer); ok=1; }
    else printf("[-> ssTCP:   %s] %s\n", SSid.name, buffer); 
  } else printf(TIMEOUT_MSG);
  close(fd); return ok;
}


int getSSserversList(SSservers sSservers[]) {
   int name=-1, port=-1, e1, e2, lines=0; char line[MAX_LINE]; FILE *fr;
   if (!(fr=fopen(CSSERVERS, "r"))) return 0;
   while(fgets(line, MAX_LINE, fr))
     if (explode2(line, &name, &port, &e1, &e2))
      { strcpy(sSservers[lines].name, line+name); sSservers[lines++].port=atoi(line+port); }
   fclose(fr); return lines;
}


int getSSfilesList(SSfiles ssFiles[]) {
   int lines=0; char line[MAX_NAME]; FILE *fr;
   if (!(fr=fopen(SSFILES, "r"))) return 0;
   while(fgets(line, MAX_NAME, fr)) {
    if (clean(line)) strcpy(ssFiles[lines++].name, line);
   } fclose(fr); return lines;
}


int appendSSfilesList(SSfiles ssFiles[], char name[]) {
   FILE *fr;
   if (!(fr=fopen(SSFILES, "a"))) return 0;
   fprintf(fr, "\r\n%s", name);
   fclose(fr); return 1;
}


int selectSS(int servers) { srand(time(NULL)); return rand()%servers; }
