import configparser

config = configparser.ConfigParser()
config.optionxform = str #allows for capital letters

for i in range(1, 21):
    if i % 2 == 0:
        config['General'] = {'network': 'simulation',
                             'cpu-time-limit': '60s',
                             'debug-on-errors': 'true',
                             'sim-tim-limit': '1000s',
                             'simulation.numTX': i,
                             'simulation.radius': '20',
                             'simulation.TXNode[*].packGen.messageSize': '64',
                             'simulation.TXNode[*].packGen.iatDistribution': 'exponential(0.02)',
                             'simulation.RXNode.packSink.outFile': '"exp2_numTX_' + str(i) + '"',
                             'simulation.TXNode[*].trans.txPowerDBm': '0',
                             'simulation.TXNode[*].trans.bitRate': '250000',
                             'simulation.TXNode[*].trans.csThreshDBm': '-50',
                             'simulation.TXNode[*].trans.noisePowerDBm': '-120',
                             'simulation.TXNode[*].trans.turnaroundTime': '0.001',
                             'simulation.TXNode[*].trans.csTime': '0.000125',
                             'simulation.RXNode.trans.txPowerDBm': '0',
                             'simulation.RXNode.trans.bitRate': '250000',
                             'simulation.RXNode.trans.csThreshDBm': '-50',
                             'simulation.RXNode.trans.noisePowerDBm': '-120',
                             'simulation.RXNode.trans.turnaroundTime': '0.001',
                             'simulation.RXNode.trans.csTime': '0.000125',
                             'simulation.TXNode[*].MAC.backoffDistribution': 'exponential(0.03)',
                             'simulation.TXNode[*].MAC.bufferSize': '5',
                             'simulation.TXNode[*].MAC.maxBackoffs': '5',
                             'simulation.RXNode.MAC.backoffDistribution': 'exponential(0.03)',
                             'simulation.RXNode.MAC.bufferSize': '5',
                             'simulation.RXNode.MAC.maxBackoffs': '5'}

        with open('./experiment2/exp2_numTX_' + str(i) + '.ini', 'w') as configfile:
            config.write(configfile)
