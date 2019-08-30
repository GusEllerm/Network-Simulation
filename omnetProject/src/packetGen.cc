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

Define_Module(PacketGen);

void PacketGen::initialize()
{
    int seqno = 0;
    int messageSize = par("messageSize");
    int txId = getParentModule()->par("nodeId");
    double iatDistribution = par("iatDistribution");

    scheduleAt(simTime() + 0.1, new cMessage("xx")); //sends the initial message!
}

void PacketGen::handleMessage(cMessage *msg)
{
    if (!strncmp(msg->getName(), "xx", 2))
    {
       printf("test");
    }
}
