import RPi.GPIO as GPIO
import pigpio
import serial
import time
import numpy as np
import socket
import struct

#----------------------------------------#
#-----------add code for hils------------#
port0 = '/dev/ttyAMA0'
port3 = '/dev/ttyAMA1'
baud = 115200

# ser0 -> send * and receive sensor data
# ser3 -> send angle or rpm
ser0 = serial.Serial(
    port = port0,
    baudrate = baud,
    bytesize = serial.EIGHTBITS,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    timeout = 0.0001
    )

ser3 = serial.Serial(
    port = port3,
    baudrate = baud,
    bytesize = serial.EIGHTBITS,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    timeout = 5
    )

class UDP:
    address = "0.0.0.0"
    port = np.nan
    socket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

star = '*'
null = '\0'
#---------------------------------------#

np.set_printoptions(precision=6, suppress = True)

pi = pigpio.pi()

class Data:
    # transmitted의 첫번째는 sensor 값 받아오는 것, 두번째는 process하는 것, 세번째는 
    transmitted = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    roll = 0.0
    pitch = 0.0
    yaw = 0.0
    p = 0.0
    q = 0.0
    r = 0.0
    roll_des = 0/180*np.pi
    pitch_des = 90.0/180*np.pi
    yaw_des = 0/180*np.pi
        
    #0부터 9, '.', '-'이외의 값 들어오면 이전값으로 계속 있음. 정해진 값 들어와야 바꿈.
    def str_to_float(self, splited_str):
        if (len(splited_str) == 9):# 9이면 숫자나 -, .가 아닌 값이 들어간 데이터를 nan처리
            for j in range(0, 9):
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
        elif (len(splited_str) > 9):# len이 9보다 크면 숫자값 또는 '.', '-'가 ','로 처리된 것이라 어떤 숫자가 ,로 잘못 들어간 것인지 모르므로 전체 data를 nan처리
            for j in range (0, 9):
                self.transmitted[j] = np.NaN
        elif (len(splited_str) < 9):# len이 9보다 작으면 maxlen과 비교
            index = 0# j로 값을 넣게 되면 ,의 개수에 따라 인덱스의 값이 달라진다. 따라서 index를 따로 둔다.
            for j in range(0, len(splited_str)):
                error = 0
                decimalP = 0
                minus = 0
                for i in range(0, len(splited_str[j])):
                    if (i >= 10):# maxlen보다 크면 이 데이터와 다음 데이터 nan처리
                        error = 2
                        break
                    if (not((splited_str[j][i] >= '0' and splited_str[j][i] <= '9') or (splited_str[j][i] == '.')
                      or (splited_str[j][i] == '-'))):
                        error = 1
                    if (splited_str[j][i] == '.'):
                        decimalP += 1
                        if (decimalP >= 2):
                            error = 2
                    elif (splited_str[j][i] == '-'):
                        minus += 1
                        if (minus >= 2):
                            error = 2
                if (error == 0):
                    self.transmitted[index] = float(splited_str[j])
                    index += 1
                elif (error == 1):# filtering 단계에서 바꾸어줌. 센서들로 추측값을 넣던지, 혹은 이전 값을 넣던지.
                    self.transmitted[index] = np.NaN
                    index += 1
                elif (error == 2):# 이번 값과 다음 값을 모두 nan으로 처리해줌. ','가 손실되어 이번 값과 다음 값을 구분해 주는 ,가 없으므로 j를 1 증가시킨다.
                    self.transmitted[index] = np.NaN
                    index += 1
                    self.transmitted[index] = np.NaN
                    index += 1

                
        self.roll = self.transmitted[0]/180*np.pi
        self.pitch = self.transmitted[1]/180*np.pi
        self.yaw = self.transmitted[2]/180*np.pi
        self.p = self.transmitted[3]/180*np.pi
        self.q = self.transmitted[4]/180*np.pi
        self.r = self.transmitted[5]/180*np.pi
        #self.roll_des = self.transmitted[6]
        #self.pitch_des = self.transmitted[7]
        #self.yaw_des = self.transmitted[8]

    
    # data 출력하는 함수
    def print_data(self):
        print("roll: {0}\t\t pitch: {1}\t\t yaw: {2}".format(self.roll, self.pitch, self.yaw))
        print("p: {0}\t\t\t q: {1}\t\t\t r: {2}".format(self.p, self.q, self.r))
        print("roll_des: {0}\t\t pitch_des: {1}\t\t yaw_des: {2}".format(self.roll_des, self.pitch_des, self.yaw_des))
        print("\n")
        
    def get_data(self):
        arr = np.array([[self.roll, self.pitch, self.yaw, self.p, self.q, self.r,
                        self.roll_des, self.pitch_des, self.yaw_des]])
        return arr

class Gain:
    p = 0.1362
    q = -0.0864
    r = -0.0864
    phi = 3.9441
    theta = 2.5318
    psi = 2.5318

class Command:
    edf_upper = 0.0
    edf_lower = 0.0
    servo_outer = 0.0
    servo_inner = 0.0
    
def trim_Euler(Euler):#euler값이 -pi ~ pi사이의 값으로 출력되게 함.
    return (Euler+np.pi)%(2*np.pi)-np.pi

def quat_inv(quat):
    return np.concatenate([quat[0:+1,0:+1],-quat[1:3+1,0:+1]])/np.linalg.norm(quat)

def quat_mult(q_1,q_2):
    return np.array([[q_1[0,0]*q_2[0,0]-q_1[1,0]*q_2[1,0]-q_1[2,0]*q_2[2,0]-q_1[3,0]*q_2[3,0]],
        [q_1[0,0]*q_2[1,0]+q_1[1,0]*q_2[0,0]+q_1[2,0]*q_2[3,0]-q_1[3,0]*q_2[2,0]],
        [q_1[0,0]*q_2[2,0]-q_1[1,0]*q_2[3,0]+q_1[2,0]*q_2[0,0]+q_1[3,0]*q_2[1,0]],
        [q_1[0,0]*q_2[3,0]+q_1[1,0]*q_2[2,0]-q_1[2,0]*q_2[1,0]+q_1[3,0]*q_2[0,0]]])

def quat2dcm(quat):
    dcm = np.zeros((3,3))
    dcm[0,0] = quat[0,0]**2+quat[1,0]**2-quat[2,0]**2-quat[3,0]**2
    dcm[1,1] = quat[0,0]**2-quat[1,0]**2+quat[2,0]**2-quat[3,0]**2
    dcm[2,2] = quat[0,0]**2-quat[1,0]**2-quat[2,0]**2+quat[3,0]**2
    dcm[0,1] = 2*(quat[1,0]*quat[2,0]-quat[0,0]*quat[3,0])
    dcm[0,2] = 2*(quat[0,0]*quat[2,0]+quat[1,0]*quat[3,0])
    dcm[1,0] = 2*(quat[1,0]*quat[2,0]+quat[0,0]*quat[3,0])
    dcm[1,2] = 2*(quat[2,0]*quat[3,0]-quat[0,0]*quat[1,0])
    dcm[2,0] = 2*(quat[1,0]*quat[3,0]-quat[0,0]*quat[2,0])
    dcm[2,1] = 2*(quat[2,0]*quat[3,0]+quat[0,0]*quat[1,0])
    return dcm

def Euler2quat(Euler):
    quat = np.zeros((4,1))
    cos_phi_2 = np.cos(Euler[0,0]/2)
    cos_theta_2 = np.cos(Euler[1,0]/2)
    cos_psi_2 = np.cos(Euler[2,0]/2)
    sin_phi_2 = np.sin(Euler[0,0]/2)
    sin_theta_2 = np.sin(Euler[1,0]/2)
    sin_psi_2 = np.sin(Euler[2,0]/2)
    quat[0,0] = cos_phi_2*cos_theta_2*cos_psi_2+sin_phi_2*sin_theta_2*sin_psi_2
    quat[1,0] = sin_phi_2*cos_theta_2*cos_psi_2-cos_phi_2*sin_theta_2*sin_psi_2
    quat[2,0] = cos_phi_2*sin_theta_2*cos_psi_2+sin_phi_2*cos_theta_2*sin_psi_2
    quat[3,0] = cos_phi_2*cos_theta_2*sin_psi_2-sin_phi_2*sin_theta_2*cos_psi_2
    return quat

def main():
    global star
    global null
    
    print("start")
    
    data = Data()
    gain = Gain()
    command = Command()
    
    pi.set_servo_pulsewidth(17, 1100)
    pi.set_servo_pulsewidth(18, 1100)
    pi.set_servo_pulsewidth(22, 1500)
    pi.set_servo_pulsewidth(23, 1500)
            
    udp_fcc = UDP()
    udp_fcc.address = "192.168.0.66"
    udp_fcc.port = 4434
    udp_fcc.socket.bind((udp_fcc.address,udp_fcc.port))
    udp_fcc.socket.settimeout(0.0001)

    udp_gcs = UDP()
    udp_gcs.address = "192.168.0.9"
    udp_gcs.port = 4434
    udp_gcs.socket.settimeout(0.0001)
    
    # there is no need to set EBIMU
    """
    transmitted_str = "-172.12,-79.86,74.4,0.00.0,0.0,-0.986,-0.022,-0.175"
    splited_str = transmitted_str.split(',')
    data.str_to_float(splited_str)
    data.print_data()
    """
    start_time = time.time()
    avr_loop_time = 0
    avr_start = time.time()
    n_loop = 0
    gcs_loop_start = time.time()
    des_Euler_bytes = []
            
    while (True):
        loop_start = time.time()
        
        if avr_start+1 < loop_start:
            avr_loop_time = (loop_start-avr_start)/n_loop
            print(loop_start - avr_start)
            print(n_loop)
            avr_start = loop_start
            n_loop = 1
            print(avr_loop_time)
        else:
            n_loop = n_loop + 1
            
        if gcs_loop_start+.1 < loop_start:
            gcs_loop_start = loop_start
            try:
                des_Euler_bytes = udp_fcc.socket.recv(12)
                [data.roll_des] = struct.unpack('f',des_Euler_bytes[0:4])
                [data.pitch_des] = struct.unpack('f',des_Euler_bytes[4:8])
                [data.yaw_des] = struct.unpack('f',des_Euler_bytes[8:12])
                if des_Euler_bytes:
                    print(round(data.roll_des/np.pi*180,5),round(data.pitch_des/np.pi*180,5),round(data.yaw_des/np.pi*180,5))
                des_Euler_bytes = []
            except:
                pass
            
            """
            Euler = np.array([[data.roll],[data.pitch],[data.yaw]])
            Euler = trim_Euler(Euler)
            quat = Euler2quat(Euler)
            dcm_n2b = quat2dcm(quat).T
            des_Euler = np.array([[data.roll_des],[data.pitch_des],[data.yaw_des]])
            des_quat = Euler2quat(des_Euler)
            del_quat = quat_mult(des_quat,quat_inv(quat))
            if del_quat[0,0] < 0:
                del_quat[1:3+1,0:+1] = -del_quat[1:3+1,0:+1]
            del_quat_b = dcm_n2b@del_quat[1:3+1,0:+1]
            
            del_quat_vec_sq = del_quat[1,0]**2+del_quat[2,0]**2+del_quat[3,0]**2
            """
            
            msg = str(round(time.time()-start_time,5))
            msg = msg + ', ' + str(round(data.roll,5)) + ', ' + str(round(data.pitch,5)) + ', ' + str(round(data.yaw,5))
            msg = msg + ', ' + str(round(data.p,5)) + ', ' + str(round(data.q,5)) + ', ' + str(round(data.r,5))
            msg = msg + ', ' + str(round(command.edf_upper,5)) + ', ' + str(round(command.edf_lower,5))
            msg = msg + ', ' + str(round(command.servo_outer,5)) + ', ' + str(round(command.servo_inner,5))
            msg = msg + ', ' + '\r' + '\n'
            #msg = msg + ', ' + str(round(del_quat_vec_sq,5)) + '\r' + '\n'
            udp_gcs.socket.sendto(bytes(msg,'utf-8'),(udp_gcs.address,udp_gcs.port))
            
        #print('star write')
        ser0.write(star.encode()) # *하나 써서 데이터 받아오기 시작
        #print(str(time.time()-start_time)+':  * sent')
        while loop_start+0.008>time.time():
            s = ser0.read().decode()# 데이터 하나씩 읽어옴
            if s == '*':
                break
            elif s != '':
                print(s)
        if (s == '*'):
            
            #print(str(time.time()-start_time)+':  * received')
            datainput = ''
            count = 0
            while (True):
                if (s == '\n'):
                    break
                if (datainput != ''):
                    if (datainput[len(datainput) - 2] == '\r'):
                        count += 1
                        break
                s = ser0.read().decode()
                datainput = datainput + s
                if s == '\n' or s == '\r':
                    count += 1

            #print('received data = ' + datainput)
            
            # 여기서 연산
            datainput = datainput[0:len(datainput) - count] # \r\n제거
            splited_str = datainput.split(',')

            data.str_to_float(splited_str)
            
            
            #data.print_data()
            #buffer1 = np.append(buffer1, data.get_data(), axis = 0) # data 저장
            
            # pwm값 desktop으로 보내기 & pwm 연산
            torque_2_w_f = np.array([[-1019.4],[-1019.4]])
            max_rpm = 300
            min_rpm = 0 # should be zero
            base_rpm = 200
            diff_w_f_margin = 50
            max_gimbal_ang = 5/180*np.pi
            
            Euler = np.array([[data.roll],[data.pitch],[data.yaw]])
            Euler = trim_Euler(Euler)
            quat = Euler2quat(Euler)
            dcm_n2b = quat2dcm(quat).T
            des_Euler = np.array([[data.roll_des],[data.pitch_des],[data.yaw_des]])
            des_quat = Euler2quat(des_Euler)
            del_quat = quat_mult(des_quat,quat_inv(quat))
            if del_quat[0,0] < 0:
                del_quat[1:3+1,0:+1] = -del_quat[1:3+1,0:+1]
            del_quat_b = dcm_n2b@del_quat[1:3+1,0:0+1]
            
            #del_quat_vec_sq = del_quat[1,0]**2+del_quat[2,0]**2+del_quat[3,0]**2

            pqr = np.array([[data.p],[data.q],[data.r]])
            #Euler_err = trim_Euler(des_Euler-Euler)
            #des_pqr = np.array([[gain.phi],[gain.theta],[gain.psi]])*Euler_err
            des_pqr = np.array([[gain.phi],[gain.theta],[gain.psi]])*del_quat_b*2
            pqr_err = des_pqr-pqr
            des_diff_w_f = torque_2_w_f*gain.p*pqr_err[0,0] # pqr_err[0,0]: p_err
            des_diff_w_f[0,0] = min(max(des_diff_w_f[0,0],-diff_w_f_margin),diff_w_f_margin)
            des_diff_w_f[1,0] = min(max(des_diff_w_f[1,0],-diff_w_f_margin),diff_w_f_margin)
            des_delta_1 = gain.q*pqr_err[1,0] # pqr_err[1,0]: q_err
            des_delta_2 = gain.r*pqr_err[2,0] # pqr_err[2,0]: r_err
            
            command.edf_upper = min(max(base_rpm+des_diff_w_f[0,0],min_rpm),max_rpm)
            command.edf_lower = min(max(-base_rpm+des_diff_w_f[1,0],-max_rpm),-min_rpm)
            command.servo_outer = min(max(des_delta_1,-max_gimbal_ang),max_gimbal_ang)
            command.servo_inner = min(max(des_delta_2,-max_gimbal_ang),max_gimbal_ang)
            
            edf_max_pwm = 1800 # pwm for EDFs corresponding to max_rpm
            edf_min_pwm = 1100 # pwm for EDFs corresponding to zero rpm (= min_rpm)
            servo_max_pwm = 1500+50 # pwm for servos corresponding to +max_gimbal_ang
            servo_min_pwm = 1500-50 # pwm for servos corresponding to -max_gimbal_ang

            pi.set_servo_pulsewidth(17, (command.edf_upper-min_rpm)/(max_rpm-min_rpm)*(edf_max_pwm-edf_min_pwm)+edf_min_pwm)
            pi.set_servo_pulsewidth(18, (command.edf_lower-min_rpm)/(max_rpm-min_rpm)*(edf_max_pwm-edf_min_pwm)+edf_min_pwm)
            pi.set_servo_pulsewidth(22, command.servo_outer/max_gimbal_ang*(servo_max_pwm-servo_min_pwm)/2+1500)
            pi.set_servo_pulsewidth(23, command.servo_inner/max_gimbal_ang*(servo_max_pwm-servo_min_pwm)/2+1500)

            # send angle or rpm
            msg = str(round(command.edf_upper,3)) + ',' + str(round(command.edf_lower,3)) + ',' + str(round(command.servo_outer,3)) + ',' + str(round(command.servo_inner,3)) + '\r' + '\n'
            #print('send data: ',msg)
            ser3.write(msg.encode())
            #print(str(time.time()-start_time)+':  command sent.  '+msg)
            

            
        while loop_start+0.01>time.time():
            time.sleep(0.0001)
    
main()