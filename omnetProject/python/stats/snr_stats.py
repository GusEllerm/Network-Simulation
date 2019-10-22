import numpy as np
import matplotlib.pyplot as plt
from scipy import special

repeats = 1
snrDB = []
path_losses = []
path_lossesDB = [0.0, 12.041199826559248, 19.084850188786497, 24.082399653118497, 27.958800173440753, 31.126050015345747, 33.80392160057027, 36.123599479677743, 38.169700377572994, 40.0, 41.655707406329007, 43.167249841904997, 44.557734092273471, 45.84512142712952, 47.043650362227254, 48.164799306236993, 49.217956855130957, 50.210900204132244, 51.150144038113154, 52.04119982655925, 52.888771789356774, 53.69690723288825, 54.469113440703715, 55.20844966846424, 55.917600346881507, 56.598933918832721, 57.254550566359498, 57.88632125368877, 58.49591991595824, 59.084850188786497, 59.654467753370909, 60.206015699784373, 60.740553933027847, 61.259169681230205, 61.762707303533546, 62.252110373388028, 62.728066645405043, 63.191352195909609, 63.64258240379327, 64.082399653118486]
snr = []
ber = []
per = []
test = []

dist = list(range(1,41))

packets_transmitted = np.loadtxt("../logs/exp1/generator_exp1", skiprows=1, delimiter=',', usecols=1)
packets_transmitted = np.mean(packets_transmitted) - 2

for i in range(1,41):
    data = np.loadtxt("../logs/exp2/exp2_numRX_" + str(i) + "_collisions", skiprows=1, delimiter=',')
    #path_loss = data[0]
    SNR_total = data[0]
    count = data[1]
    snrDB.append(SNR_total/ count)
    #path_losses.append(path_loss)

for i in path_losses:
    path_lossesDB.append(10 * np.log10(i))

for i in snrDB:
    snr.append(np.power(10, i/10))

for i in snr:
    ber.append(special.erfc(np.sqrt(2 * i)))

for i in ber:
    per.append(1 - (np.power((1 - i), 64 * 8)))


plt.figure()
plt.plot(dist, path_lossesDB)
plt.grid(True)
plt.xlim(1, 40)
plt.title('Path Loss Rate with Increasing Distance')
plt.xlabel('Distance (m)')
plt.ylabel('Path Loss Rate (dB)')

plt.figure()
plt.plot(dist, per)
plt.grid(True)
plt.xlim(1, 40)
plt.ylim(0, 1)
plt.title('Packet Error Rate with Increasing Distance')
plt.xlabel('Distance (m)')
plt.ylabel('Packet Error Rate')

plt.figure()
plt.plot(dist, ber)
plt.grid(True)
plt.xlim(1, 40)
plt.ylim(0, 0.03)
plt.title('Bit Error Rate with Increasing Distance')
plt.xlabel('Distance (m)')
plt.ylabel('Bit Error Rate')

plt.figure()
plt.plot(dist, snrDB)
plt.grid(True)
plt.xlim(1, 40)
plt.title('SNR with Increasing Distance')
plt.xlabel('Distance (m)')
plt.ylabel('SNR (dB)')


plt.show()
