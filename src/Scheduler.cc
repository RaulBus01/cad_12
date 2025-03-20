//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 

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

#include "Scheduler.h"
#include <algorithm>

Define_Module(Scheduler);



Scheduler::Scheduler()
{
    selfMsg = nullptr;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


void Scheduler::initialize()
{
    NrUsers = par("usersCount");
    NrOfChannels = par("channels");
    selfMsg = new cMessage("selfMsg");
    for(int user = 0; user < NrUsers; ++user)
    {
        if (user < 3) {
            users.push_back((User(user, par("W_LP").intValue())));
        } else if (user < 6) {
            users.push_back((User(user, par("W_MP").intValue())));
        } else {
            users.push_back((User(user, par("W_HP").intValue())));
        }
    }

    scheduleAt(simTime(), selfMsg);
}

void Scheduler::handleMessage(cMessage *msg) {
    // Handle weight updates from FLC
    if (msg->arrivedOn("inFLC")) {
        if (msg->hasPar("newHPWeight")) {
            int newWeight = msg->par("newHPWeight").longValue();
            EV << "Scheduler received new HP weight: " << newWeight << endl;
            
            // Update HP user weights
            for (int i = 6; i < NrUsers; i++) {
                users[i].setUserWeight(newWeight);
            }
        }
        delete msg;
        return;
    }
    
    for(int i = 0; i < NrUsers; i++) {
        if (msg->arrivedOn("rxInfo", i)) {
            // Update queue length
            if (msg->hasPar("queueLengthInfo")) {
                int queueLen = msg->par("queueLengthInfo").longValue();
                users[i].setQueueLength(queueLen);
                EV << "Scheduler: Updated queue length for user " << i 
                   << " to " << queueLen << endl;
            }
        
            delete msg;
            return;
        }
    }

     if (msg == selfMsg) {
        double currentSimTime = simTime().dbl();
        int remainingChannels = NrOfChannels;

        // Create priority-ordered vector of users
        std::vector<std::pair<double, int>> priorityQueue; // {priority, userIndex}
        for(int i = 0; i < users.size(); i++) {
            double priority = users[i].getUserPriority(currentSimTime);
            priorityQueue.push_back({priority, i});
        }

        // Sort by priority (highest first)
        std::sort(priorityQueue.begin(), priorityQueue.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });

        // Create channel allocation map
        std::vector<int> channelAllocations(NrOfChannels, -1); // -1 means unallocated

        // For each user in priority order
        for(const auto& [priority, userIndex] : priorityQueue) {
            if(remainingChannels <= 0) break;
            
            int userQueueLength = std::max(0, users[userIndex].getQueueLength());
            if (userQueueLength <= 0) continue;
            

            int blocksToAllocate = std::min(userQueueLength, remainingChannels);
            if (blocksToAllocate > 0) {
                std::string assignedChannelStr;
                int channelsAssigned = 0;
                
                // Simple sequential channel allocation
                for (int c = 0; c < NrOfChannels && channelsAssigned < blocksToAllocate; c++) {
                    if (channelAllocations[c] < 0) { // If channel is free
                        channelAllocations[c] = userIndex;
                        
                        if (!assignedChannelStr.empty())
                            assignedChannelStr += ",";
                        assignedChannelStr += std::to_string(c);
                        channelsAssigned++;
                    }
                }
                
                // Create and send command message
                cMessage *cmd = new cMessage("cmd");
                cmd->addPar("nrBlocks");
                cmd->par("nrBlocks").setLongValue(blocksToAllocate);

                cmd->addPar("Weight");
                cmd->par("Weight").setLongValue(users[userIndex].getUserWeight());

                cmd->addPar("userPriorityType");
                cmd->par("userPriorityType").setLongValue(users[userIndex].getUserPriority(currentSimTime));

                cmd->addPar("userIndex");
                cmd->par("userIndex").setLongValue(userIndex);



                cmd->addPar("assignedChannels");
                cmd->par("assignedChannels").setStringValue(assignedChannelStr.c_str());
                
                users[userIndex].updateLastTimeServed(currentSimTime);
                users[userIndex].decrementQueueLength(blocksToAllocate);
                remainingChannels -= blocksToAllocate;
                
                send(cmd, "txScheduling", userIndex);
            }
        }

        scheduleAt(simTime() + par("schedulingPeriod").doubleValue(), selfMsg);
    }
}
