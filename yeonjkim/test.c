#include <stdio.h>
#include <sys/time.h>

void dot_3x3_3x1(double dcm_n2b[3][3], double del_quat[4][1], double res[3][1])
{
	int sum = 0;
	for(int i = 0; i < 3; i++)
	{
		sum = 0;
		for(int j = 0; j < 3; j++)
		{
			sum = sum + dcm_n2b[i][j] * del_quat[j + 1][0];
		}
		res[i][0] = sum;
	}
}

int main(void)
{
	double dcm_n2b[3][3] = {{-20, 20, 10}, {4, -10, 28}, {22, 20, 4}};
	double del_quat[4][1] = {{9}, {8}, {7}, {6}};
	double res[3][1] = {0};
	dot_3x3_3x1(dcm_n2b, del_quat, res);
	printf("%f\n%f\n%f\n", res[0][0], res[1][0], res[2][0]); 
	return 0;
}
