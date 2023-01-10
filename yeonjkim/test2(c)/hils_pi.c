#include "hils_pi.h"

//buffer는 추후 구현 예정. linked list로 구현해야 할 듯. 따라서 get_data는 나중에.
char *port0 = "/dev/ttyAMA0";
char *port3 = "/dev/ttyAMA1";
char star = '*';
int baud = 115200;

double moduler(double dividend, double divisor);
void error_handling(char *message);

typedef struct GainType
{
	float p, q, r, phi, theta, psi;
}	Gain;

typedef struct Command
{
	float edf_upper, edf_lower, servo_outer, servo_inner;
}	Command;

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


void trim_Euler(double euler[3][1])
{
	for(int i = 0; i < 3; i++)
	{
		euler[i][0] = moduler((euler[i][0] + M_PI), 2 * M_PI) - M_PI;
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

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void str_to_float(char **splited_str, Data *data, int len)
{
	float transmitted[9] = {0};
	 
	if (len == 9)// 9이면 숫자나 -, .가 아닌 값이 들어간 데이터를 nan처
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
	data->roll = transmitted[0] /180 * M_PI;
	data->pitch = transmitted[1] / 180 *M_PI;
	data->yaw = transmitted[2] / 180 * M_PI;
	data->p = transmitted[3] / 180 * M_PI;
	data->q = transmitted[4] / 180 * M_PI;
	data->r = transmitted[5] / 180 * M_PI;
	//data->roll_des = transmitted[6];
	//data->pitch_des = transmitted[7];
	//data->yaw_des = transmitted[8];
}

void print_data(Data data)
{
	printf("Roll : %.3f Pitch : %.3f Yaw : %.3f\n", data.roll, data.pitch, data.yaw);
	printf("Gyro_X : %.3f Gyro_Y : %.3f Gyro_Z : %.3f\n", data.gyro_x, data.gyro_y, data.gyro_z);
	printf("X_acc : %.3f Y_acc : %.3f Z_acc : %.3f\n", data.x_acc, data.y_acc, data.z_acc);
}

void free_splited_str(char **str)
{
	int     index;

	index = 0;
	while (!str[index])
	{
		free(str[index++]);
	}
	free(str);
}

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
	//serial open
	int fd0;
	int fd3;
	char **splited_str;
	char s;
	int splited_str_len;

	if ((fd0 = serialOpen(port0, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	if ((fd3 = serialOpen(port3, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	// serial open end
	
	Data data = {0, 0, 0, 0, 0, 0, 0/180*M_PI, 0/180*M_PI, 0/180*M_PI};
	Gain gain = {0.1362, -0.0864, -0.0864, 3.9441, 2.5318, 2.5318};
	Command command = {0};
	
	LinkedDeque *LD = createLinkedDeque();
	DequeNode DN = {0};
	printf("start\n");

	/*
	 * 여기서  servo, EDF에 pwm주기
	 */

	//socket open
	int sock;
	char message[BUF_SIZE];
	socklen_t adr_sz;
	
	struct sockaddr_in gcs_adr, from_adr;
	
	sock = socket(PF_INET, SOCK_DGRAM, 0);//create udp socket
	if (sock == 1)
		error_handling("socket() error");
		
	/*initialize server address info*/
	memset(&gcs_adr, 0, sizeof(gcs_adr));
	to_adr.sin_family=AF_INET;
	gcs_adr.sin_addr.s_addr = inet_addr(gcs_addr);
	gcs_adr.sin_port = htons(gcs_port);
	
	//socket open end
	
	while(1)
	{
		serialPutchar(fd0, '*');
		s = serialGetchar(fd0);
		if (s == '*')
		{
			char datainput[100] = {0};
			int count = 0;
			while (1)
			{
				if (s == '\n')
					break;
				if (datainput[0] != '\0')
					if (datainput[strlen(datainput) - 2] == '\r')
					{
						count++;
						break;
					}
				s = serialGetchar(fd0);
				datainput[strlen(datainput)] = s;
				if (s == '\n' || s == '\r')
					count++;
			}	
			// 여기서 연산
			for(int i = 0; i < count; i++)
				datainput[strlen(datainput) - 1] = 0;//\r\n제거

			splited_str = ft_split(datainput, ',', &splited_str_len);
			
			str_to_float(splited_str, &data, splited_str_len);
			
			//free
			free_splited_str(splited_str);
			
			//print_data(data);
			// data 버퍼에 저장
			DN.data = data;
			insertRearLD(LD, DN);
			// if hils, no need to move servo
			
			//pwm값 시뮬레이터 pc로 보내기. and pwm 연산
			float torque_2_w_f[2][1] = {{-1019.4}, {-1019.4}};
			int max_rpm = 300;
			int min_rpm = 0;//should be zero
			int base_rpm = 200; 
			int diff_w_f_margin = 50;
			float max_gimbal_ang = 5/180 * M_PI;
			
			float euler[3][1] = {{data.roll}, {data.pitch}, {data.yaw}};
			float quat[4][1] = {0};
			float dcm_n2b[3][3] = {0};
			float des_Euler[3][1] = {0};
			float des_quat[4][1] = {0};
			float del_quat[4][1] = {0};
			float del_quat_b[4][1] = {0};
			float del_quat_vec_sq = 0;
			float pqr[3][1] = {0};
			
			trim_Euler(euler);
			quat = Euler2quat(euler, quat);
			dcm_n2b = transpose(quat2dcm(quat));
			des_Euler = {{data.roll_des}, {data.pitch_des}, {data.yaw_des}};
			des_quat = Euler2quat(des_Euler);
			del_quat = quat_mult(des_quat, quat_inv(quat));
			if (del_quat[0][0] < 0)
			{
				for(int i = 1; i < 4; i++)
				{
					del_quat[i][0] = -del_quat[i][0];
				}
			}
			dot_3x3_3x1(dcm_n2b, del_quat, del_quat_b);
			
			del_quat_vec_sq = pow(del_quat[1][0], 2) + pow(del_quat[2][0], 2) + pow(del_quat[3][0], 2);
			pqr 
			command.edf_upper = min()
			int edf_max_pwm = 1800;
			int edf_min_pwm = 1100;
			int servo_max_pwm = 1500 + 50;
			int servo_min_pwm = 1500 - 50;
			
			//send pwm and angle

			
		}
	}
	displayLinkedDeque(LD);
	deleteLinkedDeque(LD);
	return (0);
}
