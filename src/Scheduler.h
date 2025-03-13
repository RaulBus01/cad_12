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

#ifndef __SCHED_TST_SCHEDULER_H_
#define __SCHED_TST_SCHEDULER_H_

#include <omnetpp.h>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class User{
public:
    User(int index, int weight)
    {
        this->index = index;
        this->weight = assignUserWeight(index);
        this->lastTimeServed = 0;
        this->radioLinkQuality = 1.0;
        this->queueLength = 0;
    }

    int assignUserWeight(int index)
    {
       if (index == 0)
            return 4; // High Priority
        else if (index == 1)
            return 5; // High Priority
        else if (index >= 2 && index <= 5) {
            //Medium Priority
            return 2;
        }
        else if (index >= 6 && index <= 7)
            return 1; // Low Priority
        else
            return 1; // Default Low Priority
       
    }

    int addQueueLength()
    {
        this->queueLength++;
        return this->queueLength;
    }

    int decrementQueueLength(int subtrahend)
    {
        this->queueLength -= subtrahend;
        return this->queueLength;
    }

    int getQueueLength()
    {
        return this->queueLength;
    }

    double updateLastTimeServed(double currentSimTime)
    {
        this->lastTimeServed = currentSimTime;
        return this->lastTimeServed;
    }

    double getUserPriority(double currentSimTime)
    {
        if(this->lastTimeServed < 0)
        {
            return this->weight;
        }
        return this->weight*(currentSimTime - this->lastTimeServed) * this->radioLinkQuality;
    }

    int getUserIndex()
    {
        return this->index;
    }

    int getUserWeight()
        {
            return this->weight;
        }
    void updateDelayStats(double delay) {
        totalDelay += delay;
        packetCount++;
    }
    
    double getAverageDelay() const {
        return packetCount > 0 ? totalDelay / packetCount : 0.0;
    }
    
    int getPacketCount() const {
        return packetCount;
    }
     void setRadioLinkQuality(double quality) {
        this->radioLinkQuality = quality;
    }
    
    double getRadioLinkQuality() const {
        return this->radioLinkQuality;
    }
private:
    User();
    int index;
    int weight;
    double totalDelay;
    int packetCount;
    int queueLength;
    double radioLinkQuality;
    double lastTimeServed;
};


class Scheduler : public cSimpleModule
{
public:
    Scheduler();
    ~Scheduler();
private:
    cMessage *selfMsg;
    int NrUsers;
    std::vector<User> users;
    int NrOfChannels;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
