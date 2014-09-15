#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{

	struct hostent *h;
	struct in_addr *a;
	
	if((h=gethostbyname("saasbook"))==NULL) exit(1); //Error

	printf("official host name: %s\n", h->h_name);

	a= (struct in_addr*)h->h_addr_list[0];

	printf("internet adress: %s (%08lX)\n", inet_ntoa(*a), (long unsigned int)ntohl(a->s_addr));
	
	exit(0);
}
