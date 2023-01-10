#ifndef HILS_PI_H
#define HILS_PI_H

#define BAUD	115200
#define TRUE	1
#define FALSE	0
#define BUF_SIZE 1024
#define fcc_addr 192.168.0.46
#define gcs_addr 192.168.0.9
#define fcc_port 4434
#define gcs_port 4434

typedef struct DataType
{
	float roll, pitch, yaw, p, q, r, roll_des, pitch_des, yaw_des; 
}	Data;

typedef struct DequeNodeType
{
	Data data;
	struct DequeNodeType* pRLink;
	struct DequeNodeType* pLLink;
}	DequeNode;

typedef struct LinkedDequeType
{
	int currentElementCount;
	DequeNode* pFrontNode;
	DequeNode* pRearNode;
}	LinkedDeque;

#include <stdio.h>
#include <string.h>
#include <wiringSerial.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

char			**ft_split(const char *s, char c, int *len);
void			print_data(Data data);
LinkedDeque*	createLinkedDeque();
LinkedDeque*	createLinkedDeque();
int				insertFrontLD(LinkedDeque* pDeque, DequeNode element);
int				insertRearLD(LinkedDeque* pDeque, DequeNode element);
DequeNode*		deleteFrontLD(LinkedDeque* pDeque);
DequeNode*		deleteRearLD(LinkedDeque* pDeque);
DequeNode*		peekFrontLD(LinkedDeque* pDeque);
DequeNode*		peekRearLD(LinkedDeque* pDeque);
void			deleteLinkedDeque(LinkedDeque* pDeque);
int				isLinkedDequeFull(LinkedDeque* pDeque);
int				isLinkedDequeEmpty(LinkedDeque* pDeque);
void			displayLinkedDeque(LinkedDeque* pDeque);

#endif
