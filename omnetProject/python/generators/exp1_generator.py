import configparser

config = configparser.ConfigParser()
config.optionxform = str #allows for capital letters

for i in range(1, 41):
    config['General'] = {'network': 'simulation',
                         'cpu-time-limit': '60s',
                         'debug-on-errors': 'true',
                         'sim-time-limit': '1000s',
                         'seed-set': '${repetition}',
                         'repeat': '1',
                         'simulation.numTX': '1',
                         'simulation.radius': i,
                         'simulation.TXNode[*].packGen.messageSize': '64',
                         'simulation.TXNode[*].packGen.iatDistribution': '0.01',
                         'simulation.RXNode.packSink.outFile': '"./exp1/exp1_radius_' + str(i) + 'm"',
                         'simulation.RXNode.packSink.outFile2': '"./exp2/exp2_numTX_' + str(i) + '_ReceivedPackets"',
                         'simulation.TXNode[*].packGen.outFile': '"./exp1/generator_exp1"',
                         'simulation.TXNode[*].MAC.outFile': '"./exp2/mac_' + str(i) + '_TX"',
                         'simulation.RXNode.MAC.outFile': '"./exp2/mac_' + str(i) + '_RX"',
                         'simulation.TXNode[*].MAC.outFile': '"./exp1/IGNORE"',
                         'simulation.RXNode.MAC.outFile': '"./exp1/IGNORE"',
                         'simulation.TXNode[*].trans.txPowerDBm': '0',
                         'simulation.TXNode[*].trans.bitRate': '250000',
                         'simulation.TXNode[*].trans.csThreshDBm': '-50',
                         'simulation.TXNode[*].trans.noisePowerDBm': '-120',
                         'simulation.TXNode[*].trans.outFile': '"./exp2/exp2_numTX_' + str(i) + '_collisions"',
                         'simulation.RXNode.trans.outFile': '"./exp2/exp2_numRX_' + str(i) + '_collisions"',
                         'simulation.TXNode[*].trans.turnaroundTime': '0.001',
                         'simulation.TXNode[*].trans.csTime': '0.000125',
                         'simulation.RXNode.trans.txPowerDBm': '0',
                         'simulation.RXNode.trans.bitRate': '250000',
                         'simulation.RXNode.trans.csThreshDBm': '-50',
                         'simulation.RXNode.trans.noisePowerDBm': '-120',
                         'simulation.RXNode.trans.turnaroundTime': '0.001',
                         'simulation.RXNode.trans.csTime': '0.000125',
                         'simulation.TXNode[*].MAC.backoffDistribution': 'exponential(0.003)',
                         'simulation.TXNode[*].MAC.bufferSize': '5',
                         'simulation.TXNode[*].MAC.maxBackoffs': '5',
                         'simulation.RXNode.MAC.backoffDistribution': 'exponential(0.003)',
                         'simulation.RXNode.MAC.bufferSize': '5',
                         'simulation.RXNode.MAC.maxBackoffs': '5'}

    with open('../simulations/exp1_radius_' + str(i) + 'm.ini', 'w') as configfile:
        config.write(configfile)
