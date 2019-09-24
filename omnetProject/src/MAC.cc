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


//        appMessage *outMsg = new appMessage(*buffer.back());
//        //buffer.pop_back();
//
//        macMessage *mmsg = new macMessage();
//        mmsg->encapsulate(outMsg);
//
//        transmissionRequest *requestMsg = new transmissionRequest;
//        requestMsg->encapsulate(mmsg);
//
//        send(requestMsg, "out0");

    }


    // State machine requires something else to place an appmessage into the buffer
    // It will wait till something is in the buffer before commencing
    FSM_Switch(MAC_FSM){
        case FSM_Exit(INIT):
            backoffCounter = 0;

            // This is here to sync with the init cycle of the transceiver
            if (firstPass) {
                transmissionRequest *initPacket = new transmissionRequest;
                send(initPacket, "out0");
                firstPass = false;
            }

            // if there is something to read from the buffer start the state machine to transmit
            else if (buffer.size() > 0) {
                EV << "Somthing in the buffer to send \n";
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
            // TODO
            break;
        case FSM_Exit(TRANSMITLOCK):
            // will recieve a CSResponse packet
            // busyChannel will be either true or false
            if (dynamic_cast<CSResponse *>(msg)) {
                EV << "GOT CS RESPONSE IN MAC";
                CSResponse *csmsg = static_cast<CSResponse *>(msg);
                if (csmsg->getBusyChannel()) {
                    // Channel is busy
                    EV << "Channel is busy \n";
                    EV << csmsg->getBusyChannel();
                    EV << "\n";

                    if (backoffCounter <= maxBackoffs) {
                        backoffCounter += 1;
                        FSM_Goto(MAC_FSM, TRANSMITCS);
                    } else {
                        // packet is dropped
                        buffer.pop_back();
                        FSM_Goto(MAC_FSM, INIT);
                    }
                } else {
                    // Channel is clear
                    EV << "Channel is clear \n";
                    FSM_Goto(MAC_FSM, TRANSMITMSG);
                }
            }
            break;

        case FSM_Exit(TRANSMITMSG):
            EV << "TRANSMIT MESSAGE \n";
            FSM_Goto(MAC_FSM, TRANSMITLOCK); // this is just here to stop the compiler complaning about unfinished code
            // TODO
            break;

        case FSM_Exit(TRANSMITFAIL):
            // TODO
            break;
    }

}
