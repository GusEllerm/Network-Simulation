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

#include "MAC.h"
#include "appMessage_m.h"
#include "macMessage_m.h"
#include "transmissionRequest_m.h"
#include "transmissionIndication_m.h"
#include "CSRequest_m.h"
#include "CSResponse_m.h"
#include <list>

#include <stdlib.h>
#include <string.h>

Define_Module(MAC);
#define FSM_DEBUG

void MAC::initialize()
{
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    MAC_State = 0;

    buffer.clear();
}

void MAC::handleMessage(cMessage *msg)
{

    // buffer control (adding & dropping packets to the buffer)
    if (dynamic_cast<appMessage *>(msg))
    {
        appMessage *appMsg = static_cast<appMessage *>(msg);

        if (buffer.size() < bufferSize)
        {
            buffer.push_front(appMsg);
        } else {
            EV << "drop @ mac";
            delete appMsg;
        }

        appMessage *outMsg = new appMessage(*buffer.back());
        buffer.pop_back();

        macMessage *mmsg = new macMessage();
        mmsg->encapsulate(outMsg);

        transmissionRequest *requestMsg = new transmissionRequest;
        requestMsg->encapsulate(mmsg);

        send(requestMsg, "out0");

    }

    // RX path
    else if (dynamic_cast<transmissionIndication *>(msg))
    {
        transmissionIndication *tiMsg = static_cast<transmissionIndication *>(msg);
        macMessage *mMsg = static_cast<macMessage *>(tiMsg->decapsulate());
        appMessage *appMsg = static_cast<appMessage *>(mMsg->decapsulate());
        send(appMsg, "out1");
    }
}
