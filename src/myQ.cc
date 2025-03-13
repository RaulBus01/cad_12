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

#include "myQ.h"

Define_Module(MyQ);

void MyQ::initialize()
{
    queue.setName("queue");
    radioLinkQuality = getParentModule()->par("radioLinkQuality").doubleValue();
  
}

void MyQ::handleMessage(cMessage *msg)
{
    int queueLength;


    if (msg->arrivedOn("rxPackets")) {
        queue.insert(msg);
    } else if (msg->arrivedOn("rxScheduling")) {
        // Read parameters from msg
        int nrOfRadioBlocks = (int)msg->par("nrBlocks");
        int userPriorityType = (int)msg->par("userPriorityType");
        int userWeight = (int)msg->par("Weight");
        int userIndex = (int)msg->par("userIndex");
        EV << "MYQ received" << nrOfRadioBlocks << " blocks to user " << userIndex << endl;
        delete msg;

        // Empty the queue
        while (!queue.isEmpty() && nrOfRadioBlocks > 0) {
            msg = (cMessage *)queue.pop();
            cMsgPar *par = new cMsgPar("userPriorityType");
            par->setLongValue(userPriorityType);
            msg->addPar(par);
            
            par = new cMsgPar("userIndex");
            par->setLongValue(userIndex);
            msg->addPar(par);

            par = new cMsgPar("Weight");
            par->setLongValue(userWeight);
            msg->addPar(par);
            
            par = new cMsgPar("radioLinkQuality");
            par->setDoubleValue(radioLinkQuality);
            msg->addPar(par);
            


            send(msg, "txPackets");
            
            nrOfRadioBlocks--;
        }
    }

    queueLength = queue.getLength();
    cMessage *qInfo = new cMessage("qInfo");
    
    qInfo->addPar("queueLengthInfo");
    qInfo->par("queueLengthInfo").setLongValue(queueLength);
    qInfo->addPar("radioLinkQuality");
    qInfo->par("radioLinkQuality").setDoubleValue(radioLinkQuality);

    // Send queue length info
    send(qInfo, "txInfo");
}
