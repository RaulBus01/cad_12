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
    void setQueueLength(int length)
    {
        this->queueLength = length;
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
   
        return this->weight * (currentSimTime - this->lastTimeServed) * getAverageChannelQuality();
    }

    // Add method to calculate average channel quality
    double getAverageChannelQuality() const {
        if (channelQualities.empty()) return 1.0; // Default if no qualities set
        
        double sum = 0.0;
        for (const auto& quality : channelQualities) {
            sum += quality;
        }
        return sum / channelQualities.size();
    }

    int getUserIndex()
    {
        return this->index;
    }
    void setUserWeight(int newWeight) {
        this->weight = newWeight;
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
 
    std::vector<double> channelQualities; // Store per-channel quality values

    void setChannelQualities(const std::vector<double>& qualities) {
        this->channelQualities = qualities;
    }

    // Update to only use channel qualities, remove radioLinkQuality fallback
    double getChannelQuality(int channelIndex) const {
        if (channelIndex >= 0 && channelIndex < (int)channelQualities.size())
            return channelQualities[channelIndex];
        return 1.0; // Default quality if channel index is invalid
    }

    // Get a metric for overall radio link quality (for display/statistics)
    double getRadioLinkQuality() const {
        return getAverageChannelQuality();
    }
private:
    User();
    int index;
    int weight;
    double totalDelay;
    int packetCount;
    int queueLength;
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
