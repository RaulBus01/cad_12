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

    csvFile.open("delays.csv");
    csvFile << "Time,UserIndex,RadioLinkQuality,Weight,Priority,Delay\n"; // Header

}
void Sink::writeToCSV(int userIndex, double radioLinkQuality,int weight, int priority, simtime_t delay) {
    csvFile << simTime() << "," << userIndex << "," <<radioLinkQuality << ","<< weight << "," << priority << "," << delay << "\n";
           
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
  
     if(msg->arrivedOn("rxPackets"))
     {
   
        int userPriority = (int)msg->par("userPriorityType");
        int userWeight = (int)msg->par("Weight");
        int userIndex = (int)msg->par("userIndex");
        double radioLinkQuality = msg->par("radioLinkQuality").doubleValue();

        writeToCSV(userIndex, radioLinkQuality, userWeight, userPriority, lifetime);
      
     }
     
    
      delete msg;
}
void Sink::finish()
{
    
    csvFile.close();


    
  
   
}
