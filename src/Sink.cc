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

#include "Sink.h"
#include <numeric>
#include <fstream>
Define_Module(Sink);

void Sink::initialize()
{
    int numUsers = par("gateSize").intValue();
    EV << "Number of users: " << numUsers << endl;
    for (int i = 0; i < numUsers; i++) {
        std::string signalName = "lifetime:user" + std::to_string(i);
        lifetimeSignals.push_back(registerSignal(signalName.c_str()));
        EV << "Registered signal: " << signalName << endl;
    }
    meanHPdelay = 0;

}


void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
  
    if(msg->arrivedOn("rxPackets"))
    {
        // Extract message parameters
        int userPriority = msg->par("userPriorityType").longValue();
        int userWeight = msg->par("Weight").longValue();
        int userIndex = msg->par("userIndex").longValue();
        
        // Get channel quality if available
        double channelQuality = 1.0;
        if (msg->hasPar("channelQuality")) {
            channelQuality = msg->par("channelQuality").doubleValue();
        }
        
        // Emit the signal for this user's packet lifetime
        if (userIndex >= 0 && userIndex < (int)lifetimeSignals.size()) {
            emit(lifetimeSignals[userIndex], lifetime);
            EV << "Emitted lifetime signal for user " << userIndex 
               << ", priority " << userPriority 
               << ", weight " << userWeight 
               << ", delay " << lifetime << "s" << endl;
        }
    }
     
    
      delete msg;
}
void Sink::finish()
{

}
