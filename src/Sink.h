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

#ifndef __CAD_SINK_H
#define __CAD_SINK_H

#include <omnetpp.h>
using namespace omnetpp;

class Sink : public cSimpleModule
{
  private:
    // Signal declarations
    std::vector<simsignal_t> lifetimeSignals;
    
    // HP delay tracking variables
    double meanHPDelay;
    simtime_t lastUpdateTime;
    simtime_t updateInterval;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif
