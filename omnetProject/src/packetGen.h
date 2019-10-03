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

#ifndef __WSN_PACKETGEN_H_
#define __WSN_PACKETGEN_H_

#include <omnetpp.h>
#include "appMessage_m.h"
#include <fstream>

using namespace omnetpp;

namespace wsn {

/**
 * TODO - Generated class
 */
class PacketGen : public cSimpleModule
{
public:
    ~PacketGen();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    //appMessage* createMessage();

    double distro;
    int messageSize;
    int seqno;
    int txId;
    int txCount;

    appMessage *message = nullptr;
    appMessage *outMessage = nullptr;
    appMessage *appMsg = nullptr;

    std::ofstream outFileGenerator;
};
}

#endif
