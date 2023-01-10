#define _POSIX_C_SOURCE 200809L
#include "ebimu_c.h"
#include <sys/time.h>
#include <softPwm.h> 

//buffer는 추후 구현 예정. linked list로 구현해야 할 듯. 따라서 get_data는 나중에.
char *port = "/dev/ttyAMA0";
char star = '*';
int baud = 115200;


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
	data->roll = transmitted[0];
	data->pitch = transmitted[1];
	data->yaw = transmitted[2];
	data->gyro_x = transmitted[3];
	data->gyro_y = transmitted[4];
	data->gyro_z = transmitted[5];
	data->x_acc = transmitted[6];
	data->y_acc = transmitted[7];
	data->z_acc = transmitted[8];
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

double timediff(struct timeval before, struct timeval after)//return time by second
{
	double time;
	
	time = (after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec) / 1000000.0;
	return (time);
}

double curtime(struct timeval cur)
{
	double time;
	time = cur.tv_sec + cur.tv_usec / 1000000.0;
	return time;
}
int main(void)
{
	//serial open
	int fd;
	char **splited_str;
	char s;
	int splited_str_len;
	/*variables for measure time*/
	double timebuffer[5] = {0};//sensor, parsing, savedata, pwnprocessing, pwmsending
	char *name[5] = {"sensor", "parsing", "savedata", "pwmprocessing", "pwmsending"};
	struct timeval after;
	struct timeval before;
	struct timeval timeafter1, timeafter2, timeafter3, timeafter4, timeafter5;
	struct timeval timebefore1, timebefore2, timebefore3, timebefore4, timebefore5;
	int count = 0;
	/*variable declare end*/
	if(wiringPiSetup()==-1)
		return 1;
	softPwmCreate(1, 0, 200);
	
	if ((fd = serialOpen(port, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}

	// serial open end
	
	Data data = {0};
	LinkedDeque *LD = createLinkedDeque();
	DequeNode DN = {0};
	printf("start\n");
	
	gettimeofday(&before, NULL);
	count = 0;
	while(1)
	{
		gettimeofday(&timebefore1, NULL);
		serialPutchar(fd, '*');
		s = serialGetchar(fd);
		if (s == '*')
		{
			char datainput[100] = {0};
			while (1)
			{
				if (s == '\n')
					break;
				if (datainput[0] != '\0')
					if (datainput[strlen(datainput) - 2] == '\r')
						break;
				s = serialGetchar(fd);
				datainput[strlen(datainput)] = s;
			}
			
			gettimeofday(&timeafter1, NULL);
			timebuffer[0] = timediff(timebefore1, timeafter1) + timebuffer[0];
			
			gettimeofday(&timebefore2, NULL);
			
			datainput[strlen(datainput) - 1] = 0;//\r제거
			datainput[strlen(datainput) - 1] = 0;//\n제거
			splited_str = ft_split(datainput, ',', &splited_str_len);
			
			str_to_float(splited_str, &data, splited_str_len);
			
			//free
			free_splited_str(splited_str);
			
			gettimeofday(&timeafter2, NULL);
			timebuffer[1] = timediff(timebefore2, timeafter2) + timebuffer[1];
			
			//print_data(data);
			
			gettimeofday(&timebefore3, NULL);
			
			// data 버퍼에 저장
			DN.data = data;
			insertRearLD(LD, DN);
			count++;
			
			gettimeofday(&timeafter3, NULL);
			timebuffer[2] = timediff(timebefore3, timeafter3) + timebuffer[2];
			gettimeofday(&after, NULL);
			
			if (timediff(before, after) >= 720)
			{
				printf("count : %d\n", count);
				for(int k = 0; k < 5; k++)
				{
					printf("%s : %lf\t", name[k], timebuffer[k]/count);
				}
				printf("\n");
				break;
			}
			// if hils, no need to move servo
			gettimeofday(&timebefore4, NULL);
			//pwm processing
			int a = 1500 - (100 / 9 * (data.yaw));
			int b = 1334 + (50 * (data.pitch) / 9);
			if (a <= 500)
				a = 500;
			else if (a >= 2500)
				a = 2500;
			if (b <= 500)
				b = 500;
			else if (b >= 2500)
				b = 2500;
			gettimeofday(&timeafter4, NULL);
			timebuffer[3] = timediff(timebefore4, timeafter4) + timebuffer[3];
			// send angle or rpm
			gettimeofday(&timebefore5, NULL);
			softPwmWrite(1, 20);
			softPwmWrite(1, 21);
			softPwmWrite(1, 22);
			softPwmWrite(1, 23);
			gettimeofday(&timeafter5, NULL);
			timebuffer[4] = timediff(timebefore5, timeafter5) + timebuffer[4];
			/*
			char *message = "100.401,100.402,100.403,100.404\r\n";
			printf("send data : %s", message);
			serialPuts(fd3, message);*/
		}
	}
	//displayLinkedDeque(LD);
	deleteLinkedDeque(LD);
	return (0);
}
