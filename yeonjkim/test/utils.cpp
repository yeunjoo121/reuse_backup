#include "hils_pi.hpp"

double moduler(double dividend, double divisor)
{
	try
	{
		if (divisor == 0)
			throw ("Divide by zero not allowed\n");
		if (dividend < 0)
		{
			if (fmod(dividend, divisor) != 0)
			{
				return -((((int)(dividend / divisor) - 1) * divisor - dividend));
			}
		}
	}
	catch (const char* e)
	{
		cout << "exception caught: " << e;
	}
	return (fmod(dividend, divisor));
}

Quat quat_inv(Quat quat)
{
	Quat res;
	double q[4][1];
	
	q[0][0] = quat.quat[0][0] / quat.norm();
	q[1][0] = -quat.quat[1][0] / quat.norm();
	q[2][0] = -quat.quat[2][0] / quat.norm();
	q[3][0] = -quat.quat[3][0] / quat.norm();
	res.set_quat(q);
	return (res);
}

Quat quat_mult(Quat q1, Quat q2)
{
	Quat res;
	double q[4][1];
	
	q[0][0] = q1.quat[0][0] * q2.quat[0][0] - q1.quat[1][0] * q2.quat[1][0] - q1.quat[2][0] * q2.quat[2][0] - q1.quat[3][0] * q2.quat[3][0];
	q[1][0] = q1.quat[0][0] * q2.quat[1][0] + q1.quat[1][0] * q2.quat[0][0] + q1.quat[2][0] * q2.quat[3][0] - q1.quat[3][0] * q2.quat[2][0];
	q[2][0] = q1.quat[0][0] * q2.quat[2][0] - q1.quat[1][0] * q2.quat[3][0] + q1.quat[2][0] * q2.quat[0][0] + q1.quat[3][0] * q2.quat[1][0];
	q[3][0] = q1.quat[0][0] * q2.quat[3][0] + q1.quat[1][0] * q2.quat[2][0] - q1.quat[2][0] * q2.quat[1][0] + q1.quat[3][0] * q2.quat[0][0];
	
	res.set_quat(q);
	return (res);
}

void error_handling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

long double timeret(timeval t)
{
	long double tm = 0;
	
	tm = t.tv_sec + t.tv_usec / 1000000.0;
	return (tm);
}

Pqr find_des_pqr(Dcm a, Quat b, Gain gain)
{
	double del_quat_b[3][1] = {0};
	double sum = 0;
	
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
	
int serialOpen_t1(const char *device, const int baud)//timeout이 0.0001초
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case      50:	myBaud =      B50 ; break ;
    case      75:	myBaud =      B75 ; break ;
    case     110:	myBaud =     B110 ; break ;
    case     134:	myBaud =     B134 ; break ;
    case     150:	myBaud =     B150 ; break ;
    case     200:	myBaud =     B200 ; break ;
    case     300:	myBaud =     B300 ; break ;
    case     600:	myBaud =     B600 ; break ;
    case    1200:	myBaud =    B1200 ; break ;
    case    1800:	myBaud =    B1800 ; break ;
    case    2400:	myBaud =    B2400 ; break ;
    case    4800:	myBaud =    B4800 ; break ;
    case    9600:	myBaud =    B9600 ; break ;
    case   19200:	myBaud =   B19200 ; break ;
    case   38400:	myBaud =   B38400 ; break ;
    case   57600:	myBaud =   B57600 ; break ;
    case  115200:	myBaud =  B115200 ; break ;
    case  230400:	myBaud =  B230400 ; break ;
    case  460800:	myBaud =  B460800 ; break ;
    case  500000:	myBaud =  B500000 ; break ;
    case  576000:	myBaud =  B576000 ; break ;
    case  921600:	myBaud =  B921600 ; break ;
    case 1000000:	myBaud = B1000000 ; break ;
    case 1152000:	myBaud = B1152000 ; break ;
    case 1500000:	myBaud = B1500000 ; break ;
    case 2000000:	myBaud = B2000000 ; break ;
    case 2500000:	myBaud = B2500000 ; break ;
    case 3000000:	myBaud = B3000000 ; break ;
    case 3500000:	myBaud = B3500000 ; break ;
    case 4000000:	myBaud = B4000000 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 0.001;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}

int serialOpen_t5(const char *device, const int baud)//timeout이 5초
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case      50:	myBaud =      B50 ; break ;
    case      75:	myBaud =      B75 ; break ;
    case     110:	myBaud =     B110 ; break ;
    case     134:	myBaud =     B134 ; break ;
    case     150:	myBaud =     B150 ; break ;
    case     200:	myBaud =     B200 ; break ;
    case     300:	myBaud =     B300 ; break ;
    case     600:	myBaud =     B600 ; break ;
    case    1200:	myBaud =    B1200 ; break ;
    case    1800:	myBaud =    B1800 ; break ;
    case    2400:	myBaud =    B2400 ; break ;
    case    4800:	myBaud =    B4800 ; break ;
    case    9600:	myBaud =    B9600 ; break ;
    case   19200:	myBaud =   B19200 ; break ;
    case   38400:	myBaud =   B38400 ; break ;
    case   57600:	myBaud =   B57600 ; break ;
    case  115200:	myBaud =  B115200 ; break ;
    case  230400:	myBaud =  B230400 ; break ;
    case  460800:	myBaud =  B460800 ; break ;
    case  500000:	myBaud =  B500000 ; break ;
    case  576000:	myBaud =  B576000 ; break ;
    case  921600:	myBaud =  B921600 ; break ;
    case 1000000:	myBaud = B1000000 ; break ;
    case 1152000:	myBaud = B1152000 ; break ;
    case 1500000:	myBaud = B1500000 ; break ;
    case 2000000:	myBaud = B2000000 ; break ;
    case 2500000:	myBaud = B2500000 ; break ;
    case 3000000:	myBaud = B3000000 ; break ;
    case 3500000:	myBaud = B3500000 ; break ;
    case 4000000:	myBaud = B4000000 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 50;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}
