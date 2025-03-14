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

//
// TODO auto-generated module
//
package cad;
simple Sink
{
 parameters:
        int gateSize;
        double meanHPDelay = default(0.0) @mutable; 
        @display("i=block/sink");
        //individual signals for each user 
        @signal[lifetime:user0](type="simtime_t");
        @signal[lifetime:user1](type="simtime_t");
        @signal[lifetime:user2](type="simtime_t");
        @signal[lifetime:user3](type="simtime_t");
        @signal[lifetime:user4](type="simtime_t");
        @signal[lifetime:user5](type="simtime_t");
        @signal[lifetime:user6](type="simtime_t");
        @signal[lifetime:user7](type="simtime_t");
        
 
        @statistic[userLifetime0](source="lifetime:user0"; record=vector,stats; title="Packet lifetime for user 0");
        @statistic[userLifetime1](source="lifetime:user1"; record=vector,stats; title="Packet lifetime for user 1");
        @statistic[userLifetime2](source="lifetime:user2"; record=vector,stats; title="Packet lifetime for user 2");
        @statistic[userLifetime3](source="lifetime:user3"; record=vector,stats; title="Packet lifetime for user 3");
        @statistic[userLifetime4](source="lifetime:user4"; record=vector,stats; title="Packet lifetime for user 4");
        @statistic[userLifetime5](source="lifetime:user5"; record=vector,stats; title="Packet lifetime for user 5");
        @statistic[userLifetime6](source="lifetime:user6"; record=vector,stats; title="Packet lifetime for user 6");
        @statistic[userLifetime7](source="lifetime:user7"; record=vector,stats; title="Packet lifetime for user 7");
     
    gates:
        input rxPackets[gateSize];

}
