import numpy as np
import matplotlib.pyplot as plt

test = []
test2 = []
test3 = []
numTXX = []
drop_rates = []
overflows = []
timeouts = []
total_sent_after_mac = []
total_received = []

for i in range(1, 21):
    if (i % 2 == 0):
        numTXX.append(i)

for q in range (1,21):
    i = 0
    j = 0
    total_mac_loss = 0
    repeats = 20
    packets_received = []
    packets_sent = []
    buffer_overflows = []
    buffer_timeouts = []
    loss_rates = []
    total_mac_loss = []
    sent_on_chan = []
    lost_on_chan = []
    lost_on_mac = []
    channel_loss = []
    mac_loss = []


    if (q % 2 == 0):

        rx_data = np.loadtxt("../logs/exp2/exp2_numTX_" + str(q), skiprows=1, delimiter=',', usecols=0)
        tx_data = np.loadtxt("../logs/exp2/generator_" + str(q) + "_TX", skiprows=1, delimiter=',')
        mac_data = np.loadtxt("../logs/exp2/mac_" + str(q) + "_TX", skiprows=1, delimiter=',')

        node_ids = tx_data[:, 0]
        tx_packets = tx_data[:, 1]

        buffer_overflow_data = mac_data[:, 1]
        buffer_timeout_data = mac_data[:, 2]

        num_nodes = np.max(node_ids)

        for data in rx_data:
            packets_received.append(data)

        while (j < repeats * (int(num_nodes) + 1)):
            total_tx = 0
            total_overflows = 0
            total_timeouts = 0

            for k in range(0, int(num_nodes) + 1):
                total_tx += tx_packets[j + k]
                total_overflows += buffer_overflow_data[j + k]
                total_timeouts += buffer_timeout_data[j + k]

            packets_sent.append(total_tx)
            buffer_overflows.append(total_overflows)
            buffer_timeouts.append(total_timeouts)

            j += (int(num_nodes) + 1)

        for i in range (0, repeats):
            loss_rate = (packets_received[i] / packets_sent[i] * 100)
            loss_rates.append(loss_rate)
            total_mac_loss.append(buffer_overflows[i] + buffer_timeouts[i])

            sent = packets_sent[i] - total_mac_loss[i]
            sent_on_chan.append(sent)

        for i in range(0, repeats):
            lost = sent_on_chan[i] - packets_received[i]
            lost_on_chan.append(lost)

            loss = (lost_on_chan[i] / packets_sent[i] * 100)
            channel_loss.append(loss)

            lost_mac = total_mac_loss[i] - packets_received[i]
            lost_on_mac.append(lost_mac)

            loss_mac = (lost_on_mac[i] / packets_sent[i] * 100)
            mac_loss.append(loss_mac)

        drop_rates.append((100 - (np.mean(loss_rates))) / 100)
        overflows.append(np.mean(buffer_overflows))
        timeouts.append(np.mean(buffer_timeouts))

        test.append(((np.mean(loss_rates) + np.mean(channel_loss))) / 100)
        test2.append(((np.mean(loss_rates) + np.mean(mac_loss))) / 100)

        total_sent_after_mac.append(np.floor(np.mean(sent_on_chan)))
        total_received.append(np.floor(np.mean(packets_received)))

plt.figure()
plt.plot(numTXX, test)
plt.hold(True)
plt.plot(numTXX, test2)
plt.title('Ratio Lost in MAC vs. Channel')
plt.xlabel('Number of Transmitters')
plt.ylabel('Loss Rate')
plt.legend(['Channel Loss', 'MAC Loss'])
plt.grid(True)
plt.xlim(2, 20)
plt.ylim(0, 1)

plt.figure()
plt.plot(numTXX, drop_rates)
plt.title('Total Loss Rate')
plt.xlabel('Number of Transmitters')
plt.ylabel('Loss Rate')
plt.grid(True)
plt.xlim(2, 20)
plt.ylim(0, 1)

plt.figure()
plt.plot(numTXX, overflows)
plt.title('Buffer Overflows')
plt.xlabel('Number of Transmitters')
plt.ylabel('Number of Lost Packets')
plt.grid(True)
plt.xlim(2, 20)

plt.figure()
plt.plot(numTXX, total_received)
plt.title('Buffer Overflows')
plt.xlabel('Number of Transmitters')
plt.ylabel('Number of Lost Packets')
plt.grid(True)
plt.xlim(2, 20)


plt.show()


#print((np.mean(loss_rates) + np.mean(channel_loss)) + (np.mean(loss_rates) + np.mean(mac_loss)))
