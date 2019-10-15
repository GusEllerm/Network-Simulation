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

#ifndef __WSN_PACKETSINK_H_
#define __WSN_PACKETSINK_H_

#include <omnetpp.h>
#include <fstream>
#include "appMessage_m.h"

using namespace omnetpp;

namespace wsn {

/**
 * TODO - Generated class
 */
class PacketSink : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    appMessage *appMsg = nullptr;
    std::string outFileName;
    std::string outFileName2;
    std::ofstream outFile;
    std::ofstream outFile2;
    int bufferSize;
    int rxCount;
};
}

#endif
