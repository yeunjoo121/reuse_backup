#include <cmath>
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

typedef struct GainType
{
	double p, q, r, phi, theta, psi;
}	Gain;

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

class Quat{
public:
	double quat[4][1];

	Quat(double a, double b, double c, double d)
	{
		quat[0][0] = a;
		quat[1][0] = b;
		quat[2][0] = c;
		quat[3][0] = d;
	}
};

class Dcm{
public:
	double dcm[3][3];
	
	Dcm()
	{}
	void set_Dcm(double a, double b, double c, double d, double e, double f, double g, double h, double i)
	{
		dcm[0][0] = a;
		dcm[0][1] = b;
		dcm[0][2] = c;
		dcm[1][0] = d;
		dcm[1][1] = e;
		dcm[1][2] = f;
		dcm[2][0] = g;
		dcm[2][1] = h;
		dcm[2][2] = i;
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
	void print_pqr()
	{
		cout << "p: " << p << " q: " << q << " r: " << r << endl;
	} 
};

Pqr find_des_pqr(Dcm a, Quat b, Gain gain)
{
	double del_quat_b[3][1] = {0};
	int sum = 0;
	
	for(int i = 0; i < 3; i++)
	{
		sum = 0;
		for(int j = 0; j < 3; j++)
		{
			sum = sum + a.dcm[i][j] * b.quat[j + 1][0];
		}
		del_quat_b[i][0] = sum;
	}
	Pqr des_pqr = Pqr(gain.phi * del_quat_b[0][0] * 2, gain.theta * del_quat_b[1][0] * 2, gain.psi * del_quat_b[2][0] * 2);
	return (des_pqr);
}

Pqr find_pqr_err(Data data, Pqr des_pqr)
{
	Pqr pqr_err = Pqr(des_pqr.p - data.p, des_pqr.q - data.q, des_pqr.r - data.r);
	return pqr_err;
}

int main(void)
{
	Gain gain = {0, 0, 0, 1, 2, 3};
	float a = 1.0, b = 2.0, c = 3.0, d = 4.0, e = 5.0, f = 6.0, g = 7.0, h = 8.0, i = 9.0;
	Quat quat = Quat(a,b,c,d);
	Dcm dcm;
	dcm.set_Dcm(a,b,c,d,e,f,g,h,i);
	Pqr des_pqr = find_des_pqr(dcm, quat, gain);
	
	Data data = {0};
	data.p = 1;
	data.q = 2;
	data.r = 3;
	Pqr pqr_err = find_pqr_err(data, des_pqr);
	pqr_err.print_pqr();
	return 0;
}
