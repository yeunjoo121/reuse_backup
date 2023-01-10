#include "hils_pi.hpp"
#define BUF_SIZE 1024
#define _POSIX_C_SOURCE 200809L
#define SERVO1	22
#define SERVO2	23
#define DUCT1	17
#define DUCT2	18

//duct 22, 27 servo 12 13
//buffer는 추후 구현 예정. linked list로 구현해야 할 듯. 따라서 get_data는 나중에.
const char *port0 = "/dev/ttyAMA0";
const char *port3 = "/dev/ttyAMA1";
char star = '*';
int baud = 115200;
int port = 4434;
const char *server_addr = "192.168.0.9";

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
	char **splited_str_udp;
	char s;
	int splited_str_len;
	int splited_str_udp_len;

	if ((fd0 = serialOpen_t1(port0, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	if ((fd3 = serialOpen_t5(port3, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	// serial open end
	//wiringpi setup
	if(wiringPiSetup()==-1)
		return 1;

	softPwmCreate(SERVO1, 0, 200);
	softPwmCreate(SERVO2, 0, 200);
	softPwmCreate(DUCT1, 0, 200);
	softPwmCreate(DUCT2, 0, 200);
	
	softPwmWrite(SERVO1,15);
	softPwmWrite(SERVO2, 15);
	softPwmWrite(DUCT1, 11);
	softPwmWrite(DUCT2, 11);
	
		//udp open
	int serv_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	int clnt_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	int str_len = 0;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t serv_len = sizeof(serv_addr);
	socklen_t clnt_len = sizeof(clnt_addr);
	
	bzero(&serv_addr, serv_len);
	bzero(&clnt_addr, clnt_len);
	char message[BUF_SIZE];
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	clnt_addr.sin_family = AF_INET;
	clnt_addr.sin_addr.s_addr = inet_addr(server_addr);
	clnt_addr.sin_port = htons(port);
	
	Bind(serv_fd, (struct sockaddr *)&serv_addr, serv_len);
	
	struct timeval optVal = {0, 100};
	int optLen = sizeof(optVal);
	setsockopt(serv_fd, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen);//set timeout
	
	if (serv_fd == -1)
		error_handling("socket() error");

	//udp open end
	
	Data data = {0};
	Command command = {0};
	Gain gain = {0.1362, -0.0864, -0.0864, 3.9441, 2.5318, 2.5318};
	LinkedDeque *LD = createLinkedDeque();
	DequeNode DN = {0};
	printf("start\n");
	Euler euler;
	/*
	 * pi setup part. python code 208 - 211
	 * */
	
	struct timeval start_time_s, avr_start_s, gcs_loop_start_s, loop_start_s, cur_time_s, send_time_s;
	long double start_time, avr_start, gcs_loop_start, loop_start, cur_time, send_time;
	double avr_loop_time, n_loop;
	gettimeofday(&start_time_s, NULL);
	start_time = timeret(start_time_s);
	avr_loop_time = 0;
	gettimeofday(&avr_start_s, NULL);
	avr_start = timeret(avr_start_s);
	n_loop = 0;
	gettimeofday(&gcs_loop_start_s, NULL);
	gcs_loop_start = timeret(gcs_loop_start_s);
	while (1)
	{
		gettimeofday(&loop_start_s, NULL);
		loop_start = timeret(loop_start_s);
		if (avr_start + 1 < loop_start)
		{
			avr_loop_time = (loop_start - avr_start) / (double)n_loop;
			cout << loop_start - avr_start << endl;
			cout << n_loop << endl;
			avr_start = loop_start;
			n_loop = 1;
			cout << avr_loop_time << endl;
		}
		else
			n_loop = n_loop + 1;

		if (gcs_loop_start + 0.1 < loop_start)
		{
			gcs_loop_start = loop_start;

			//udp receive
			/*receive string from client except \n*/
			str_len = recvfrom(serv_fd, message, BUF_SIZE, 0, NULL, NULL);
			
			if (str_len > 0)
			{
				//\r\n 제거
				message[strlen(message) - 1] = 0;
				message[strlen(message) - 1] = 0;
				
				splited_str_udp = ft_split(message, ',', &splited_str_udp_len);
				euler.str_to_float(splited_str_udp, splited_str_udp_len);
				data.roll_des = euler.euler[0][0];
				data.pitch_des = euler.euler[1][0];
				data.yaw_des = euler.euler[2][0];
				cout << round(((data.roll_des/M_PI * 180) * 100000)) / 100000 << " " << round(((data.pitch_des/M_PI * 180) * 100000)) / 100000 << " " << round(((data.yaw_des/M_PI * 180) * 100000)) / 100000 << endl;
				euler.set_euler(0, 0, 0);//euler를 통해 data의 des값 초기화 후 euler를 0으로 초기
				
				//받은 것 다시 보내기화
				gettimeofday(&send_time_s, NULL);
				send_time = timeret(send_time_s);

				string msg = to_string(round(((send_time - start_time) * 100000)) / 100000);
				msg.append(", ").append(to_string(round((data.roll * 100000)) / 100000)).append(", ").append(to_string(round((data.pitch * 100000)) / 100000)).append(", ").append(to_string(round((data.yaw * 100000)) / 100000));
				msg.append(", ").append(to_string(round((data.p * 100000)) / 100000)).append(", ").append(to_string(round((data.q * 100000)) / 100000)).append(", ").append(to_string(round((data.r * 100000)) / 100000));
				msg.append(", ").append(to_string(round((command.edf_upper * 100000)) / 100000)).append(", ").append(to_string(round((command.edf_lower * 100000)) / 100000));
				msg.append(", ").append(to_string(round((command.servo_outer * 100000)) / 100000)).append(", ").append(to_string(round((command.servo_inner * 100000)) / 100000));
				msg.append(", ").append("\r\n");
				sendto(clnt_fd, msg.c_str(), msg.length(), 0, (struct sockaddr*)&clnt_addr, clnt_len);
			}
			memset(message, 0, BUF_SIZE);
		}
		serialPutchar(fd0, '*');
		gettimeofday(&cur_time_s, NULL);
		cur_time = timeret(cur_time_s);
		while(loop_start + 0.008 > cur_time)
		{
			s = serialGetchar(fd0);
			if (s == '*')
				break;
			/*else if (s != '\0')
				cout << s << endl;*/
			gettimeofday(&cur_time_s, NULL);
			cur_time = timeret(cur_time_s);
		}
		
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
			//printf("receive data = %s", datainput);
			
			// 여기서 연산
			for (int i = 0; i < count; i++)
				datainput[strlen(datainput) - 1] = 0;//\r\n제거
				
			splited_str = ft_split(datainput, ',', &splited_str_len);
			
			data.str_to_float(splited_str, splited_str_len);
			
			//free
			free_splited_str(splited_str);
			
			//print_data(data);
			// data 버퍼에 저장
			DN.data = data;
			insertRearLD(LD, DN);
			// if hils, no need to move servo
			
			//pwm값 시뮬레이터 pc로 보내기. and pwm연산
			double torque_2_w_f[2][1] = {{-1019.4}, {-1019.4}};
			double max_rpm = 300;
			double min_rpm = 0;//should be zero
			double base_rpm = 200; 
			double diff_w_f_margin = 50;
			double max_gimbal_ang = 5/180 * M_PI;
			double des_diff_w_f[2][1] = {0};
			
			euler.set_euler(data.roll, data.pitch, data.yaw);
			euler.trim_Euler();
			Quat quat = Quat(euler);
			Dcm dcm_n2b = Dcm(quat);
			dcm_n2b.transpose();
			Euler des_Euler = Euler(data.roll_des, data.pitch_des, data.yaw_des);
			Quat des_quat = Quat(des_Euler);
			Quat del_quat = quat_mult(des_quat, quat_inv(quat));
			if (del_quat.quat[1][0] < 0)
			{
				for (int i = 1; i < 4; i++)
				{
					del_quat.quat[i][0] = -del_quat.quat[i][0];
				}
			}
			Pqr des_pqr = find_des_pqr(dcm_n2b, del_quat, gain);
			Pqr pqr_err = find_pqr_err(data, des_pqr);
			des_diff_w_f[0][0] = torque_2_w_f[0][0]*gain.p*pqr_err.p;
			des_diff_w_f[1][0] = torque_2_w_f[1][0]*gain.p*pqr_err.p;
			des_diff_w_f[0][0] = min(max(des_diff_w_f[0][0], -diff_w_f_margin), diff_w_f_margin);
			des_diff_w_f[1][0] = min(max(des_diff_w_f[1][0], -diff_w_f_margin), diff_w_f_margin);
			double des_delta_1 = gain.q * pqr_err.q;
			double des_delta_2 = gain.r * pqr_err.r;
			
			command.edf_upper = min(max(base_rpm + des_diff_w_f[0][0], min_rpm), max_rpm);
			command.edf_lower = min(max(-base_rpm + des_diff_w_f[1][0], -max_rpm), -min_rpm);
			command.servo_outer = min(max(des_delta_1, -max_gimbal_ang), max_gimbal_ang);
			command.servo_inner = min(max(des_delta_2, -max_gimbal_ang), max_gimbal_ang);
			
			int edf_max_pwm = 18;
			int edf_min_pwm = 11;
			int servo_max_pwm = 15 + 1; 
			int servo_min_pwm = 15 - 1;
			
			//pwm send
			softPwmWrite(DUCT1,(command.edf_upper - min_rpm) / (max_rpm - min_rpm) * (edf_max_pwm - edf_min_pwm) + edf_min_pwm);
			softPwmWrite(DUCT2, (command.edf_lower - min_rpm) / (max_rpm - min_rpm) * (edf_max_pwm - edf_min_pwm) + edf_min_pwm);
			softPwmWrite(SERVO1, command.servo_outer / max_gimbal_ang * (servo_max_pwm - servo_min_pwm) / 2 + 15);
			softPwmWrite(SERVO2, command.servo_inner / max_gimbal_ang * (servo_max_pwm - servo_min_pwm) / 2 + 15);
			
			string msg = to_string(round(((command.edf_upper) * 1000)) / 1000);
			msg.append(", ").append(to_string(round((command.edf_lower * 1000)) / 1000)).append(", ").append(to_string(round((command.servo_outer * 1000)) / 1000)).append(", ").append(to_string(round((command.servo_inner * 1000)) / 1000)).append("\r\n");
			sendto(clnt_fd, msg.c_str(), msg.length(), 0, (struct sockaddr*)&clnt_addr, clnt_len);
		}
		gettimeofday(&cur_time_s, NULL);
		cur_time = timeret(cur_time_s);
		while (loop_start + 0.009 > cur_time)
		{
			usleep(100);//100microseconds = 0.0001sec
			gettimeofday(&cur_time_s, NULL);
			cur_time = timeret(cur_time_s);
		}
	}
	displayLinkedDeque(LD);
	deleteLinkedDeque(LD);
	close(serv_fd); //end udp socket
	return (0);
}
