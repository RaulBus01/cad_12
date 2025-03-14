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

#include "Source.h"

Define_Module(Source);

Source::Source()
{
    sendMessageEvent = nullptr;
}

Source::~Source()
{
    cancelAndDelete(sendMessageEvent);
}

void Source::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    
    // Calculate sending time once during initialization
    int nrUsers = par("usersCount").intValue();
    int nrPackets = par("packetsPerUser").intValue();
    int PacketLength = 1;
    double schedulingCycle = 1.0;
    double netwload = par("networkLoad").doubleValue();
    int nrChannels = par("channels").intValue();
    
    // Output debug information only once
    EV << "Users: " << nrUsers << " Packets: " << nrPackets 
       << " Packet Length: " << PacketLength 
       << " Scheduling Cycle: " << schedulingCycle 
       << " Network Load: " << netwload 
       << " Channels: " << nrChannels << endl;
    
    double sendingTime = (nrUsers * nrPackets * PacketLength * schedulingCycle) / (netwload * nrChannels);
    
    // Output the calculated sending time for debugging
    EV << "Calculated Sending Time: " << sendingTime << " ms" << endl;
    
    // Store sending time as a class member or parameter for use in handleMessage
    sendingInterval = sendingTime;
    
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == sendMessageEvent);
    
    int nrPackets = par("packetsPerUser").intValue();
    for(int i = 0; i < nrPackets; i++) {
        cMessage* job = new cMessage("job");
        send(job, "txPackets");
    }
    
    if(simTime() >= par("MAX_Sim").doubleValue()) {
        endSimulation();
    }
    
    // Use the pre-calculated sending time
    scheduleAt(simTime() + exponential(sendingInterval), sendMessageEvent);
}
