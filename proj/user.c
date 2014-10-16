/*
Grupo: 59
Daniel Vasco 41352
Miguel Carvalho 76017
Henrique Caldeira 75838
*/


#include "user.h"


int main(int argc, char*argv[]) {
  int len=argc, n=1, files=0, loop=1; char buffer[BUFFER_SIZE], buf[BUF_SIZE]; ServerID CSid, SSid;
  gethostname(buffer, BUF_SIZE); strcpy(CSid.name, buffer); CSid.port = BASE_PORT+NG;
  while(len>1){ len-=2;
    if (!strcmp(argv[n],NAME_COM)) strcpy(CSid.name, argv[n+1]);
    else if (!strcmp(argv[n],PORT_COM)) CSid.port=atoi(argv[n+1]); n+=2;
  } while(loop){ reset(buffer); printf(">");fgets(buffer, MAX_COMM, stdin);
  /* -------------------------------------------------- LST -------------------------------------------------- */
    if (!strcmp(buffer,LIST_COM)) usClientUDP(CSid, &SSid, &files, buffer, buf);
    else if (strlen(buffer)>(len=strlen(RETRIEVE_COM))+1&&!strncmp(buffer, RETRIEVE_COM, len)) {
    /* -------------------------------------------------- retrieve -------------------------------------------------- */
      strcpy(buf, DOWNLOAD); strcat(buf, SP); strcat(buf, buffer+strlen(RETRIEVE_COM)+1); strcpy(buffer, buf);
      if (SSid.port) { printf("[<- ssTCP: %s] \t%s", SSid.name, buffer); usClientTCP(SSid, buffer, buf, _RETRIEVE); }
      else printf(ERROR_MSG);
    } else if (!strncmp(buffer,UPLOAD_COM, strlen(UPLOAD_COM))) {
    /* -------------------------------------------------- upload -------------------------------------------------- */
    strcpy(buf,UPLOAD); strcat(buf,SP); strcat(buf,buffer+strlen(UPLOAD_COM)+1); clear(buf); strcat(buf,END);
    printf("[<- csTCP: %s] \t%s", CSid.name, buf); usClientTCP(CSid, buffer, buf, _UPLOAD);
    /* -------------------------------------------------- exit -------------------------------------------------- */
    } else if (!strcmp(buffer,EXIT_COM)) loop=0;
    else printf(ERROR_MSG);
  } return 0;
}


void usClientUDP(ServerID CSid, ServerID*SSid, int*files, char buffer[], char buf[]){
  int fd=-1, addrlen, n, i=0, s=0, f=-3, len; struct hostent *hostptr; struct sockaddr_in serveraddr;
  while (fd==-1) {
    fd = socket (AF_INET, SOCK_DGRAM, 0);
    hostptr = gethostbyname(CSid.name); strcpy(buf,hostptr->h_name);
    memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr));
    serveraddr.sin_family=AF_INET; serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    serveraddr.sin_port=htons((u_short)CSid.port); addrlen=sizeof(serveraddr);
  } /* -------------------------------------------------- LST -------------------------------------------------- */
  strcpy(buffer,LIST_SERVER); buffer[strlen(LIST_SERVER)]='\0';
  sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&serveraddr, addrlen); printf("[<- csUDP: %s] \t%s", buf, buffer);
  recvfrom (fd, buffer, BUF_SIZE, 0, (struct sockaddr*)&serveraddr, &addrlen); close(fd);
  printf("[-> csUDP: %s] \t%s\n\n", buf,clear(buffer)); buffer[(len=strlen(buffer))]='\n';  
  for (n=0; n<=len; n++) {
    if (buffer[n]==' '||buffer[n]=='\n') {
    if (buffer[n]=='\n') s=1; buffer[n]='\0';
      if (f==-3&&strcmp(buffer,LIST_ANSWERS)) { printf(ERROR_MSG); s=1; }
      if (f==-2) strcpy(SSid->name,buffer+i);
      if (f==-1) SSid->port=atoi(buffer+i);
      if (!f) *files=atoi(buffer+i);
      if (f>0)  printf("%d - %s\n",f,buffer+i);
      i=n+1; f++; if (s) break;
  }} printf("\n");
}


void usClientTCP(ServerID SSid, char buffer[], char buf[], int action){
  int fd=-1, len, ok=0; unsigned long fileSize; char name[MAX_NAME], comm[MAX_COMM], *fileData;
  struct hostent *hostptr;struct sockaddr_in serveraddr;
  while (fd==-1) {
  if ((fd=socket (AF_INET, SOCK_STREAM, 0))) {
    hostptr = gethostbyname (SSid.name);
    memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr)); serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
    serveraddr.sin_port=htons((u_short)SSid.port);
    if (connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))) { close(fd); fd=-1; } 
  }} switch (action) {
    case  _RETRIEVE: /* -------------------------------------------------- _RETRIEVE -------------------------------------------------- */
      strcpy(name, buffer+strlen(DOWNLOAD)+1);clean(name);
      strcpy(buf,REQ_ANSWERS); strcat(buf,SP); strcat(buf,STATUS);
      if (Write(fd, buffer, strlen(buffer)) && Read(fd, buffer, (len=strlen(buf)))) {
        strcpy(comm,buffer); comm[len]='\0'; strcat(comm, SP);
          if (!strncmp(buffer, buf, len)) {
            reset(buf); read(fd, buffer, 1); buffer[0]='\0';
	    while (buffer[0]!=' ') { Read(fd, buffer, 1); buffer[1]='\0'; strcat(buf, buffer); }
	    fileSize=atoul(buf); strcat(comm, buf); fileData=(char*)malloc(fileSize+1); ok=Read(fd, fileData, fileSize);
	    if (ok) writeFile(name, fileData, fileSize);
	    free(fileData); read(fd, buffer, 1); /*if (buffer[0]==END[0]) ok=1;*/
          } else { read(fd, buffer, BUF_SIZE); strcat(comm, buffer); }
          printf("[-> ssTCP: %s] \t%s\n", SSid.name, comm);
          if (ok) printf(DL_MSG);
	  else printf(DL_ERROR);
       } else printf(TIMEOUT_MSG);
       break;
    case  _UPLOAD: /* -------------------------------------------------- _UPLOAD -------------------------------------------------- */
      len=strlen(UP_ANSWER)+strlen(UP_NEW)+1; strcpy(name, buf+strlen(UPLOAD)+1); clean(name);
      if (Write(fd, buf, strlen(buf)) && Read(fd, buffer, len)) { buffer[len-1]='\0';
        if (!strcmp(buffer+strlen(UP_ANSWER), UP_DUP)) {
	printf("[-> csTCP: %s] \t%s\n", SSid.name, buffer); }
	 else if (!strcmp(buffer+strlen(UP_ANSWER), UP_NEW)) {
	  printf("[-> csTCP: %s] \t%s\n", SSid.name, buffer);
	  strcpy(buffer,UP_SEND); strcat(buffer,SP); fileData=readFile(name, &fileSize);
	  if (fileData==NULL) printf(ERROR_MSG);
	  else {
	    sprintf(buf,"%lu",fileSize); strcat(buffer,buf); strcat(buffer,SP);
	    Write(fd, buffer, strlen(buffer)); printf("[<- csTCP: %s] \t%s\n", SSid.name, buffer);
	    Write(fd, fileData, fileSize); free(fileData); Write(fd, END, 1);
	    strcpy(buf,UP_ANSWER2); strcat(buf,SP); strcat(buf,STATUS); strcat(buf,END); Read(fd, buffer, strlen(buf));
	    printf("[-> csTCP: %s] \t%s\n", SSid.name, clear(buffer));
	  }} else { printf("[-> csTCP: %s] \t%s\n", SSid.name, buffer);printf(ERROR_MSG); }
      } else printf(TIMEOUT_MSG);
      break; 
  } close(fd);
}
