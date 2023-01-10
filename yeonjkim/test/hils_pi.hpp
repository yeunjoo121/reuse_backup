#ifndef HILS_PI_H
#define HILS_PI_H

#define BAUD	115200
#define TRUE	1
#define FALSE	0

#include <cmath>
#include <wiringPi.h>
#include <softPwm.h> 
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <termios.h>
#include <wiringSerial.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

double moduler(double dividend, double divisor);

typedef struct GainType
{
	double p, q, r, phi, theta, psi;
}	Gain;

typedef struct Command
{
	double edf_upper, edf_lower, servo_outer, servo_inner;
}	Command;

class Data {
public:
	double roll, pitch, yaw, p, q, r, roll_des, pitch_des, yaw_des;

	void str_to_float(char **splited_str, int len)
	{
		float transmitted[9] = {0};
		
		if (len == 9)// 9이면 숫자나 -, .가 아닌 값이 들어간 데이터를 nan처리
		{
			for (int j = 0; j < 9; j++)
			{
				int error = 0;
				int decimalP = 0;
				int minus = 0;
				for (int i = 0; i < strlen(splited_str[j]); i++)
				{
					if (!((splited_str[j][i] >= '0' && splited_str[j][i] <= '9') || (splited_str[j][i] == '.')
					|| (splited_str[j][i] == '-')))
						error = 1;
					if (splited_str[j][i] == '.')
					{
						decimalP += 1;
						if (decimalP >= 2)
							error = 1;
					}
					else if (splited_str[j][i] == '-')
					{
						minus += 1;
						if (minus >= 2)
							error = 1;
					}
				}
				if (error == 0)
					transmitted[j] = atof(splited_str[j]);
				else if (error == 1)
					transmitted[j] = sqrt(-1.0f);
			}
		}
		else if (len > 9)
		{
			for (int j = 0; j < 9; j++)
				transmitted[j] = sqrt(-1.0f);
		}
		else if (len < 9)//len이 9보다 작으면 maxlen과 비교
		{
			int index = 0; // j로 값을 넣게 되면 ,의 개수에 따라 인덱스의 값이 달라진다. 따라서 index를 따로 둔다.
			for (int j = 0; j < len; j++)
			{
				int error = 0;
				int decimalP = 0;
				int minus = 0;
				for (int i = 0; i < (int)(strlen(splited_str[j])); i++)
				{
					if (i >= 10)
					{
						error = 2;
						break;
					}
					if (!((splited_str[j][i] >= '0' && splited_str[j][i] <= '9') || (splited_str[j][i] == '.')
					|| (splited_str[j][i] == '-')))
						error = 1;
					if (splited_str[j][i] == '.')
					{
						decimalP += 1;
						if (decimalP >= 2)
							error = 2;
					}
					else if (splited_str[j][i] == '-')
					{
						minus += 1;
						if (minus >= 2)
							error = 2;
					}
				}
				if (error == 0)
					transmitted[index++] = atof(splited_str[j]);
				else if (error == 1)
					transmitted[index++] = sqrt(-1.0f);
				else if (error == 2)
				{
					transmitted[index++] = sqrt(-1.0f);
					transmitted[index++] = sqrt(-1.0f);
				}
			}
		}
		roll = transmitted[0] / 180 * M_PI;
		pitch = transmitted[1] / 180 * M_PI;
		yaw = transmitted[2] / 180 * M_PI;
		p = transmitted[3] / 180 * M_PI;
		q = transmitted[4] / 180 * M_PI;
		r = transmitted[5] / 180 * M_PI;
		//roll_des = transmitted[6];
		//pitch_des = transmitted[7];
		//yaw_des = transmitted[8];
	}

	void print_data()
	{
		cout << "roll : " << roll << "\t\t pitch : " << pitch << "\t\t yaw : " << yaw << endl;
		cout << "p : " << p << "\t\t q : " << q << "\t\t r : " << r << endl;
		cout << "roll_des : " << roll_des << "\t\t pitch_des : " << pitch_des << "\t\t yaw_des : " << yaw_des << endl;
	}
};

class Euler{
public:
	double euler[3][1];
	Euler()
	{}
	Euler(double roll, double pitch, double yaw)
	{ 
		euler[0][0] = roll;
		euler[1][0] = pitch;
		euler[2][0] = yaw;
	}
	void set_euler(double roll, double pitch, double yaw)
	{
		euler[0][0] = roll;
		euler[1][0] = pitch;
		euler[2][0] = yaw;
	}
	void str_to_float(char **splited_str, int len)
	{
		double transmitted[3] = {0};
		if (len == 3)// 3이면 -, .가 아닌 값이 들어간 데이터를 nan처리
		{
			for(int j = 0; j < 3; j++)
			{
				int error = 0;
				int decimalP = 0;
				int minus = 0;
				for (int i = 0; i < strlen(splited_str[j]); i++)
				{
					if (!((splited_str[j][i] >= '0' && splited_str[j][i] <= '9') || (splited_str[j][i] == '.')
					|| (splited_str[j][i] == '-')))
					{
						error = 1;
					}
					if (splited_str[j][i] == '.')
					{
						decimalP += 1;
						if (decimalP >= 2)
							error = 1;
					}
					else if (splited_str[j][i] == '-')
					{
						minus += 1;
						if (minus >= 2)
							error = 1;
					}
				}
				if (error == 0)
					transmitted[j] = atof(splited_str[j]);
				else if (error == 1)
					transmitted[j] = sqrt(-1.0f);
			}
		}
		else if (len > 3)
		{
			for (int j = 0; j < 3; j++)
				transmitted[j] = sqrt(-1.0f);
		}
		else if (len < 3)//len이 3보다 작으면 maxlen과 비교
		{
			int index = 0; // j로 값을 넣게 되면 ,의 개수에 따라 인덱스의 값이 달라진다. 따라서 index를 따로 둔다.
			for (int j = 0; j < len; j++)
			{
				int error = 0;
				int decimalP = 0;
				int minus = 0;
				for (int i = 0; i < (int)(strlen(splited_str[j])); i++)
				{
					if (i >= 10)
					{
						error = 2;
						break;
					}
					if (!((splited_str[j][i] >= '0' && splited_str[j][i] <= '9') || (splited_str[j][i] == '.')
					|| (splited_str[j][i] == '-')))
						error = 1;
					if (splited_str[j][i] == '.')
					{
						decimalP += 1;
						if (decimalP >= 2)
							error = 2;
					}
					else if (splited_str[j][i] == '-')
					{
						minus += 1;
						if (minus >= 2)
							error = 2;
					}
				}
				if (error == 0)
					transmitted[index++] = atof(splited_str[j]);
				else if (error == 1)
					transmitted[index++] = sqrt(-1.0f);
				else if (error == 2)
				{
					transmitted[index++] = sqrt(-1.0f);
					transmitted[index++] = sqrt(-1.0f);
				}
			}
		}
		euler[0][0] = transmitted[0];
		euler[1][0] = transmitted[1];
		euler[2][0] = transmitted[2];
	}
	void trim_Euler()
	{
		for(int i = 0; i < 3; i++)
		{
			euler[i][0] = moduler((euler[i][0] + M_PI), 2 * M_PI) - M_PI;
		}
	}
	
	void print_euler()
	{
		cout << "roll : " << euler[0][0] << "\tpitch : " << euler[1][0] << "\tyaw : " << euler[2][0] << endl;
	}
};

class Quat{
public:
	double quat[4][1];

	Quat()
	{}
	
	Quat(Euler e)
	{
		double cos_phi_2 = cos(e.euler[0][0]/2);
		double cos_theta_2 = cos(e.euler[1][0]/2);
		double cos_psi_2 = cos(e.euler[2][0]/2);
		double sin_phi_2 = sin(e.euler[0][0]/2);
		double sin_theta_2 = sin(e.euler[1][0]/2);
		double sin_psi_2 = sin(e.euler[2][0]/2);
		
		quat[0][0] = cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2;
		quat[1][0] = sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2;
		quat[2][0] = cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2;
		quat[3][0] = cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2;
	}
	double norm()
	{
		double sum = 0;
		
		for(int i = 0; i < 4; i++)
		{
			sum = quat[i][0] * quat[i][0] + sum;
		}
		sum = sqrt(sum);
		return (sum);
	}
	
	void set_quat(double q[4][1])
	{
		quat[0][0] = q[0][0];
		quat[1][0] = q[1][0];
		quat[2][0] = q[2][0];
		quat[3][0] = q[3][0];
	}
	
	void print_quat()
	{
		cout << quat[0][0] << " " << quat[1][0] << " " << quat[2][0] << " " << quat[3][0] << endl;
	}
};

class Dcm{
public:
	double dcm[3][3];
	
	Dcm()
	{}
	Dcm(Quat q)
	{
		dcm[0][0] = pow(q.quat[0][0], 2) + pow(q.quat[1][0], 2) - pow(q.quat[2][0], 2) - pow(q.quat[3][0], 2);
		dcm[1][1] = pow(q.quat[0][0], 2) - pow(q.quat[1][0], 2) + pow(q.quat[2][0], 2) - pow(q.quat[3][0], 2);
		dcm[2][2] = pow(q.quat[0][0], 2) - pow(q.quat[1][0], 2) - pow(q.quat[2][0], 2) + pow(q.quat[3][0], 2);
		dcm[0][1] = 2 * (q.quat[1][0] * q.quat[2][0] - q.quat[0][0] * q.quat[3][0]);
		dcm[0][2] = 2 * (q.quat[0][0] * q.quat[2][0] + q.quat[1][0] * q.quat[3][0]);
		dcm[1][0] = 2 * (q.quat[1][0] * q.quat[2][0] + q.quat[0][0] * q.quat[3][0]);
		dcm[1][2] = 2 * (q.quat[2][0] * q.quat[3][0] - q.quat[0][0] * q.quat[1][0]);
		dcm[2][0] = 2 * (q.quat[1][0] * q.quat[3][0] - q.quat[0][0] * q.quat[2][0]);
		dcm[2][1] = 2 * (q.quat[2][0] * q.quat[3][0] + q.quat[0][0] * q.quat[1][0]);
	}
	void set_Dcm(double d[3][3])
	{
		dcm[0][0] = d[0][0];
		dcm[0][1] = d[0][1];
		dcm[0][2] = d[0][2];
		dcm[1][0] = d[1][0];
		dcm[1][1] = d[1][1];
		dcm[1][2] = d[1][2];
		dcm[2][0] = d[2][0];
		dcm[2][1] = d[2][1];
		dcm[2][2] = d[2][2];
	}
	void print_dcm()
	{
		cout << dcm[0][0] << " " << dcm[0][1] << " " << dcm[0][2] << endl;
		cout << dcm[1][0] << " " << dcm[1][1] << " " << dcm[1][2] << endl;
		cout << dcm[2][0] << " " << dcm[2][1] << " " << dcm[2][2] << endl;
	}
	void transpose()
    {
        double mat[3][3] = {0};
        
        for(int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                mat[i][j] = dcm[j][i];
            }
        }
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                dcm[i][j] = mat[i][j];
            }
        }
    }
};

class Pqr{
public:
	double p;
	double q;
	double r;
	
	Pqr(double p1, double q1, double r1)
	{
		p = p1;
		q = q1;
		r = r1;
	}
	void set_pqr(double p1, double q1, double r1)
	{
		p = p1;
		q = q1;
		r = r1;
	}
	void print_pqr()
	{
		cout << "p: " << p << " q: " << q << " r: " << r << endl;
	} 
};

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


int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Socket(int domain, int type, int protocol);


Pqr find_pqr_err(Data data, Pqr des_pqr);
Pqr find_des_pqr(Dcm a, Quat b, Gain gain);
int serialOpen_t1(const char *device, const int baud);
int serialOpen_t5(const char *device, const int baud);
long double timeret(timeval t);
void error_handling(const char *message);
Quat quat_mult(Quat q1, Quat q2);
Quat quat_inv(Quat quat);
char			**ft_split(const char *s, char c, int *len);
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
