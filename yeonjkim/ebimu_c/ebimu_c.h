#ifndef EBIMU_C_H
#define EBIMU_C_H

#define BAUD	115200
#define TRUE	1
#define FALSE	0

typedef struct DataType
{
	float roll, pitch, yaw, gyro_x, gyro_y, gyro_z, x_acc, y_acc, z_acc; 
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
#include <wiringPi.h>
#include <stdlib.h>
#include <math.h>

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
