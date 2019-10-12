import numpy as np
import csv
import matplotlib.pyplot as plt


def successful_packets_per_node(num_nodes):
    master_dict = {}
    final_dict = {}
    total_successful_packets = 0
    repeat = 0

    # Produce intermediate dictionary's
    for j in range(0, 20):
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
        final_dict[node] = final_dict[node] / 20
        total_successful_packets += final_dict[node]

    for node in final_dict.keys():
        print("Node " + str(node) + " accounts for " +
              "{0:.2f}".format((final_dict[node] / total_successful_packets) * 100) + "% of successful packets")

    print("\n")
    # print(master_dict)
    # print(final_dict)
    # print(total_successful_packets)

def generated_packets_per_node():
    pass


def main():
    for num_nodes in range(2, 22, 2):
        # total_generated_packets, gen_pkts_dict = generated_packets_per_node(num_nodes)
        successful_packets_per_node(num_nodes)



if __name__ == "__main__":
    main()