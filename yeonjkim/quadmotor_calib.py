import RPi.GPIO as GPIO
import pigpio
import time

#GPIO.setmode(GPIO.BOARD)
pi = pigpio.pi()

motorA_pin = 27;#GPIO27 - pin13 아래  
motorB_pin = 22;#GPIO22 - pin15 위 
#motorC_pin = 17;#GPIO4 - pin7

ESC_PWM_MIN = 1175 # 75는 안돌고 80부터 돌기 시작함 
ESC_PWM_START = 1200
ESC_PWM_MAX = 1800

# ESC_PWM_MIN = 1450
# ESC_PWM_START = 1500
# ESC_PWM_MAX = 2000


print("connect ok")
try:
    for i in range(7):
        if i <= 4:
            ti = 3;
        else:
            ti = 3;
        
        print(i)
        print(ESC_PWM_START +100*i)
        pi.set_servo_pulsewidth(motorA_pin,1200+50*i)
        #pi.set_servo_pulsewidth(motorB_pin, i*50+ ESC_PWM_MIN)
        pi.set_servo_pulsewidth(motorB_pin,1200+50*i)
        #pi.set_servo_pulsewidth(motorA_pin, i*50 + ESC_PWM_MIN) 
        time.sleep(float(ti))
            

    #stop
    pi.set_servo_pulsewidth(motorA_pin, ESC_PWM_MIN)
    pi.set_servo_pulsewidth(motorB_pin, ESC_PWM_MIN)
    #pi.set_servo_pulsewidth(motorC_pin, ESC_PWM_MIN)

except KeyboardInterrupt:
    pi.set_servo_pulsewidth(motorA_pin, ESC_PWM_MIN)
    pi.set_servo_pulsewidth(motorB_pin, ESC_PWM_MIN)
    