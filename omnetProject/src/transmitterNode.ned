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

package wsn;

module Transmitter
{
    parameters:
        int nodeId;
        int nodeXPos;
        int nodeYPos;
        
    gates:
        input in0;
        output out0;

    submodules:
        packGen: PacketGen;
        MAC: MAC;
        trans: Transceiver;
        
    connections:
        packGen.out0 --> MAC.in0;
        trans.out0 --> MAC.in1;
        MAC.out0 --> trans.in0;
        trans.out1 --> out0;
        in0 --> trans.in1;
        


}
