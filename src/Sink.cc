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
    user6WeightSignal = registerSignal("user6Weight");
    user7WeightSignal = registerSignal("user7Weight");
    // Initialize delay tracking values
    meanHPDelay = 0.0;
    maxHPDelay = 0.0; 
    sumHPDelay = 0.0;
    numHPPackets = 0;
    par("meanHPDelay").setDoubleValue(0.0);  // Create and initialize this parameter


}

// In Sink.cc - Update handleMessage() to track HP delays properly
void Sink::handleMessage(cMessage *msg) {
    // Calculate packet lifetime (end-to-end delay)
    simtime_t lifetime = simTime() - msg->getCreationTime(); // Lifetime in seconds

    
    if (msg->arrivedOn("rxPackets")) {
        // Get user index and check if it's a high priority user
        int userIndex = msg->par("userIndex").longValue();
        int userWeight = msg->par("Weight").longValue();
        
        // Emit lifetime signal for statistics
        emit(lifetimeSignals[userIndex], lifetime);
        
        // Track high priority user delays (users with weight >= 4)
        if (userIndex == 6 || userIndex == 7) {
            // Update running average with exponential moving average
            if (userIndex == 6) {
                emit(user6WeightSignal, userWeight);
            } else if (userIndex == 7) {
                emit(user7WeightSignal, userWeight);
            }
            double alpha = 0.5;  // Smoothing factor
            double currentHPDelay = lifetime.dbl();
            maxHPDelay = std::max(maxHPDelay, currentHPDelay);
          
            sumHPDelay = sumHPDelay + currentHPDelay; 
            numHPPackets++;
            if (meanHPDelay == 0) {
                meanHPDelay = currentHPDelay;
            } else {
                meanHPDelay = alpha * meanHPDelay + (1 - alpha) * currentHPDelay;
            }
          
            EV << "Current HP delay: " << currentHPDelay 
               << "s, Max HP delay: " << maxHPDelay << "s" << endl;
            // Update parameter for FLC to access
            par("meanHPDelay").setDoubleValue(meanHPDelay);
            
        }
        
        // Delete the message
        delete msg;
    }
}

void Sink::finish()
{
    EV << "Simulation finished" << endl;
    EV << "Mean HP delay: " << meanHPDelay << "s" << endl;
    EV << "Max HP delay: " << maxHPDelay << "s" << endl;
    EV << "Total HP packets: " << numHPPackets << endl;
    EV << "Total HP delay: " << sumHPDelay/numHPPackets << "s" << endl;

}
