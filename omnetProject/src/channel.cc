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

#include "channel.h"
#include "appMessage_m.h"
#include "signalStart_m.h"
#include "signalStop_m.h"


Define_Module(Channel);

void Channel::initialize()
{
    numGates = gateCount();
}

void Channel::handleMessage(cMessage *msg)
{
    if (dynamic_cast<signalStart *>(msg))
    {
        signalStart *startMsg = static_cast<signalStart *>(msg);

        for (int idx = 0; idx < numGates; idx++)
        {
            signalStart *sendMsg = new signalStart(*startMsg); //deep copy
            send(sendMsg, "out", idx);
        }

        delete msg;
    }

    else if (dynamic_cast<signalStop *>(msg))
    {
        signalStop *stopMsg = static_cast<signalStop *>(msg);

        for (int idx = 0; idx < numGates; idx++)
        {
            signalStop *sendMsg = new signalStop(*stopMsg); //deep copy
            send(sendMsg, "out", idx);
        }

        delete msg;
    }

    else {
        EV << "something strange got to channel";
        delete msg;
    }
}
