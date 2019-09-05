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
#include "transmissionRequest_m.h"
#include "transmissionConfirm_m.h"
#include "macMessage_m.h"
#include "signalStart_m.h"
#include "signalStop_m.h"


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
    if (dynamic_cast<signalStart *>(msg))
    {

    }

    else if (dynamic_cast<signalStop *>(msg))
    {

    }


    if (dynamic_cast<transmissionRequest *>(msg))
    {
        if (transceiverState) //transmitting
        {
            transmissionConfirm *confirmMsg = new transmissionConfirm;
            confirmMsg->setStatus("statusBusy");
            send(confirmMsg, "out1");

            //transmissionRequest *requestMsg = static_cast<transmissionRequest *>(msg);
            //macMessage *mmsg = static_cast<macMessage *>(requestMsg->decapsulate());
            //delete requestMsg;
        }

    }

}
