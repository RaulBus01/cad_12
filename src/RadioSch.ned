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
import User;
import Sink;
import Scheduler;

network RadioSch2
{
    parameters:
        int usersCount = default(8);

        @display("bgb=356,364");
    types:
    submodules:
        scheduler: Scheduler {
            usersCount = parent.usersCount;
        }
        sink: Sink {
            gateSize = parent.usersCount;
            @display("p=77,197");
        }
        user[usersCount]: User {
            
            @display("p=200,100+100*i");
        }
          flc: FLC {
            parameters:
          
                @display("p=89,100");
        }
        gen: GeneratorFLC {
            parameters:
                @display("p=209,100");
        }
    connections:
        gen.out --> flc.in;
        scheduler.toFLC --> flc.fromScheduler;
    
        
        for i=0..usersCount - 1 {
            scheduler.txScheduling[i] --> user[i].rxScheduling;
            user[i].txPackets --> sink.rxPackets[i];
            user[i].txInfo --> scheduler.rxInfo[i];

        }
}

