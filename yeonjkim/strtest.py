transmitted_str = "-172.12,-79.86,74.4,0.a,0.0,0.0,-0.986,-0.022,-0.175"
splited_str = transmitted_str.split(',')
Roll = float(splited_str[0])
Pitch = float(splited_str[1])
Yaw = float(splited_str[2])
Gyro_X = float(splited_str[3])
Gyro_Y = float(splited_str[4])
Gyro_Z = float(splited_str[5])
X_acc = float(splited_str[6])
Y_acc = float(splited_str[7])
Z_acc = float(splited_str[8])
"""
roll: -172.12 pitch : -79.86 yaw : 74.4
gyro_x : 0.0 gyro_y : 0.0 gyro_z : 0.0
x_acc : -0.986 y_acc : -0.022 z_acc : -0.175
"""
print("Roll : {0} Pitch : {1} Yaw : {2}".format(Roll, Pitch, Yaw))
print("Gyro_X : {0} Gyro_Y : {1} Gyro_Z : {2}".format(Gyro_X, Gyro_Y, Gyro_Z))
print("X_acc : {0} Y_acc : {1} Z_acc : {2}".format(X_acc, Y_acc, Z_acc))
print("\n")

#print("farr1 : {0} farr2 : {1}".format(farr1, farr2))