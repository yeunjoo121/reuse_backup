import time
import os
try:
    while (True):
        print(".")
        time.sleep(1)
    
except KeyboardInterrupt:
    os.system('sudo poweroff')