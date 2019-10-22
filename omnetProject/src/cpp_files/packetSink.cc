//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "packetSink.h"
#include "../messages/appMessage_m.h"
#include "circBuff.h"
#include <iostream>
#include <fstream>
#include <string>

namespace wsn {

    Define_Module(PacketSink);

    void PacketSink::initialize()
    {
        rxCount = 0;

        outFileName = par("outFile").str();
        outFileName = outFileName.substr(1, outFileName.size() - 2);
        outFile.open("../logs/" + outFileName, std::ios_base::app);

        outFileName2 = par("outFile2").str();
        outFileName2 = outFileName2.substr(1, outFileName2.size() - 2);
        outFile2.open("../logs/" + outFileName2, std::ios_base::app);

        outFile.seekp(0, std::ios_base::end);
        if (outFile.tellp() == 0) {
            outFile << "Packets_RX" << std::endl;
        }

        outFile2.seekp(0, std::ios_base::end);
        if (outFile2.tellp() == 0) {
            outFile2 << "SenderID,SequenceNum" << std::endl;
        }
    }

    void PacketSink::handleMessage(cMessage *msg)
    {
        if (dynamic_cast<appMessage *>(msg))
        {
            appMsg = static_cast<appMessage *>(msg);

            outFile2 << appMsg->getSenderId() << "," << appMsg->getSeqno() << std::endl;

            rxCount++;
            delete msg;
        }
    }

    void PacketSink::finish()
    {
        outFile << rxCount << std::endl;
        outFile2 << "=,=" << std::endl;
    }
}
