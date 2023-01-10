#include "hils_pi.hpp"

//buffer는 추후 구현 예정. linked list로 구현해야 할 듯. 따라서 get_data는 나중에.
const char *port0 = "/dev/ttyAMA0";
const char *port3 = "/dev/ttyAMA1";
char star = '*';
int baud = 115200;

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
	
	Data data;
	Gain gain = {0.1362, -0.0864, -0.0864, 3.9441, 2.5318, 2.5318};
	Command command = {0};
	
	LinkedDeque *LD = createLinkedDeque();
	DequeNode DN;
	cout << "start" << endl;

	for (int k = 0; k < 10; k++)
	{
		serialPutchar(fd0, '*');
		s = serialGetchar(fd0);
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
				s = serialGetchar(fd0);
				datainput[strlen(datainput)] = s;
			}
			cout << "receive data = " << datainput << endl; 
			
			// 여기서 연산
			datainput[strlen(datainput) - 1] = 0;//\r제거
			datainput[strlen(datainput) - 1] = 0;//\n제거
			splited_str = ft_split(datainput, ',', &splited_str_len);
			
			data.str_to_float(splited_str, splited_str_len);
			
			//free
			free_splited_str(splited_str);
			
			//print_data(data);
			// data 버퍼에 저장
			DN.data = data;
			insertRearLD(LD, DN);
			// if hils, no need to move servo
			
			// send angle or rpm
			const char *message = "100.401,100.402,100.403,100.404\r\n";
			printf("send data : %s", message);
			serialPuts(fd3, message);
		}
	}
	displayLinkedDeque(LD);
	deleteLinkedDeque(LD);
	return (0);
}
