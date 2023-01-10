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
	int sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t adr_sz;
	char *mac_ip = "192.168.0.46";
	int port = 4444;
	
	struct sockaddr_in serv_adr, from_adr;
	
	sock = socket(PF_INET, SOCK_DGRAM, 0);//create udp socket
	if (sock == 1)
		error_handling("socket() error");
		
	/*initialize server address info*/
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(mac_ip);
	serv_adr.sin_port = htons(port);
	
	while (true)
	{
		fputs("Input message(Q to exit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
		/*
		 * in this time, client address assign automatically
		 * send input string to server except \n
		 */
		 sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
		 /*
		  * receive echo string from server and print
		  */
		  adr_sz = sizeof(from_adr);
		  str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);
		  message[str_len] = '\0'; //수신한 문자열 맨 뒤에 널 추가
		  printf("Message from server: %s", message);
	  }
	  close(sock);
	  return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}
