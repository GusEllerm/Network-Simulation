import numpy as np
import matplotlib.pyplot as plt


for q in range (1, 21):
    i = 0
    j = 0
    repeats = 10
    numTX = range(1, 21)
    packets_received = []
    packets_sent = []
    buffer_overflows = []
    buffer_timeouts = []

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
