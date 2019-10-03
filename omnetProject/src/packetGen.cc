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
#include "appMessage_m.h"
#include "CSRequest_m.h"

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
    time_t t = time(0);
    struct tm *now = localtime(& t);

    char date[80];
    strftime(date,80," %Y-%m-%d %H-%M-%S",now);

    outFileGenerator.open("../logs/generator");
    outFileGenerator << "TX_Count" << std::endl;

    txCount = 0;
    seqno = 0;
    txId = getParentModule()->par("nodeId");
    messageSize = par("messageSize");
    distro = par("iatDistribution");

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

        message->setTimeStamp(simTime());
        message->setSenderId(txId);
        message->setSeqno(seqno);
        message->setMsgSize(messageSize);

        seqno++;

        send(message, "out0");
        txCount++;
        scheduleAt((distro + simTime()), msg); //callback function to keep sending app messages.
    }
}

//appMessage* PacketGen::createMessage()
//{
////    char name[80];
////    sprintf(name, "TX ID: %d, Seqno: %d, Time: %f", txId, seqno, simTime().dbl());
////
////    message = new appMessage(name);
////
////    message->setTimeStamp(simTime());
////    message->setSenderId(txId);
////    message->setSeqno(seqno);
////    message->setMsgSize(messageSize);
////
////    seqno++;
////
////    return message;
//}

void PacketGen::finish()
{
    outFileGenerator << txCount << std::endl;
}
}
