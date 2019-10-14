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

namespace wsn {
Define_Module(MAC);
#define FSM_DEBUG

MAC::~MAC()
{
    if (curMessage != nullptr)
    {
        delete curMessage;
    }

    if (initPacket != nullptr)
    {
        delete initPacket;
    }

    if (confirm != nullptr)
    {
        delete confirm;
    }

    if (csMsg != nullptr)
    {
        delete csMsg;
    }

    while (!buffer.empty())
    {
        appMessage *appMsgDelete = buffer.back();
        delete appMsgDelete;
        buffer.pop_back();
    }
}

void MAC::initialize()
{
    txId = getParentModule()->par("nodeId");
    bufferSize = par("bufferSize");
    maxBackoffs = par("maxBackoffs");
    backoffDistribution = par("backoffDistribution");

    outFileName = par("outFile").str();
    outFileName = outFileName.substr(1, outFileName.size() - 2);
    outFile.open("../logs/" + outFileName, std::ios_base::app);

    outFile.seekp(0, std::ios_base::end);
    if (outFile.tellp() == 0) {
        outFile << "Node_ID,Overflowed,Timed_Out" << std::endl;
    }

    firstPass = true;
    MAC_State = 0;
    timedOut = 0;
    bufferDropped = 0;

    buffer.clear();
}

void MAC::handleMessage(cMessage *msg)
{
    // buffer control (adding & dropping packets to the buffer)
    if (dynamic_cast<appMessage *>(msg))
    {
        appMsg = static_cast<appMessage *>(msg);

        if (buffer.size() < bufferSize)
        {

            buffer.push_front(new appMessage(*appMsg));
        }
        else {
            bufferDropped++;
            EV << "Buffer overflow.. packet dropped ";
            EV << buffer.size();
            EV << "/";
            EV << bufferSize;
            EV << "\n";
            return;
        }

        // This is here to sync with the init cycle of the transceiver
        if (firstPass)
        {
            appMessage *test = buffer.back();
            delete test;
            buffer.pop_back();
            initPacket = new transmissionRequest;
            send(initPacket, "out0");
            firstPass = false;
        }
    }

    if (csMsg != nullptr)
    {
        delete csMsg;
        csMsg = nullptr;
    }

    // TX path
    // consumes items from the buffer
    FSM_Switch(MAC_FSM){
        case FSM_Exit(INIT):
            //delete csMsg;

            if (confirm != nullptr)
            {
                delete confirm;
                confirm = nullptr;
            }

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
            csrmsg = new CSRequest;
            EV << "HASOUDHASODHUOADHOASHDUSDO";
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
                csMsg = static_cast<CSResponse *>(msg);
                EV << "GOT CS RESPONSE IN MAC \n";
                EV << "Channel status..  busy: ";
                EV << csMsg->getBusyChannel();
                EV << "\n";
                if (csMsg->getBusyChannel()) {

                    if (backoffCounter < maxBackoffs) {
                        backoffCounter += 1;

                        EV << "Channel busy.. backoffCounter: ";
                        EV << backoffCounter;
                        EV << "/";
                        EV << maxBackoffs;
                        EV << "\n";

                        smsg = new SelfMessage();
                        smsg->setDescription("Wait for backoffDistribution");
                        scheduleAt(simTime() + backoffDistribution, smsg);
                        FSM_Goto(MAC_FSM, TRANSMITLOCK);

                    } else {
                        timedOut++;
                        delete smsg;
                        // packet is dropped
                        appMessage *test = buffer.back();
                        delete test;
                        buffer.pop_back();

                        delete curMessage;
                        curMessage = nullptr;
                        EV << "Packet has reached max backoffs - dropping packet & restarting \n";
                        FSM_Goto(MAC_FSM, INIT);
                    }
                } else {
                    // Channel is clear
                    FSM_Goto(MAC_FSM, TRANSMITMSG);
                }

                //cancelAndDelete(smsg);
            }

            else if (dynamic_cast<transmissionConfirm *>(msg)) {
                confirm = static_cast<transmissionConfirm *>(msg);

                appMessage *test = buffer.back();
                delete test;
                buffer.pop_back();

                delete curMessage;
                curMessage = nullptr;
                EV << "Transmission Successful.. Buffer Size: ";
                EV << buffer.size();
                EV << "\n";
                FSM_Goto(MAC_FSM, INIT);
            }

            else if (dynamic_cast<SelfMessage *>(msg)) {
                EV << "GOTR INTO THE ELSE IF";
                FSM_Goto(MAC_FSM, TRANSMITCS);
            }
            break;

        case FSM_Exit(TRANSMITMSG):
        {
            EV << "TRANSMIT MESSAGE \n";
            appMsgEncap = new appMessage(*curMessage);
            mmsg = new macMessage();
            requestMsg = new transmissionRequest();

            mmsg->encapsulate(appMsgEncap);
            requestMsg->encapsulate(mmsg);

            send(requestMsg, "out0");

            FSM_Goto(MAC_FSM, TRANSMITLOCK);
            break;
        }
    }

    // RX path
    if (dynamic_cast<transmissionIndication *>(msg))
    {
        tiMsg = static_cast<transmissionIndication *>(msg);
        macMessage *macMsg = static_cast<macMessage *>(tiMsg->decapsulate());
        appMessage *appMsgSend = static_cast<appMessage *>(macMsg->decapsulate());

        send(appMsgSend, "out1");

        delete macMsg;
        delete tiMsg;
    }
}
void MAC::finish()
{
    outFile << txId << "," << bufferDropped << "," << timedOut << std::endl;
}
}
