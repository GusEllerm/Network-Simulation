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

#ifndef __WSN_MAC_H_
#define __WSN_MAC_H_
#define FSM_DEBUG

#include <omnetpp.h>
#include "appMessage_m.h"
#include "macMessage_m.h"
#include "transmissionIndication_m.h"
#include "transmissionRequest_m.h"
#include "transmissionConfirm_m.h"
#include "CSRequest_m.h"
#include "selfMessage_m.h"
#include "CSResponse_m.h"
#include <list>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class MAC : public cSimpleModule
{
  public:
    ~MAC();
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    int MAC_State;
    int backoffCounter;
    int bufferSize;
    int maxBackoffs;
    double backoffDistribution;
    bool firstPass = false;

    // State control

    cFSM MAC_FSM;
    enum {
        INIT = 0,
        TRANSMITCS = FSM_Transient(1),
        TRANSMITLOCK = FSM_Steady(1),
        TRANSMITMSG = FSM_Transient(2)
    };

    std::list<appMessage *> buffer;

    appMessage *curMessage = NULL;
    appMessage *appMsg = NULL;
    appMessage *appMsgEncap;
    macMessage *mmsg;
    transmissionIndication *tiMsg = NULL;
    transmissionRequest *initPacket = NULL;
    transmissionRequest *requestMsg = NULL;
    CSRequest *csrmsg = NULL;
    CSResponse *csMsg = NULL;
    transmissionConfirm *confirm = NULL;
    SelfMessage *smsg;
};

#endif
