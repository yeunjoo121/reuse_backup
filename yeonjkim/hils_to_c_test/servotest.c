// 예제 소스 작성
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h> 
#include <unistd.h>


#define SERVO 1

int main()
{
	if(wiringPiSetup()==-1)
		return 1;

	softPwmCreate(SERVO, 0, 200);
    

	for(int j = 0; j < 4; j++)
	{
		for(int i = 5; i <= 25; i++)
		{
			softPwmWrite(SERVO,i);
			delay(500);
		}
		for(int i = 25; i >= 5; i--)
		{
			softPwmWrite(SERVO, i);
			delay(500);
		}
	}
	return 0;
}
