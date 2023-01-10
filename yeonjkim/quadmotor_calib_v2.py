import RPi.GPIO as GPIO
import pigpio
import time

#GPIO.setmode(GPIO.BOARD)
pi = pigpio.pi()

motorA_pin = 27;#GPIO27 - pin13 아래  
motorB_pin = 22;#GPIO22 - pin15 위 
#motorC_pin = 17;#GPIO4 - pin7

goal_pwm0 = 1200
goal_pwm1 = 1300
goal_pwm2 = 1400
goal_pwm3 = 1500
goal_pwm4 = 1600
goal_pwm5 = 1700
goal_pwm6 = 1800

goal_delay = 4

ESC_PWM_MIN = 1150
ESC_PWM_START = 1200
ESC_PWM_MAX = 1800

# ESC_PWM_MIN = 1450
# ESC_PWM_START = 1500
# ESC_PWM_MAX = 2000


print("connect ok")
try:
    
#     for j in range(21):
#             
#         pi.set_servo_pulsewidth(motorA_pin,1200+5*j) #아래
#         time.sleep(0.2)
#         
#         print(j)
#         print(ESC_PWM_START + j*5)
#     
    for i in range(121):
        if ((1200 + 5 * i) == goal_pwm0):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm1):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm2):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm3):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm4):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm5):
            ti = goal_delay;
        elif ((1200 + 5 * i) == goal_pwm6):
            ti = goal_delay;
        else:
            ti = 0.1;
        
        print(i)
        print(ESC_PWM_START + i*5)
        pi.set_servo_pulsewidth(motorB_pin,1200+5*i) #아래 
        #pi.set_servo_pulsewidth(motorB_pin, i*50+ ESC_PWM_MIN)
        pi.set_servo_pulsewidth(motorA_pin,1150) #위 
        #pi.set_servo_pulsewidth(motorA_pin, i*50 + ESC_PWM_MIN) 
        time.sleep(float(ti))
            

    #stop
    pi.set_servo_pulsewidth(motorA_pin, ESC_PWM_MIN)
    pi.set_servo_pulsewidth(motorB_pin, ESC_PWM_MIN)
    #pi.set_servo_pulsewidth(motorC_pin, ESC_PWM_MIN)

except KeyboardInterrupt:
    pi.set_servo_pulsewidth(motorA_pin, ESC_PWM_MIN)
    pi.set_servo_pulsewidth(motorB_pin, ESC_PWM_MIN)
    