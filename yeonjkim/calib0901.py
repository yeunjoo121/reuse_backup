import RPi.GPIO as GPIO
import pigpio
import time

pi = pigpio.pi()

MIN_SIGNAL = 1150
MAX_SIGNAL = 1800

motorA_pin = 22
inputint = 0

try:
    print("this program will calibrate the esc")

    pi.set_servo_pulsewidth(motorA_pin, MAX_SIGNAL)
    
    while (inputint != 1):
        inputint = (int)(input("숫자 1 입력하면 esc calibration 시작\n"))
        

    print("sending minimum output")
    
    pi.set_servo_pulsewidth(motorA_pin, MIN_SIGNAL)
        
    
except KeyboardInterrupt:
    pi.set_servo_pulsewidth(motorA_pin, MIN_SIGNAL)