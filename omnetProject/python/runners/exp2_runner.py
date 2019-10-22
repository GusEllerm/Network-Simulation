import subprocess
import os
import time

configs = []

for i in range(2, 21):
    if (i % 2 == 0):
        configs.append("../src/omnetProject -n .:../src/ned -f exp2_numTX_" + str(i) + ".ini -u Cmdenv")

#subprocess.call(["python3", "clean_up.py"])
os.chdir("../")
os.chdir("../")
os.chdir("./simulations")

timeStart = time.time()

for i in configs:
     subprocess.call([i], shell=True)

#subprocess.call("./omnetProject -n .:../src -f exp2_numTX_6.ini -u Cmdenv", shell=True)

timeEnd = time.time()
totalTime = timeEnd - timeStart
print(totalTime)
