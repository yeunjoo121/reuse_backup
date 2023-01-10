#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serv_addr;
	socket_t serv_len = sizeof(serv_addr);
	char message[BUF_SIZE];
	int str_len;
	
	bzero(&serv_addr, serv_len);
	
	if (argc != 2)
	{
		printf("Usage : %s <port> \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0); //create udp socket
	if (serv_sock == -1)
		error_handling("socket() error");
		
	/*initial server address info*/
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(4444);
	
	/*assign server address info*/
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	while (true)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		/*receive string from client except \n*/
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		/*send received data to client*/
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
	}
	close(serv_sock); //end udp socket
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
