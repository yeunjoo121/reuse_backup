#include <iostream>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

class Euler{
	private 
}
double moduler(double dividend, double divisor);

void Euler2quat(double euler[3][1], double quat[4][1])
{
	double cos_phi_2 = cos(euler[0][0]/2);
	double cos_theta_2 = cos(euler[1][0]/2);
	double cos_psi_2 = cos(euler[2][0]/2);
	double sin_phi_2 = sin(euler[0][0]/2);
	double sin_theta_2 = sin(euler[1][0]/2);
	double sin_psi_2 = sin(euler[2][0]/2);
	
	quat[0][0] = cos_phi_2 * cos_theta_2 * cos_psi_2 + sin_phi_2 * sin_theta_2 * sin_psi_2;
	quat[1][0] = sin_phi_2 * cos_theta_2 * cos_psi_2 - cos_phi_2 * sin_theta_2 * sin_psi_2;
	quat[2][0] = cos_phi_2 * sin_theta_2 * cos_psi_2 + sin_phi_2 * cos_theta_2 * sin_psi_2;
	quat[3][0] = cos_phi_2 * cos_theta_2 * sin_psi_2 - sin_phi_2 * sin_theta_2 * cos_psi_2;
}

void quat2dcm(double quat[4][1], double dcm[3][3])
{
	dcm[0][0] = pow(quat[0][0], 2) + pow(quat[1][0], 2) - pow(quat[2][0], 2) - pow(quat[3][0], 2);
	dcm[1][1] = pow(quat[0][0], 2) - pow(quat[1][0], 2) + pow(quat[2][0], 2) - pow(quat[3][0], 2);
	dcm[2][2] = pow(quat[0][0], 2) - pow(quat[1][0], 2) - pow(quat[2][0], 2) + pow(quat[3][0], 2);
	dcm[0][1] = 2 * (quat[1][0] * quat[2][0] - quat[0][0] * quat[3][0]);
	dcm[0][2] = 2 * (quat[0][0] * quat[2][0] + quat[1][0] * quat[3][0]);
	dcm[1][0] = 2 * (quat[1][0] * quat[2][0] + quat[0][0] * quat[3][0]);
	dcm[1][2] = 2 * (quat[2][0] * quat[3][0] - quat[0][0] * quat[1][0]);
	dcm[2][0] = 2 * (quat[1][0] * quat[3][0] - quat[0][0] * quat[2][0]);
	dcm[2][1] = 2 * (quat[2][0] * quat[3][0] + quat[0][0] * quat[1][0]);
}
void transpose(double arr[3][3])
{
	int mat[3][3] = {0};
	
	for(int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mat[i][j] = arr[j][i];
		}
	}
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			arr[i][j] = mat[i][j];
		}
	}
}
void quat_mult(double quat1[4][1], double quat2[4][1], double res[4][1])
{
	res[0][0] = quat1[0][0] * quat2[0][0] - quat1[1][0] * quat2[1][0] - quat1[2][0] * quat2[2][0] - quat1[3][0] * quat2[3][0];
	res[1][0] = quat1[0][0] * quat2[1][0] + quat1[1][0] * quat2[0][0] + quat1[2][0] * quat2[3][0] - quat1[3][0] * quat2[2][0];
	res[2][0] = quat1[0][0] * quat2[2][0] - quat1[1][0] * quat2[3][0] + quat1[2][0] * quat2[0][0] + quat1[3][0] * quat2[1][0];
	res[3][0] = quat1[0][0] * quat2[3][0] + quat1[1][0] * quat2[2][0] - quat1[2][0] * quat2[1][0] + quat1[3][0] * quat2[0][0];
}


void trim_Euler(double euler[3][1], double res[3][1])
{
	for(int i = 0; i < 3; i++)
	{
		res[i][0] = moduler((euler[i][0] + M_PI), 2 * M_PI) - M_PI;
	}
}

double moduler(double dividend, double divisor)
{
	if (divisor == 0)//divide by zero
	{
		write(-1, "divide by zero\n", 15);
	}
	if (dividend < 0)
	{
		if (fmod(dividend, divisor) != 0)
		{
			return -((((int)(dividend / divisor) - 1) * divisor - dividend));
		}
	}
	return (fmod(dividend, divisor));
}

double norm(double arr[4][1])
{
	double sum = 0;
	for(int i = 0; i < 4; i++)
	{
		sum = arr[i][0] * arr[i][0] + sum;
	}
	sum = sqrt(sum);
	return (sum);
}
void quat_inv(double quat[4][1], double res[4][1])
{
	res[0][0] = quat[0][0] / norm(quat);
	res[1][0] = -quat[1][0] / norm(quat);
	res[2][0] = -quat[2][0] / norm(quat);
	res[3][0] = -quat[3][0] / norm(quat);
}

int main(void)
{
	double quat[4][1] = {{0.03312635}, {0.82946632}, {0.49036859}, {0.26538069}};
	double res[4][1] = {0};
	
	quat_inv(quat, res);
	for(int i = 0; i < 4; i++)
	{
		printf("%.8f\n", res[i][0]);
	}
	return 0;
}
