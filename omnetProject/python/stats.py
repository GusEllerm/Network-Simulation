import numpy as np
import matplotlib.pyplot as plt

dist = list(range(1,41))
packets_received = []
loss_rate = []
receive_rate = []

packets_transmitted = np.loadtxt("../logs/generator", skiprows=1) - 2

for i in range(1,41):
    data = np.loadtxt("../logs/exp1/exp1_radius_" + str(i), skiprows=1)
    mean = np.mean(data)
    packets_received.append(mean)
    loss_rate.append(1 - (mean/packets_transmitted))
    receive_rate.append((mean/packets_transmitted))

plt.figure()
plt.plot(dist, receive_rate)
plt.title('Channel Packet Receive Rate Simulation')
plt.xlabel('Distance (m)')
plt.ylabel('Receive Rate')
plt.grid(True)

plt.figure()
plt.plot(dist, loss_rate)
plt.title('Channel Packet Loss Rate Simulation')
plt.xlabel('Distance (m)')
plt.ylabel('Loss Rate')
plt.grid(True)

plt.show()
