from subprocess import call
#call("sudo pigpiod", shell=True)#터미널에서 sudo pigpiod 실행. 별도의 서브 쉘을 실행하고 해당 쉘 위에서 명령 실행
import RPi.GPIO as GPIO
import pigpio
import gpiozero import Servo
import time
"""
GPIO.setmode(GPIO.BOARD)
pi = pigpio.pi()
"""
motorA_pin = 3;
motorB_pin = 5;
motorC_pin = 6;
motorD_pin = 9;


motorA = Servo(motorA_pin)
#motorB = Servo(motorB_pin)
#motorC = Servo(motorC_pin)
#motorD = Servo(motorD_pin)

while True:
    motorA.min()
    sleep(2)
    motorA.mid()
    sleep(2)
    motorA.max()
    sleep(2)
    
