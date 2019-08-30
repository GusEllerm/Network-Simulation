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

import wsn.Channel;
import wsn.ReceiverNode;
import wsn.TransmitterNode;

network simulation
{
    parameters:
        int numTX;
        double radius;
    
    submodules:
        TXNode[numTX] : TransmitterNode
        {
            nodeId = index;
            nodeXPos = radius * cos((2 * 3.14159 * index) / numTX);
            nodeYPos = radius * sin((2 * 3.14159 * index) / numTX);
        };
        
        RXNode : ReceiverNode
        {
        	nodeId = -1;
        	nodeXPos = 0;
        	nodeYPos = 0;
        };
        
        Chan : Channel {};
        
	connections allowunconnected:
	    Chan.out++ --> RXNode.in0;
	    
	    for idx = 0 .. (numTX - 1)
	    {
	    	Chan.in++ <-- TXNode[idx].out0;
	    }
}