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

#include "transceiver.h"
#include "appMessage_m.h"
#include "transmissionRequest_m.h"
#include "transmissionConfirm_m.h"
#include "transmissionIndication_m.h"
#include "macMessage_m.h"
#include "signalStart_m.h"
#include "signalStop_m.h"
#include "selfMessage_m.h"
#include "CSRequest_m.h"
#include "CSResponse_m.h"
#include <list>
#include <stdlib.h>
#include <ctgmath>
#include <math.h>
#include <string.h>


Define_Module(Transceiver);
#define FSM_DEBUG

void Transceiver::initialize()
{
    txPowerDBm = par("txPowerDBm");
    bitRate = par("bitRate");
    csThreshDBm = par("csThreshDBm");
    noisePowerDBm = par("noisePowerDBm");
    turnaroundTime = par("turnaroundTime");
    csTime = par("csTime");

    transceiverState = 0; //RX

    nodeId = getParentModule()->par("nodeId");
    nodeXPos = getParentModule()->par("nodeXPos");
    nodeYPos = getParentModule()->par("nodeYPos");

}

void Transceiver::handleMessage(cMessage *msg)
{
    // The receive path (receive path gets run always therefore is not included in the state machine)
    if (dynamic_cast<signalStart *>(msg))
    {
        signalStart *startMsg = static_cast<signalStart *>(msg);

        // Collision check
        if (!currentTransmissions.empty())
        {
            startMsg->setCollidedFlag(true);
            for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
            {
                (*it)->setCollidedFlag(true);
            }
        }

        // Check if a node has more than one on going transmission, if so - end the program
        for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
        {
            if (startMsg->getId() == (*it)->getId())
            {
                EV << "Aborting in transceiver -- node has more than one active transmission";
                exit(EXIT_FAILURE);
            }
        }

        currentTransmissions.push_front(new signalStart(*startMsg)); //deep copy
        delete msg;
        return;
    }

    if (dynamic_cast<signalStop *>(msg))
      {
          signalStop *stopMsg = static_cast<signalStop *>(msg);
          signalStart *startMsg;

          // Check for corresponding start message
          bool found = false;
          for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
          {
              if (stopMsg->getId() == (*it)->getId())
              {
                  startMsg = *it;
                  it = currentTransmissions.erase(it); //to avoid demolishing the memory we just pointed to.
                  found = true;
              }
          }
          if (!found) //we got a stop signal for a transmission that never existed!
          {
              EV << "Aborting in transceiver";
              exit(EXIT_FAILURE);
          }

          delete stopMsg;

          if (startMsg->getCollidedFlag())
          {
              //TODO how do we drop a collided message?
          }
          else
          {
              macMessage *mmsg = static_cast<macMessage *>(startMsg->decapsulate());

              double msgtransmitPowerDBm = startMsg->getTransmitPowerDBm();
              double msgXPos = startMsg->getPosX();
              double msgYPos = startMsg->getPosY();

              double xDist = (nodeXPos - msgXPos) * (nodeXPos - msgXPos);
              double yDist = (nodeYPos - msgYPos) * (nodeYPos - msgYPos);

              double dist = sqrt(xDist + yDist);

              if (dist <= ref)
              {
                  lossRatio = ref;
              }
              else
              {
                  lossRatio = pow(dist, pathLossExponent);
              }

              lossRatioDB = 10 * log10(lossRatio);
              receivedPowerDBm = msgtransmitPowerDBm - lossRatioDB;
              bitRateDB = 10 * log10(bitRate);
              snrDB = (receivedPowerDBm) - (noisePowerDBm + bitRateDB);
              snr = pow(10, snrDB/10);
              ber = erfc(sqrt(2 * snr));

              int packetLength = static_cast<appMessage *>(mmsg->getEncapsulatedPacket())->getMsgSize();
              packetLength = packetLength * 8;

              per = 1 - (pow((1 - ber), packetLength));

              u = uniform(0, 1);

              if (u < per)
              {
                  delete mmsg;
              }
              else
              {
                  transmissionIndication *indicationMsg = new transmissionIndication();
                  indicationMsg->encapsulate(mmsg);
                  send(indicationMsg, "out0");
              }
          }


         EV << "Path Loss (delta): ";
         EV << lossRatioDB;
         EV << "||";
         EV << "Received Power DBm (R): ";
         EV << receivedPowerDBm;
         EV << "||";
         EV << "SNR DB (E): ";
         EV << snrDB;
         EV << "||";
         EV << "Bit Error Rate (p): ";
         EV << ber;
         EV << "||";
         EV << "Packet Error Rate: ";
         EV << per;
         EV << "||";

         delete startMsg;
         return;
      }


    FSM_Switch(transmitFSM)
    {
        case FSM_Exit(INIT):
            FSM_Goto(transmitFSM, RECEIVE);
            break;

        case FSM_Enter(RECEIVE):
                // Transmission Confirm received from the TRANSMIT state
                if (dynamic_cast<transmissionConfirm *>(msg)) {
                    EV << "Status ok sent to MAC\n";
                    send(msg, "out0"); // Sends the transmission confirm to the MAC layer
                } else if (dynamic_cast<SelfMessage *>(msg)) {
                    delete msg;
                }
            break;

        case FSM_Exit(RECEIVE):

            // transmission Request from MAC
            if (dynamic_cast<transmissionRequest *>(msg)) {
                // message is deleted after stream {RECEIVE -> TURNAROUNDLOCK -> TRANSMIT}
                transmissionRequest *tmsg = static_cast<transmissionRequest *>(msg);

                // Creating a self message for differentiating between new transmissionRequests and current
                // That way we can send statusBusy if we receive another request while transmitting
                SelfMessage *smsg = new SelfMessage();
                smsg->setDescription("Current Stream");
                smsg->encapsulate(tmsg);
                scheduleAt(simTime() + turnaroundTime, smsg);
                FSM_Goto(transmitFSM, TURNAROUNDLOCK);
            }

            // CS request from MAC
            else if (dynamic_cast<CSRequest *>(msg)) {
                csr = static_cast<CSRequest *>(msg);
                delete csr;

                EV << "CS Request received\n";
                double totalPower = 0;
                double totalPowerDBm = 0;

                 for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
                 {
                     double msgtransmitPowerDBm = (*it)->getTransmitPowerDBm();
                     double msgXPos = (*it)->getPosX();
                     double msgYPos = (*it)->getPosY();

                     double xDist = (nodeXPos - msgXPos) * (nodeXPos - msgXPos);
                     double yDist = (nodeYPos - msgYPos) * (nodeYPos - msgYPos);

                     double dist = sqrt(xDist + yDist);

                     if (dist <= ref)
                     {
                         lossRatio = ref;
                     }
                     else
                     {
                         lossRatio = pow(dist, pathLossExponent);
                     }

                     lossRatioDB = 10 * log10(lossRatio);
                     receivedPowerDBm = msgtransmitPowerDBm - lossRatioDB;
                     double receivedPower = pow(10, receivedPowerDBm / 10);
                     totalPower += receivedPower;
                 }

                totalPowerDBm = 10 * log10(totalPower);

                SelfMessage *smsg = new SelfMessage();
                smsg->setTotalPower(totalPowerDBm);
                scheduleAt(simTime() + csTime, smsg);

                FSM_Goto(transmitFSM, CSLOCK);
            }

            break;

        case FSM_Enter(TURNAROUNDLOCK):
            break;
        case FSM_Exit(TURNAROUNDLOCK):
            if (dynamic_cast<SelfMessage *>(msg))
            {
                FSM_Goto(transmitFSM, TRANSMIT); // message has waited for the turnaround time, start transmit
            } else if (dynamic_cast<transmissionConfirm *>(msg))
            {
                FSM_Goto(transmitFSM, RECEIVE);
            }
            break;

        case FSM_Exit(TRANSMIT):
            if (dynamic_cast<SelfMessage *>(msg)){
                SelfMessage *smsg = static_cast<SelfMessage *>(msg);
                transmissionRequest *tcmsg = static_cast<transmissionRequest *>(smsg->decapsulate());
                macMessage *mmsg = static_cast<macMessage *>(tcmsg->decapsulate());

                delete smsg;
                delete tcmsg;

                signalStart *startMsg = new signalStart();

                int packetLength = static_cast<appMessage *>(mmsg->getEncapsulatedPacket())->getMsgSize();
                packetLength = packetLength * 8; //convert to bits

                startMsg->setId(nodeId);
                startMsg->setPosX(nodeXPos);
                startMsg->setPosY(nodeYPos);
                startMsg->setTransmitPowerDBm(txPowerDBm);
                startMsg->setCollidedFlag(false);

                startMsg->encapsulate(mmsg);

                send(startMsg, "out1"); //to channel

                SelfMessage *selfPacket = new SelfMessage();
                selfPacket->setDescription("End Transmission");
                scheduleAt(simTime() + (packetLength / bitRate), selfPacket);
                FSM_Goto(transmitFSM, TRANSMITFINAL);
            }


            // TransmissionRequest while dealing with a previous TransmissionRequest
            else if (dynamic_cast<transmissionRequest *>(msg))
            {
                transmissionConfirm *tcmsg = new transmissionConfirm();
                tcmsg->setStatus("statusBusy");
                send(tcmsg, "out1");
                delete msg;
            }

            else if (dynamic_cast<CSRequest *>(msg))
            {
                CSResponse *csResponse = new CSResponse();
                csResponse->setBusyChannel(true);
                send(csResponse, "out0");
                delete msg;
            }

            break;

        case FSM_Exit(TRANSMITFINAL):
            if (dynamic_cast<SelfMessage *>(msg)) {

                signalStop *stopMsg = new signalStop();
                int id = getParentModule()->par("nodeId");
                stopMsg->setId(id);
                send(stopMsg, "out1"); //to channel
                delete msg;

                transmissionConfirm *tcmsg = new transmissionConfirm();
                tcmsg->setStatus("statusOK");
                scheduleAt(simTime() + turnaroundTime, tcmsg);

                FSM_Goto(transmitFSM, TURNAROUNDLOCK);
            }

        case FSM_Enter(CSLOCK):
            break;
        case FSM_Exit(CSLOCK):
            if (msg->isSelfMessage()) FSM_Goto(transmitFSM, CSTRANSMIT);
            break;

        case FSM_Exit(CSTRANSMIT):
            EV << "SENDING CS RESPONSE\n";

            SelfMessage *smsg = static_cast<SelfMessage *>(msg);
            CSResponse *csResponse = new CSResponse();

            double totalPowerDBm = smsg->getTotalPower();

            if (totalPowerDBm > csThreshDBm)
            {
                csResponse->setBusyChannel(true);
            } else
            {
                csResponse->setBusyChannel(false);
            }

            send(csResponse, "out0");
            FSM_Goto(transmitFSM, RECEIVE);
            break;
    }
}
