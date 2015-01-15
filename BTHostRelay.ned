//
// Copyright 2009 Konstantinos V. Katsaros
//                              ntinos@aueb.gr
//                              http://mm.aueb.gr/~katsaros
//

// This file is part of BitTorrent Implementation for OMNeT++.

//    BitTorrent Implementation for OMNeT++ is free software: you can redistribute
//	  it and/or modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the License,
//	  or (at your option) any later version.

//    BitTorrent Implementation for OMNeT++ is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with BitTorrent Implementation for OMNeT++.
//    If not, see <http://www.gnu.org/licenses/>.

package inet.applications.BitTorrent;



import inet.applications.tcpapp.TCPApp;


//
// Host that participates in the overlay
//
// @author Konstantinos Katsaros
//
// Based on StandardHost module created by Markus Mauch, Bernhard Heep
//

module BTHostRelay extends BTHost
{
    parameters:
		string relayTrackerClientTCPAppType;
        @display("bgb=337,472");
    gates:


    submodules:
        //sub modlue which handles the tracker communication for the relay torrent hash
        relayTrackerClient: <relayTrackerClientTCPAppType> like TCPApp {
            parameters:
                @display("p=290,80;i=block/app");
        }
                

    connections allowunconnected:
        peerWire.relayTrackerOut --> {  } --> relayTrackerClient.relayTrackerIn;
        relayTrackerClient.btorrentOut --> {  } --> peerWire.btorrentIn;
        
        relayTrackerClient.tcpOut --> {  } --> tcp.appIn++;
        relayTrackerClient.tcpIn <-- {  } <-- tcp.appOut++;
}

