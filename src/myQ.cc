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
    int channelCount = getParentModule()->getParentModule()->getSubmodule("scheduler")->par("channels");
    channelQualities.resize(channelCount);
    int userIndex = getParentModule()->getIndex();
    double baseQuality = 0.5 + 0.5 * (userIndex % 3) / 3.0; // Base quality depends on user class
    
    for (int i = 0; i < channelCount; i++) {
        // Create deterministic variation based on both channel index and user index
        // This ensures different channels have different qualities
        // The sine function creates a smooth variation between channels
        double channelVariation = 0.3 * sin((i * (userIndex + 1)) / 5.0);
        
        // Ensure quality stays in reasonable range [0.2, 1.0]
        channelQualities[i] = std::max(0.2, std::min(1.0, baseQuality + channelVariation));
        
        EV << "User " << userIndex << " channel " << i << " has quality " << channelQualities[i] << endl;
    }
  
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
        // Get assigned channel indices if provided
        std::vector<int> assignedChannels;
        if (msg->hasPar("assignedChannels")) {
            const char* channelStr = msg->par("assignedChannels").stringValue();
            cStringTokenizer tokenizer(channelStr);
            while (tokenizer.hasMoreTokens()) {
                assignedChannels.push_back(atoi(tokenizer.nextToken()));
            }
        }
        
        EV << "MyQ received " << nrOfRadioBlocks << " blocks from user " << userIndex << endl;
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
            
            // Add channel quality for the assigned channel
            if (!assignedChannels.empty()) {
                int channelIdx = assignedChannels[nrOfRadioBlocks % assignedChannels.size()];
                double quality = channelQualities[channelIdx];
                
                cMsgPar *par = new cMsgPar("channelQuality");
                par->setDoubleValue(quality);
                msg->addPar(par);
            }

            send(msg, "txPackets");
            
            nrOfRadioBlocks--;
        }
    }

    queueLength = queue.getLength();
    cMessage *qInfo = new cMessage("qInfo");
    
    qInfo->addPar("queueLengthInfo");
    qInfo->par("queueLengthInfo").setLongValue(queueLength);

    std::string channelQualitiesStr;
    for (size_t i = 0; i < channelQualities.size(); i++) {
        if (!channelQualitiesStr.empty())
            channelQualitiesStr += ",";
        channelQualitiesStr += std::to_string(channelQualities[i]);
    }
    qInfo->addPar("channelQualities");
    qInfo->par("channelQualities").setStringValue(channelQualitiesStr.c_str());

    // Send queue length info
    send(qInfo, "txInfo");
}
