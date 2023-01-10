import RPi.GPIO as GPIO
import pigpio
import serial
import time
import numpy as np
import math

np.set_printoptions(precision=6, suppress = True)

pi = pigpio.pi()

class Data:
    # transmitted의 첫번째는 sensor 값 받아오는 것, 두번째는 process하는 것, 세번째는 
    transmitted = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    Roll = 0.0
    Pitch = 0.0
    Yaw = 0.0
    Gyro_X = 0.0
    Gyro_Y = 0.0
    Gyro_Z = 0.0
    X_acc = 0.0
    Y_acc = 0.0
    Z_acc = 0.0
        
    #0부터 9, '.', '-'이외의 값 들어오면 이전값으로 계속 있음. 정해진 값 들어와야 바꿈.
    def str_to_float(self, splited_str): 
        for j in range(0, 9):# transmitted에 다 넣고 나중에 분리
            error = 0
            decimalP = 0
            minus = 0
            for i in range(0, len(splited_str[j])):
                if (not((splited_str[j][i] >= '0' and splited_str[j][i] <= '9') or (splited_str[j][i] == '.')
                      or (splited_str[j][i] == '-'))):
                   
                    error = 1
                if (splited_str[j][i] == '.'):
                    decimalP += 1
                    if (decimalP >= 2):
                        error = 1
                elif (splited_str[j][i] == '-'):
                    minus += 1
                    if (minus >= 2):
                        error = 1
            if (error == 0):
                self.transmitted[j] = float(splited_str[j])
            elif (error == 1):# filtering 단계에서 바꾸어줌. 센서들로 추측값을 넣던지, 혹은 이전 값을 넣던지.
                self.transmitted[j] = np.NaN
                
        self.Roll = self.transmitted[0]
        self.Pitch = self.transmitted[1]
        self.Yaw = self.transmitted[2]
        self.Gyro_X = self.transmitted[3]
        self.Gyro_Y = self.transmitted[4]
        self.Gyro_Z = self.transmitted[5]
        self.X_acc = self.transmitted[6]
        self.Y_acc = self.transmitted[7]
        self.Z_acc = self.transmitted[8]

    # data 출력하는 함수
    def print_data(self):
        print("Roll : {0} Pitch : {1} Yaw : {2}".format(self.Roll, self.Pitch, self.Yaw))
        print("Gyro_X : {0} Gyro_Y : {1} Gyro_Z : {2}".format(self.Gyro_X, self.Gyro_Y, self.Gyro_Z))
        print("X_acc : {0} Y_acc : {1} Z_acc : {2}".format(self.X_acc, self.Y_acc, self.Z_acc))
        print("\n")
        
    def get_data(self):
        arr = np.array([[self.Roll, self.Pitch, self.Yaw, self.Gyro_X, self.Gyro_Y, self.Gyro_Z,
                        self.X_acc, self.Y_acc, self.Z_acc]])
        return arr

def main():
    #buffer1 = ([[0, 0, 0, 0, 0, 0, 0, 0, 0]])

    print("start")
    
    EBIMU = serial.Serial('/dev/ttyAMA0', 115200)
    """
    EBIMU.write(b'<lf>') # b는 binary, <lf>는 초기값 load
    time.sleep(0.5)
    EBIMU.write(b'<sor0>') # <sor0>는 polling mode (set output rate)
    time.sleep(0.1)
    EBIMU.write(b'<sof1>') # <sof1>는 Euler Angles 출력
    time.sleep(0.1)
    EBIMU.write(b'<sog1>') # <sog1>은 자이로(각속도) 데이터 출력
    time.sleep(0.1)
    EBIMU.write(b'<soa1>') # <soa1>는 가속도 데이터 출력 
    time.sleep(0.1)
    """
    EBIMU.flushInput()
    """
    transmitted_str = "-172.12,-79.86,74.4,0.0,0.0,0.0,-0.986,-0.022,-0.175"
    splited_str = transmitted_str.split(',')
    """
    data = Data()
    start_time = time.time()
    #print(before)
    count = 0;
    file = open('/home/pi/minjae/test.txt', 'w')
    
    while (True):
    #sensor data
        loop_start = time.time()
        EBIMU.write(b'*') # polling mode에서 데이터 받아오기 시작
        s = EBIMU.read(1)   # 데이터 하나씩만 읽어옴
        if (s == b'*'):
            datainput = b''
            while (s != b'\n'):
                s = EBIMU.read(1)
                datainput = datainput + s
                
            temp = datainput.decode('utf-8')
            file.write(temp)
            datainput = datainput[0:len(datainput) - 2] #\r\n제거
            temp = datainput.decode('utf-8') # 바이트 객체로 정의된 문자열을 유니코드 문자열로 변환
            splited_str = temp.split(',')

            data.str_to_float(splited_str)
            data.print_data()
            loop_time = time.time()
            print(loop_time - start_time)
            file.write(str(loop_time - start_time) + '\n')
            #data.print_data()
            #buffer1 = np.append(buffer1, data.get_data(), axis = 0)
            
            while (loop_start + 0.01 > time.time()):
                time.sleep(0.0001)
            
            """pwm send
            a = 1500-(100/9*(data.Yaw))
            b = 1334+(50*(data.Pitch)/9)
            if (a <= 500):
                a = 500
            elif (a >= 2500):
                a = 2500
                
            if (b <= 500):
                b = 500
            elif (b >= 2500):
                b = 2500

            pi.set_servo_pulsewidth(7, a)
            pi.set_servo_pulsewidth(12, a)
            """
            
if __name__ == "__main__":
    main()