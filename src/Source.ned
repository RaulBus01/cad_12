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
package cad;
simple Source
{
     parameters:
        double sendIaTime @unit(ms) = default(10 ms);
        double MAX_Sim @unit(ms) = default(12000 ms);
        
        double networkLoad = default(0.5);
        int packetsPerUser = default(10);
        int usersCount = default(8);
        int channels = default(15);
        volatile double generatorPeriod @unit(ms) = default(100ms);
        @display("i=block/source");

    gates:
        output txPackets;
}
