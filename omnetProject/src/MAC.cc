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
#include <deque>

Define_Module(MAC);

void MAC::initialize()
{
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    buffer.resize(bufferSize);
    buffer.clear();

}

void MAC::handleMessage(cMessage *msg)
{
    if (dynamic_cast<appMessage *>(msg))
    {
        appMessage *appMsg = static_cast<appMessage *>(msg);

        if (buffer.size() != bufferSize)
        {
            buffer.push_front(appMsg);;
        } else {
            EV << "drop @ mac";
            delete appMsg;
        }

        appMessage *outMsg = new appMessage(*buffer.back());
        buffer.pop_back();
        //buffer popping not tested.

        macMessage *mmsg = new macMessage();
        mmsg->encapsulate(outMsg);
        send(mmsg, "out0");

        //TODO encapsulate mmsg in a transmission request packet
    }
}
