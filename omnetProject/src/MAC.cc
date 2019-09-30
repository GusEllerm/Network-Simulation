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
#include "transmissionConfirm_m.h"
#include "CSRequest_m.h"
#include "CSResponse_m.h"
#include "selfMessage_m.h"
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

    firstPass = true;
    MAC_State = 0;

    buffer.clear();
}

void MAC::handleMessage(cMessage *msg)
{
    // buffer control (adding & dropping packets to the buffer)
    if (dynamic_cast<appMessage *>(msg))
    {

        if (buffer.size() < bufferSize)
        {
            appMessage *appMsg = static_cast<appMessage *>(msg);
            buffer.push_front(appMsg);
        } else {

            EV << "Buffer overflow.. packet dropped ";
            EV << buffer.size();
            EV << "/";
            EV << bufferSize;
            EV << "\n";
        }

        // This is here to sync with the init cycle of the transceiver
        if (firstPass) {
            buffer.pop_back();
            transmissionRequest *initPacket = new transmissionRequest;
            send(initPacket, "out0");
            firstPass = false;
        }

    }

    // TX path
    // consumes items from the buffer
    FSM_Switch(MAC_FSM){
        case FSM_Exit(INIT):
            backoffCounter = 0;

            // if there is something to read from the buffer start the state machine to transmit
            if (buffer.size() > 0) {
                EV << "Transmit Attempt.. ";
                EV << "Buffer size.. ";
                EV << buffer.size();
                EV << "/";
                EV << bufferSize;
                EV << "\n";
                curMessage = new appMessage(*buffer.back());
                // Start the process to send curMessage
                FSM_Goto(MAC_FSM, TRANSMITCS);
            }
            break;


        case FSM_Exit(TRANSMITCS):
        {
            // send CSRequest packet to transceiver
            CSRequest *csrmsg = new CSRequest;
            send(csrmsg, "out0");
            FSM_Goto(MAC_FSM, TRANSMITLOCK);
            break;
        }

        case FSM_Enter(TRANSMITLOCK):
            break;
        case FSM_Exit(TRANSMITLOCK):
            // will receive a CSResponse packet
            // busyChannel will be either true or false
            if (dynamic_cast<CSResponse *>(msg)) {
                CSResponse *csmsg = static_cast<CSResponse *>(msg);
                EV << "GOT CS RESPONSE IN MAC \n";
                EV << "Channel status..  busy: ";
                EV << csmsg->getBusyChannel();
                EV << "\n";
                if (csmsg->getBusyChannel()) {

                    if (backoffCounter < maxBackoffs) {
                        backoffCounter += 1;

                        EV << "Channel busy.. backoffCounter: ";
                        EV << backoffCounter;
                        EV << "/";
                        EV << maxBackoffs;
                        EV << "\n";

                        SelfMessage *smsg = new SelfMessage();
                        smsg->setDescription("Wait for backoffDistribution");

                        scheduleAt(simTime() + backoffDistribution, smsg);
                        FSM_Goto(MAC_FSM, TRANSMITLOCK);

                    } else {
                        // packet is dropped
                        buffer.pop_back();
                        delete curMessage;
                        EV << "Packet has reached max backoffs - dropping packet & restarting \n";
                        FSM_Goto(MAC_FSM, INIT);
                    }
                } else {
                    // Channel is clear
                    FSM_Goto(MAC_FSM, TRANSMITMSG);
                }
            }

            else if (dynamic_cast<transmissionConfirm *>(msg)) {
                buffer.pop_back();
                delete curMessage;
                EV << "Transmission Successful.. Buffer Size: ";
                EV << buffer.size();
                EV << "\n";
                FSM_Goto(MAC_FSM, INIT);
            }

            else if (dynamic_cast<SelfMessage *>(msg)) {
                FSM_Goto(MAC_FSM, TRANSMITCS);
            }
            break;

        case FSM_Exit(TRANSMITMSG):
        {
            EV << "TRANSMIT MESSAGE \n";

            appMessage *appMsg = new appMessage(*curMessage);
            macMessage *mmsg = new macMessage();
            transmissionRequest *requestMsg = new transmissionRequest;

            mmsg->encapsulate(appMsg);
            requestMsg->encapsulate(mmsg);

            send(requestMsg, "out0");

            FSM_Goto(MAC_FSM, TRANSMITLOCK);

            break;
        }
    }

    // RX path
    if (dynamic_cast<transmissionIndication *>(msg))
    {
        transmissionIndication *tiMsg = static_cast<transmissionIndication *>(msg);
        macMessage *mMsg = static_cast<macMessage *>(tiMsg->decapsulate());
        appMessage *appMsg = static_cast<appMessage *>(mMsg->decapsulate());

        send(appMsg, "out1");
        delete tiMsg;
        delete mMsg;
    }

}
