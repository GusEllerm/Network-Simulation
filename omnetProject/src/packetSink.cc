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
#include "appMessage_m.h"
#include "circBuff.h"
#include <iostream>
#include <fstream>
#include <string>

namespace wsn {

Define_Module(PacketSink);

void PacketSink::initialize()
{
    time_t t = time(0);
    struct tm *now = localtime(& t);

    char date[80];
    strftime(date,80," %Y-%m-%d %H-%M-%S",now);

    rxCount = 0;
    outFileName = par("outFile").str();
    outFileName = outFileName.substr(1, outFileName.size()-3);
    bufferSize = 4096;
    outFile.open("../logs/" + outFileName, std::ios_base::app);

    outFile.seekp(0, std::ios_base::end);
    if (outFile.tellp() == 0) {
        //outFile << "Time_RX,Time_TX,ID,Seqno,Size" << std::endl;
        outFile << "Packets_RX" << std::endl;
    }
}

void PacketSink::handleMessage(cMessage *msg)
{
    if (dynamic_cast<appMessage *>(msg))
    {
        appMessage *appMsg = static_cast<appMessage *>(msg);
        rxCount++;
        //outFile << simTime().dbl() << "," << appMsg->getTimeStamp() << "," << appMsg->getSenderId() << "," << appMsg->getSeqno() << "," << appMsg->getMsgSize() << std::endl;
        delete msg;
    }
}

void PacketSink::finish()
{
    outFile << rxCount << std::endl;
}
}
