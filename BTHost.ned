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

import inet.base.NotificationBoard;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.transport.udp.UDP;
import inet.nodes.inet.NetworkLayer;
import inet.applications.pingapp.PingApp;
import inet.linklayer.ppp.PPPInterface;
import inet.applications.tcpapp.TCPApp;
import inet.transport.tcp.TCP;
import inet.linklayer.ppp.PPPInterface;
import inet.linklayer.ethernet.EthernetInterface;

//
// Host that participates in the overlay
//
// @author Konstantinos Katsaros
//
// Based on StandardHost module created by Markus Mauch, Bernhard Heep
//

module BTHost
{
    parameters:
        string channelType = default("fiberline");
        double ASID= default(0);	//Ntinos Katsaros: ID of the AS this host belongs to (negative)
        bool IPForward= default(false); // true if ip packets should be forwarded
        string trackerClientTCPAppType;
        string peerWireTCPAppType;
        string routingFile= default(""); // use predefined routing file if given
        //Added by Manoj - BTR-012 - 2015-03-01
        bool seeder= default(false);

        @display("bgb=337,472");
    gates:
		//Commented by Manoj
        //input in[]; // incoming gates from router
        input ethIn[]; // placeholder for zero-size vector

		//Commented by Manoj
        //output out[]; // outgoing gates to router
        output ethOut[]; // placeholder for zero-size vector
		
		//Added by Manoj
		inout pppg[];

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=68,208;i=block/control");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=68,278;i=block/table");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = IPForward; // true if ip packets should be forwarded
                routerId = ""; // id of the router
                routingFile = routingFile; // use predefined routing file if given
                @display("p=68,348;i=block/table");
        }
        trackerClient: <trackerClientTCPAppType> like TCPApp {
            parameters:
                @display("p=120,80;i=block/app");
        }
        peerWire: <peerWireTCPAppType> like TCPApp {
            parameters:
                @display("p=200,80;i=block/app");
        }
        tcp: TCP {
            parameters:
                @display("p=200,160;i=block/wheelbarrow");
        }
        networkLayer: NetworkLayer {
            parameters:
                proxyARP = false; // sets proxy \ARP mode (replying to \ARP requests for the addresses for which a routing table entry exists)
                @display("p=200,240;i=block/fork;q=queue");
            gates:
                ifIn[sizeof(pppg)+sizeof(ethOut)];
                ifOut[sizeof(pppg)+sizeof(ethOut)];
        }
        ppp[sizeof(pppg)]: PPPInterface {
            parameters:
                @display("p=200,320,row,90;q=txQueue;i=block/ifcard");
        }
        eth[sizeof(ethOut)]: EthernetInterface {
            parameters:
                @display("p=240,350,row,90;q=txQueue;i=block/ifcard");

        }
    connections allowunconnected:
        peerWire.btorrentOut --> {  } --> trackerClient.btorrentIn;
        trackerClient.btorrentOut --> {  } --> peerWire.btorrentIn;

        peerWire.tcpOut --> {  } --> tcp.appIn++;
        peerWire.tcpIn <-- {  } <-- tcp.appOut++;

        trackerClient.tcpOut --> {  } --> tcp.appIn++;
        trackerClient.tcpIn <-- {  } <-- tcp.appOut++;

        tcp.ipOut --> {  } --> networkLayer.tcpIn;
        tcp.ipIn <-- {  } <-- networkLayer.tcpOut;

        // connections to network outside
        for i=0..sizeof(pppg)-1 {
	//edited by Manoj
            //pppg$i[i] --> {  } --> ppp[i].physIn;
            //pppg$o[i] <-- {  } <-- ppp[i].physOut;
		pppg[i] <--> {  } <--> ppp[i].phys;
            ppp[i].netwOut --> {  } --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- {  } <-- networkLayer.ifOut[i];
        }

        for i=0..sizeof(ethOut)-1 {
            ethIn[i] --> {  } --> eth[i].physIn;
            ethOut[i] <-- {  } <-- eth[i].physOut;
            eth[i].netwOut --> {  } --> networkLayer.ifIn[sizeof(pppg)+i];
            eth[i].netwIn <-- {  } <-- networkLayer.ifOut[sizeof(pppg)+i];
        }

}

