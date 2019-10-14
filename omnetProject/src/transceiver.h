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

#ifndef __WSN_TRANSCEIVER_H_
#define __WSN_TRANSCEIVER_H_
//#define FSM_DEBUG

#include <omnetpp.h>
#include "signalStart_m.h"
#include "CSRequest_m.h"
#include <list>
#include <fstream>

using namespace omnetpp;

namespace wsn {

/**
 * TODO - Generated class
 */
class Transceiver : public cSimpleModule
{
public:
    ~Transceiver();
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    const double ref = 1.0;
    const double pathLossExponent = 4.0;
    double lossRatio;
    double lossRatioDB;
    double receivedPowerDBm;
    double bitRateDB;
    double snrDB;
    double snr;
    double ber;
    double per;
    double u;

    std::string outFileName;
    std::ofstream outFile;

    int collisions;
    bool collisionCounted;

    // State control
    int transceiverState;
    cFSM transmitFSM;
    enum {
        INIT = 0,
        RECEIVE = FSM_Steady(1),
        TURNAROUNDLOCK = FSM_Steady(2),
        TRANSMIT = FSM_Transient(1),
        CSLOCK = FSM_Steady(3),
        CSTRANSMIT = FSM_Transient(2),
        TRANSMITFINAL = FSM_Steady(4)
    };

    int nodeId;
    double nodeXPos;
    double nodeYPos;

    double txPowerDBm;
    double bitRate;
    double csThreshDBm;
    double noisePowerDBm;
    double turnaroundTime;
    double csTime;

    std::list<signalStart *> currentTransmissions;

    CSRequest *csr = nullptr;
};
}
#endif
