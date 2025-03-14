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
<<<<<<< HEAD
    
    // Initialize delay tracking values
    meanHPDelay = 0.0;
    par("meanHPDelay").setDoubleValue(0.0);  // Create and initialize this parameter
    lastUpdateTime = simTime();
    updateInterval = 0.1;  // 100ms in seconds
}

// In Sink.cc - Update handleMessage() to track HP delays properly
void Sink::handleMessage(cMessage *msg) {
    // Calculate packet lifetime (end-to-end delay)
    simtime_t lifetime = simTime() - msg->getCreationTime();
    
    if (msg->arrivedOn("rxPackets")) {
        // Get user index and check if it's a high priority user
        int userIndex = msg->par("userIndex").longValue();
        int userWeight = msg->par("Weight").longValue();
        
        // Emit lifetime signal for statistics
        emit(lifetimeSignals[userIndex], lifetime);
        
        // Track high priority user delays (users with weight >= 4)
        if (userWeight >= 4) {
            // Update running average with exponential moving average
            double alpha = 0.1;  // Smoothing factor
            double currentHPDelay = lifetime.dbl();
            
            if (meanHPDelay == 0) {
                meanHPDelay = currentHPDelay;
            } else {
                meanHPDelay = (1-alpha) * meanHPDelay + alpha * currentHPDelay;
            }
          
            
            // Update parameter for FLC to access
            par("meanHPDelay").setDoubleValue(meanHPDelay);
            
            EV << "HP packet delay: " << currentHPDelay 
               << "ms, Average: " << meanHPDelay << "ms" << endl;
        }
        
        // Delete the message
        delete msg;
    }
}

=======
    
    // Initialize delay tracking values
    meanHPDelay = 0.0;
    par("meanHPDelay").setDoubleValue(0.0);  // Create and initialize this parameter
    lastUpdateTime = simTime();
    updateInterval = 0.1;  // 100ms in seconds
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    double alpha = 0.7; // EWMA factor
  
    if(msg->arrivedOn("rxPackets"))
    {
        // Extract message parameters
        int userWeight = msg->par("Weight").longValue();
        int userIndex = msg->par("userIndex").longValue();
        
        // Track HP user delays (weight >= 4 indicates HP)
        if (userWeight >= 4) {
            double ms = lifetime.dbl() * 1000;  // Convert to milliseconds
            
            // Use EWMA for smoother delay calculation
            if (meanHPDelay == 0.0) {
                meanHPDelay = ms;
            } else {
                meanHPDelay = alpha * meanHPDelay + (1-alpha) * ms;
            }
            
            // Update parameter periodically
            if (simTime() - lastUpdateTime >= updateInterval) {
                lastUpdateTime = simTime();
                
                // Update the parameter directly - FLC will read this
                par("meanHPDelay").setDoubleValue(meanHPDelay);
                EV << "Updated meanHPDelay parameter to " << meanHPDelay << "ms" << endl;
            }
        }
        
        // Emit the signal for this user's packet lifetime
        if (userIndex >= 0 && userIndex < (int)lifetimeSignals.size()) {
            emit(lifetimeSignals[userIndex], lifetime);
            EV << "Emitted lifetime signal for user " << userIndex 
               << ", weight " << userWeight 
               << ", delay " << lifetime << "s" << endl;
        }
    }
    
    delete msg;
}

>>>>>>> 3558353 (enhance Scheduler and Sink modules with user weight management, update delay tracking, and improve signal handling)
void Sink::finish()
{
    // Empty implementation
}
