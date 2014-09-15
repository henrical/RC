# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <string.h>
# define PORT 58000

int main(){


int fd, newfd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int clientlen;
char msg[] = "Zezere, daqui Lima, WTF!";
char* buffer = (char*) malloc (sizeof(char) * 1024);

fd = socket (AF_INET, SOCK_STREAM, 0);



memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr));

serveraddr.sin_family = AF_INET;
serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
serveraddr.sin_port = htons ((u_short)PORT);

bind (fd,(struct sockaddr*)&serveraddr, sizeof(serveraddr));

listen (fd, 5);

clientlen = sizeof(clientaddr);

newfd = accept (fd, (struct sockaddr*)&clientaddr, &clientlen);

read (newfd, buffer, 1024);
printf("%s\n", buffer);

write (newfd, "Olá, daqui Lima!\0", 20);
close(fd); close (newfd);





return 0;
}
