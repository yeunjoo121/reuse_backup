import RPi.GPIO as GPIO
import serial
import time

#EBIMU = serial.Serial('/dev/ttyUSB0',115200) #<sb5> = 115200 #라즈베리파이에서는 serial.Serial('/dev/ttyUSB0',115200)
EBIMU = serial.Serial('/dev/ttyAMA0',115200) #<sb5> = 115200 #라즈베리파이에서는 serial.Serial('/dev/ttyUSB0',115200)
# 데이터 순서는 Euler Angle, Gyroscope, Accelerometer 순서
""" #기본 세팅은 유지하고, 바꿀 일이 있을 때만 주석 해제 
EBIMU.flushInput() # flushInput은 캐시에 받은 모든 데이터 폐기
EBIMU.write(b'<lf>') # b는 binary, <lf>는 초기값 load
EBIMU.write(b'<sor0>') # <sor0>는 polling mode (set output rate)
EBIMU.write(b'<sof1>') # <sof1>은 Euler Angles 출력
EBIMU.write(b'<sog1>') # <sog1>은 자이로(각속도)데이터 출력함
EBIMU.write(b'<soa1>') # <soa1>은 가속도 데이터 출력
"""

EBIMU.flushInput() # flushInput은 캐시에 받은 모든 데이터 폐기

while True:
    #sensor data
    for i in range(1):
        EBIMU.write(b'*') # polling mode에서 데이터 받아오기 시작
        s=EBIMU.read(1)   # 데이터 하나씩만 읽어옴
        if s == b'*':
            data = b''
            while s != b'\n':
                s=EBIMU.read(1)
                data = data + s
            data = data[0:len(data)-2];
            transmitted_string = data.decode('utf-8') # 바이트 객체로 정의된 문자열을 유니코드 문자열로 변환
            splited_string = transmitted_string.split(',')
            
            Roll = float(splited_string[0]) # -180도 ~ +180도
            Pitch = float(splited_string[1]) # -90도 ~ +90도
            Yaw = float(splited_string[2]) # -180도 ~ +180도
            
            Gyro_X = float(splited_string[3]) # 자이로 데이터의 단위는 DPS(degree per second)
            Gyro_Y = float(splited_string[4])
            Gyro_Z = float(splited_string[5])
            
            X_acc = float(splited_string[6]) # 가속도의 단위는 중력가속도 g, g = 9.81m/s^2
            Y_acc = float(splited_string[7])
            Z_acc = float(splited_string[8])
            
            
            print("Roll : {0} Pitch : {1} Yaw : {2}".format(Roll, Pitch, Yaw))
            print("Gyro_X : {0} Gyro_Y : {1} Gyro_Z : {2}".format(Gyro_X, Gyro_Y, Gyro_Z))
            print("X_acc : {0} Y_acc : {1} Z_acc : {2}".format(X_acc, Y_acc, Z_acc))
            print("\n")
            
EBIMU.close()

