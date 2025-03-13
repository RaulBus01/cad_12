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
        users.push_back((User(user, user+1)));
    }

    scheduleAt(simTime(), selfMsg);
}

void Scheduler::handleMessage(cMessage *msg) {
    // Handle info messages
    for(int i = 0; i < NrUsers; i++) {
        if (msg->arrivedOn("rxInfo", i)) {
            users[i].addQueueLength();
            double currentTime = simTime().dbl();
            double delay = currentTime - msg->getCreationTime().dbl();
            users[i].updateDelayStats(delay);
            if(msg->hasPar("radioLinkQuality")) {
                double radioQuality = msg->par("radioLinkQuality").doubleValue();
                users[i].setRadioLinkQuality(radioQuality);
                EV << "Scheduler: User " << i << " has radio link quality " << radioQuality << endl;
            }
            
            EV << "Scheduler: Update queue length: user[" << i << "]= " 
               << users[i].getQueueLength() << endl;
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

        // Process and send in priority order
        for(const auto& [priority, userIndex] : priorityQueue) {
            if(remainingChannels <= 0) break;

            int userQueueLength = std::max(0, users[userIndex].getQueueLength());
            EV << "Priority: " << priority << " - User " << userIndex 
               << " has " << userQueueLength << " blocks in queue" << endl;
            int blocksToAllocate = std::min(userQueueLength, remainingChannels);
            EV << "Priority: " << priority << " - Allocating " << blocksToAllocate 
               << " blocks to user " << userIndex << "with link quality " << users[userIndex].getRadioLinkQuality() << endl;
            if(blocksToAllocate > 0) {

                cMessage *cmd = new cMessage("cmd");
                cmd->addPar("nrBlocks");
                cmd->par("nrBlocks").setLongValue(blocksToAllocate);

                cmd->addPar("Weight");
                cmd->par("Weight").setLongValue(users[userIndex].getUserWeight());

                cmd->addPar("userPriorityType");
                cmd->par("userPriorityType").setLongValue(users[userIndex].getUserPriority(currentSimTime));

                cmd->addPar("userIndex");
                cmd->par("userIndex").setLongValue(userIndex);

                cmd->addPar("radioLinkQuality");
                cmd->par("radioLinkQuality").setDoubleValue(users[userIndex].getRadioLinkQuality());


                EV << "Scheduler: Sending " << blocksToAllocate << " blocks to user " << userIndex << endl;

                users[userIndex].updateLastTimeServed(currentSimTime);
                users[userIndex].decrementQueueLength(blocksToAllocate);
                remainingChannels -= blocksToAllocate;

                send(cmd, "txScheduling", userIndex);
            }
        }

        scheduleAt(simTime() + par("schedulingPeriod").doubleValue(), selfMsg);
    }
}
