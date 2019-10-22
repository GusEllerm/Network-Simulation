import subprocess
import os
import time

configs = []

for i in range(1, 41):
    configs.append("../src/omnetProject -n .:../src/ned -f exp1_radius_" + str(i) + "m.ini -u Cmdenv")

#subprocess.call(["python3", "clean_up.py"])
os.chdir("../")
os.chdir("../")
os.chdir("./simulations")

timeStart = time.time()

for i in configs:
    subprocess.call([i], shell=True)

timeEnd = time.time()
totalTime = timeEnd - timeStart
print(totalTime)
