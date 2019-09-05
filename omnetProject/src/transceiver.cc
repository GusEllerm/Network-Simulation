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
#include <list>
#include <stdlib.h>
#include <ctgmath>
#include <math.h>


Define_Module(Transceiver);

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
    //RECEIVING
    if (dynamic_cast<signalStart *>(msg))
    {
        signalStart *startMsg = static_cast<signalStart *>(msg);

        if (!currentTransmissions.empty())
        {
            startMsg->setCollidedFlag(true);
            for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
            {
                (*it)->setCollidedFlag(true);
            }
        }

        for (auto it = currentTransmissions.begin(); it != currentTransmissions.end(); ++it)
        {
            if (startMsg->getId() == (*it)->getId())
            {
                EV << "Aborting in transceiver";
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

        if (!found)
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

            if (dist < ref)
            {
                lossRatio = 1.0;
            }
            else
            {
                lossRatio = pow(dist, 4);
            }

            lossRatioDB = 10 * log10(lossRatio);
            receivedPowerDBm = txPowerDBm - lossRatioDB;
            bitRateDB = 10 * log10(bitRate);
            snrDB = (receivedPowerDBm) - (noisePowerDBm + bitRateDB);
            snr = pow(10, snrDB/10);
            ber = erfc(sqrt(2 * snr));

            EV << " ";
            EV << snrDB;
            EV << " ";
            EV << snr;
            EV << " ";
            EV << ber;
            EV << " ";

            int packetLength = static_cast<appMessage *>(mmsg->getEncapsulatedPacket())->getMsgSize();
            packetLength = packetLength * 8;

            per = 1 - pow((1 - ber), packetLength);

            EV << per;

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

       delete startMsg;
       return;
    }

    // TRANMITTING
    if (dynamic_cast<transmissionRequest *>(msg))
    {
        if (transceiverState) //transmitting
        {
            delete msg;
            transmissionConfirm *confirmMsg = new transmissionConfirm;
            confirmMsg->setStatus("statusBusy");
            send(confirmMsg, "out0"); //to MAC
        }

        if (!transceiverState)
        {
            transmissionRequest *requestMsg = static_cast<transmissionRequest *>(msg);
            macMessage *mmsg = static_cast<macMessage *>(requestMsg->decapsulate());

            delete requestMsg;

            transceiverState = 1;

            scheduleAt(simTime() + turnaroundTime, mmsg);
        }
    }

    else if (dynamic_cast<macMessage *>(msg))
    {
        if (transceiverState)
        {
            macMessage *mmsg = static_cast<macMessage *>(msg);
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

            scheduleAt(simTime() + packetLength / bitRate, new cMessage("test"));
        }
    }

    else if (!strcmp(msg->getName(), "test"))
    {
        delete msg;

        signalStop *stopMsg = new signalStop();
        int id = getParentModule()->par("nodeId");
        stopMsg->setId(id);
        send(stopMsg, "out1"); //to channel
        scheduleAt(simTime() + turnaroundTime, new cMessage("test2"));
    }

    else if (!strcmp(msg->getName(), "test2"))
    {
        delete msg;
        transceiverState = 0;
        transmissionConfirm *confirmMessage = new transmissionConfirm();
        confirmMessage->setStatus("statusOK");
        send(confirmMessage, "out0"); //to MAC

    }

    else
    {
        EV << "something a bit odd in trans";
        delete msg;
    }
}
