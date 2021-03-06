import numpy as np
import csv
import matplotlib.pyplot as plt

# Control console output from here
# Note that for diagram creation all must be true

percent_success = True
percent_success_verbose = True

mac_loss = True
mac_loss_verbose = True

channel_loss = True
channel_loss_verbose = True

num_iterations = 10


def successful_packets_per_node(num_nodes):
    print(num_nodes)
    master_dict = {}
    final_dict = {}
    results_dict = {}
    total_successful_packets = 0
    repeat = 0

    # Produce intermediate dictionary's
    for j in range(0, num_iterations):
        master_dict[j] = {}

    with open("../logs/exp2/exp2_numTX_" + str(num_nodes) + "_ReceivedPackets") as packets:
        csv_reader = csv.DictReader(packets)
        line_count = 0

        for line in csv_reader:
            if line_count == 0:
                # print(f'keys are are {", ".join(line)}')
                line_count += 1
            else:

                if line["SenderID"] == "=":
                    repeat += 1
                elif line["SenderID"] not in master_dict[repeat].keys():
                    # Haven't seen the node before, init
                    master_dict[repeat][line["SenderID"]] = 0
                    final_dict[line["SenderID"]] = 0
                else:
                    master_dict[repeat][line["SenderID"]] += 1

    for dict in master_dict.keys():
        for node in master_dict[dict].keys():
            final_dict[node] += master_dict[dict][node]

    for node in final_dict.keys():
        # Get the average successful packets from each node for one iteration
        final_dict[node] = final_dict[node] / num_iterations
        total_successful_packets += final_dict[node]

    for node in range(0, num_nodes):
        results_dict[node] = (final_dict[str(node)] / total_successful_packets) * 100
        print("Node " + str(node) + " accounts for " +
              "{0:.2f}".format((final_dict[str(node)] / total_successful_packets) * 100) + "% of successful packets")

    print("")
    return results_dict



def loss_stats(num_nodes):
    # pi = per iteration
    sunk_packets_pi_dict = {}
    gen_packets_pi_dict = {}
    loss_mac_pi_dict = {}
    collision_pi_dict = {}

    # Lists are here to be used for graphing
    success_percent_pi_list = []
    mac_loss_percent_pi_list = []
    collision_percent_pi_list = []
    mac_loss_buffer_overflow_list = []
    mac_loss_timeout_list = []

    sink_packets_path = "../logs/exp2/exp2_numTX_" + str(num_nodes)
    gen_packets_path = "../logs/exp2/generator_" + str(num_nodes) + "_TX"
    mac_loss_path = "../logs/exp2/mac_" + str(num_nodes) + "_TX"
    collision_loss_path = "../logs/exp2/exp2_numRX_" + str(num_nodes) + "_collisions"

    with open(sink_packets_path) as recieved_packets, \
            open(gen_packets_path) as generated_packets, \
            open(mac_loss_path) as packets_lost_mac, \
            open(collision_loss_path) as collision_loss:

        sunk_packets_reader = csv.reader(recieved_packets)
        next(sunk_packets_reader, None)  # Skip the header
        gen_packets_reader = csv.DictReader(generated_packets)
        mac_loss_reader = csv.DictReader(packets_lost_mac)
        collision_loss_reader = csv.DictReader(collision_loss)

        # Grab data and configure into dictionary's
        for j in range(0, num_iterations):
            collision_pi_dict[j] = 0
            sunk_packets_pi_dict[j] = 0
            gen_packets_pi_dict[j] = 0
            loss_mac_pi_dict[j] = {"Overflow": 0, "Timeout": 0}

        current_iteration = 0
        for row in gen_packets_reader:
            gen_packets_pi_dict[current_iteration] += int(row['TX_Count'])
            if int(row['Node_ID']) == num_nodes - 1:
                current_iteration += 1

        current_iteration = 0
        for row in mac_loss_reader:
            loss_mac_pi_dict[current_iteration]["Overflow"] += int(row["Overflowed"])
            loss_mac_pi_dict[current_iteration]["Timeout"] += int(row["Timed_Out"])
            if int(row['Node_ID']) == num_nodes - 1:
                current_iteration += 1

        current_iteration = 0
        for row in sunk_packets_reader:
            sunk_packets_pi_dict[current_iteration] = int(row[0])
            current_iteration += 1

        current_iteration = 0
        for row in collision_loss_reader:
            collision_pi_dict[current_iteration] = int(row["Collisions"])
            current_iteration += 1

        if percent_success:
            for iteration in range(0, num_iterations):
                success_percent = (sunk_packets_pi_dict[iteration] / int(gen_packets_pi_dict[iteration])) * 100
                print("Percent of sunk packets for iteration", iteration, ": " + "{:.2f}".format(success_percent) + "%")
                success_percent_pi_list.append(success_percent)

                if percent_success_verbose:
                    print("Percentage Breakdown")
                    print("Sent packets: " + str(gen_packets_pi_dict[iteration]) +
                          " Received packets: " + str(sunk_packets_pi_dict[iteration]))
                    print("")

        # Get loss rate due to mac pi
        if mac_loss:
            for iteration in range(0, num_iterations):
                accountable_percent = (
                        ((loss_mac_pi_dict[iteration]["Overflow"] + loss_mac_pi_dict[iteration]["Timeout"]) /
                         (int(gen_packets_pi_dict[iteration]) - sunk_packets_pi_dict[iteration])) * 100)
                print("MAC accounts for: " + "{:.2f}".format(accountable_percent) + "% of total packet drops")
                mac_loss_percent_pi_list.append(accountable_percent)

                if mac_loss_verbose:
                    print("Percentage Breakdown")
                    print("Total packet loss: " + "{}".format(
                        int(gen_packets_pi_dict[iteration]) - sunk_packets_pi_dict[iteration]))
                    print("Buffer loss :" + "{}".format(loss_mac_pi_dict[iteration]["Overflow"]))
                    print("Timeout loss :" + "{}".format(loss_mac_pi_dict[iteration]["Timeout"]))
                    print("")
                    mac_loss_buffer_overflow_list.append(
                        (loss_mac_pi_dict[iteration]["Overflow"] / (
                            int(gen_packets_pi_dict[iteration] - sunk_packets_pi_dict[iteration]))) * 100
                    )
                    mac_loss_timeout_list.append(
                        (loss_mac_pi_dict[iteration]["Timeout"] / (
                            int(gen_packets_pi_dict[iteration] - sunk_packets_pi_dict[iteration])) * 100
                        )
                    )

        # Get loss rate due to channel
        if channel_loss:
            for iteration in range(0, num_iterations):
                accountable_percent = (
                        (collision_pi_dict[iteration] /
                         (int(gen_packets_pi_dict[iteration]) - sunk_packets_pi_dict[iteration])) * 100)
                print("Collisions account for: " + "{:.2f}".format(accountable_percent) + "% of total packet drops")
                collision_percent_pi_list.append(accountable_percent)

                if channel_loss_verbose:
                    print("Percentage Breakdown")
                    print("Total packet loss: " + "{}".format(
                        int(gen_packets_pi_dict[iteration]) - sunk_packets_pi_dict[iteration]))
                    print("Collision loss :" + "{}".format(collision_pi_dict[iteration]))
                    print("")

    return success_percent_pi_list, \
           mac_loss_percent_pi_list, \
           collision_percent_pi_list, \
           mac_loss_buffer_overflow_list, \
           mac_loss_timeout_list

def create_graphs(simulation_stats):

    packet_loss_means = []
    packet_success_means = []
    packet_loss_mac = []
    packet_loss_collision = []
    packet_loss_buffer = []
    packet_loss_timeout = []
    nodes_v_successful_packets = []
    total_collision = []

    # Explained packet loss (packet_loss_mac + packet_loss_collision)
    # Checks to see if we have accounted for all packet loss in the system
    packet_loss_explainable = []

    for simulation in simulation_stats.keys():
        packet_loss_means.append(
            100 - np.mean(simulation_stats[simulation]["Overall_successful_transmissions"])
        )
        packet_success_means.append(
            np.mean(simulation_stats[simulation]["Overall_successful_transmissions"])
        )
        packet_loss_mac.append(
            np.mean(simulation_stats[simulation]["Loss_due_to_MAC"])
        )
        packet_loss_collision.append(
            np.mean(simulation_stats[simulation]["Loss_due_to_Collisions"])
        )
        packet_loss_buffer.append(
            np.mean(simulation_stats[simulation]["Loss_due_to_buffer_overflow"])
        )
        packet_loss_timeout.append(
            np.mean(simulation_stats[simulation]["Loss_due_to_timeout"])
        )
        nodes_v_successful_packets.append(
            list(simulation_stats[simulation]["node_v_successful_packets"].values())
        )

    print(nodes_v_successful_packets)
    for j in range(0, len(simulation_stats.keys())):
        packet_loss_explainable.append(
            (((packet_loss_mac[j] + packet_loss_collision[j]) / 100) * (packet_loss_means[j] / 100)) * 100
        )


    # print(packet_loss_means)
    # print(packet_loss_explainable)

    print(packet_loss_means)
    print(packet_loss_mac)
    for i in range(0, len(packet_loss_means)):
        total_collision.append(packet_loss_means[i] - packet_loss_mac[i])


    plt.figure()
    plt.plot(list(simulation_stats.keys()), total_collision)
    plt.title("Collision Packet Loss")
    plt.ylabel("Total Packets (%)")
    plt.xlabel("Number of Transmitters")
    plt.xlim(2, 20)
    plt.ylim(0, 100)
    plt.show()


    plt.figure()
    plt.plot(list(simulation_stats.keys()), packet_loss_means)
    plt.title('Total Packets Lost')
    plt.xlabel("Number of Transmitters")
    plt.ylabel("Total Packets (%)")
    plt.grid(True)
    plt.xlim(2, 20)
    plt.ylim(0, 100)
    plt.show()

    plt.figure()
    plt.plot(list(simulation_stats.keys()), packet_loss_mac)
    plt.plot(list(simulation_stats.keys()), packet_loss_collision)
    plt.title("Packet loss - Mac vs Collision")
    plt.legend(["MAC packet loss", "Collision packet loss"])
    plt.xlabel("Number of Transmitters")
    plt.ylabel("Total packets (%)")
    plt.grid(True)
    plt.xlim(2, 20)
    plt.ylim(0, 100)
    plt.show()

    plt.figure()
    plt.plot(list(simulation_stats.keys()), packet_loss_mac, linestyle=':')
    plt.plot(list(simulation_stats.keys()), packet_loss_buffer)
    plt.plot(list(simulation_stats.keys()), packet_loss_timeout)
    plt.title("MAC Packet Loss - Buffer Overflow vs Timeout")
    plt.legend(["Total MAC packet loss", "Buffer overflow", "Timeout"])
    plt.xlabel("Number of Transmitters")
    plt.ylabel("Total packets (%)")
    plt.grid(True)
    plt.xlim(2, 20)
    plt.ylim(0, 3)
    plt.show()

    for i in range(0, len(packet_loss_timeout)):
        packet_loss_timeout[i] = packet_loss_timeout[i] * 100

    plt.figure()
    plt.plot(list(simulation_stats.keys()), packet_loss_timeout, linestyle=':')
    plt.title("Timeout per Transmitter")
    plt.legend(["Packets dropped due to timeout"])
    plt.xlabel("Number of Transmitters")
    plt.ylabel("Packets Timeout (%)")
    plt.grid(True)
    plt.xlim(2, 20)
    plt.ylim(0, 100)
    plt.show()


def main():
    simulation_stats = {}

    for simulation in range(2, 22, 2):
        simulation_stats[simulation] = {"Overall_successful_transmissions": [],
                                        "Loss_due_to_MAC": [],
                                        "Loss_due_to_Collisions": [],
                                        "Loss_due_to_buffer_overflow": [],
                                        "Loss_due_to_timeout": [],
                                        "node_v_successful_packets": {}}

    for simulation in range(2, 22, 2):
        overall, mac, collision, buffer, timeout = loss_stats(simulation)
        succsess_per_node = successful_packets_per_node(simulation)
        simulation_stats[simulation]["node_v_successful_packets"] = succsess_per_node
        simulation_stats[simulation]["Overall_successful_transmissions"] = overall
        simulation_stats[simulation]["Loss_due_to_MAC"] = mac
        simulation_stats[simulation]["Loss_due_to_Collisions"] = collision
        simulation_stats[simulation]["Loss_due_to_buffer_overflow"] = buffer
        simulation_stats[simulation]["Loss_due_to_timeout"] = timeout

    create_graphs(simulation_stats)

if __name__ == "__main__":
    main()
