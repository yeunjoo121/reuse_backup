import numpy as np

def quat_inv(quat):
    return np.concatenate([quat[0:+1,0:+1],-quat[1:3+1,0:+1]])/np.linalg.norm(quat)

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

quat = np.array([[1], [2], [3], [4]])
dcm_n2b = quat2dcm(quat).T
quat = quat_inv(quat)
del_quat = np.array([[9], [8], [7], [6]])
print(del_quat)
print(quat)
print(dcm_n2b)

del_quat_b = dcm_n2b@del_quat[1:3+1,0:0+1]
print(del_quat[1:3+1,0:0+1])
print(del_quat_b)