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

#include "packetGen.h"
#include "../messages/appMessage_m.h"
#include "../messages/CSRequest_m.h"

namespace wsn {
    Define_Module(PacketGen);

    PacketGen::~PacketGen()
    {
        if (message != nullptr)
        {
            delete message;
        }
    }

    void PacketGen::initialize()
    {
        outFileName = par("outFile").str();
        txId = getParentModule()->par("nodeId");
        messageSize = par("messageSize");
        distro = par("iatDistribution");

        outFileName = outFileName.substr(1, outFileName.size() - 2);
        outFile.open("../logs/" + outFileName, std::ios_base::app);

        outFile.seekp(0, std::ios_base::end);
        if (outFile.tellp() == 0) {
            outFile << "Node_ID,TX_Count,X,Y" << std::endl;
        }

        txCount = 0;
        seqno = 0;

        scheduleAt(simTime(), new cMessage); //sends the initial message!
    }

    void PacketGen::handleMessage(cMessage *msg)
    {
        if (dynamic_cast<appMessage *>(msg))
        {
            delete msg;
        }
        else
        {
            if (message != nullptr)
            {
                delete message;
            }

            char name[80];
            sprintf(name, "TX ID: %d, Seqno: %d, Time: %f", txId, seqno, simTime().dbl());

            message = new appMessage(name);
            txCount++;

            message->setTimeStamp(simTime());
            message->setSenderId(txId);
            message->setSeqno(seqno);
            message->setMsgSize(messageSize);

            seqno++;

            send(message, "out0");

            scheduleAt((exponential(distro) + simTime()), msg); //callback function to keep sending app messages.
        }
    }

    void PacketGen::finish()
    {
        double xPos = getParentModule()->par("nodeXPos");
        double yPos = getParentModule()->par("nodeYPos");
        outFile << txId << "," << txCount << "," << xPos << "," << yPos << std::endl;
    }
}
