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

Define_Module(PacketGen);


void PacketGen::initialize()
{
    seqno = 0;
    txId = getParentModule()->par("nodeId");
    messageSize = par("messageSize");
    distro = par("iatDistribution");

    scheduleAt(simTime(), new cMessage); //sends the initial message!
}

void PacketGen::handleMessage(cMessage *msg)
{
    appMessage *message = createMessage();
    send(message, "out0");
    scheduleAt((distro + simTime()), msg); //callback function to keep sending app messages.
}

appMessage* PacketGen::createMessage()
{
    char name[80];
    sprintf(name, "TX ID: %d, Seqno: %d, Time: %f", txId, seqno, simTime().dbl());

    appMessage* message = new appMessage(name);

    message->setTimeStamp(simTime());
    message->setSenderId(txId);
    message->setSeqno(seqno);
    message->setMsgSize(messageSize);

    seqno++;

    return message;
}
