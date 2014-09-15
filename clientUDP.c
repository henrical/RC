# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>
# define PORT 59000

int main(){


int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;
char msg[] = "Olá...Zezere, daqui Lima!";
char* buffer = (char*) malloc (sizeof(char) * 64);

fd = socket (AF_INET, SOCK_DGRAM, 0);

hostptr = gethostbyname ("zezere.ist.utl.pt");

memset ((void*)&serveraddr,(int)'\0', sizeof(serveraddr));

serveraddr.sin_family = AF_INET;

serveraddr.sin_addr.s_addr = ((struct in_addr *) (hostptr->h_addr_list[0]))->s_addr;

serveraddr.sin_port = htons ((u_short)PORT);

addrlen = sizeof (serveraddr);

sendto (fd, msg, strlen(msg) + 1, 0, (struct sockaddr *) &serveraddr, addrlen);

addrlen = sizeof (serveraddr);

recvfrom (fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serveraddr, &addrlen);

close(fd); 

printf("%s\n", buffer);

return 0;
}

