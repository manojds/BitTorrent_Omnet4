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
import inet.util.NAMTraceWriter;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.applications.tcpapp.TCPApp;
import inet.transport.tcp.TCP;
import inet.transport.udp.UDP;
import inet.applications.udpapp.UDPApp;
import inet.nodes.inet.NetworkLayer;
import inet.applications.pingapp.PingApp;
import inet.linklayer.ppp.PPPInterface;
import inet.linklayer.ethernet.EthernetInterface;


//
// \IP host with TCP, UDP layers and applications.
//
module Tracker
{
    parameters:
        string channelType;
        double numTcpApps;
        double numUdpApps;
        string tcpAppType;
        string udpAppType;
        bool IPForward;
        double namid;
        string routingFile;
    gates:
	//Following two gates are commented by Manoj
        //input in[];
        //output out[];
	//added following gate instead of above
	inout pppg[];

        input ethIn[];
        output ethOut[];
    submodules:
        namTrace: NAMTraceWriter {
            parameters:
                namid = namid;
                @display("p=60,310;i=block/sink");
        }
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=60,70;i=block/control");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=60,150;i=block/table");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = IPForward;
                routerId = "";
                routingFile = routingFile;
                @display("p=60,230;i=block/table");
        }
        tcpApp[numTcpApps]: <tcpAppType> like TCPApp {
            parameters:
                @display("p=163,67;i=block/app");
        }
        tcp: TCP {
            parameters:
                @display("p=163,154;i=block/wheelbarrow");
        }
        udpApp[numUdpApps]: <udpAppType> like UDPApp {
            parameters:
                @display("i=block/app;p=272,67");
        }
        udp: UDP {
            parameters:
                @display("p=272,154;i=block/transport");
        }
        pingApp: PingApp {
            parameters:
                @display("i=block/app;p=343,200");
        }
        networkLayer: NetworkLayer {
            parameters:
                proxyARP = false;
                @display("p=248,247;i=block/fork;q=queue");
            gates:
                ifIn[sizeof(pppg)+sizeof(ethOut)];
                ifOut[sizeof(pppg)+sizeof(ethOut)];
        }
        ppp[sizeof(pppg)]: PPPInterface {
            parameters:
                @display("p=205,350,row,90;q=txQueue;i=block/ifcard");
        }
        eth[sizeof(ethOut)]: EthernetInterface {
            parameters:
                @display("p=240,350,row,90;q=txQueue;i=block/ifcard");
        }
    connections allowunconnected:
        for i=0..numTcpApps-1 {
            tcpApp[i].tcpOut --> {  } --> tcp.appIn++;
            tcpApp[i].tcpIn <-- {  } <-- tcp.appOut++;
        }

        tcp.ipOut --> {  } --> networkLayer.tcpIn;
        tcp.ipIn <-- {  } <-- networkLayer.tcpOut;

        for i=0..numUdpApps-1 {
            udpApp[i].udpOut --> {  } --> udp.appIn++;
            udpApp[i].udpIn <-- {  } <-- udp.appOut++;
        }

        udp.ipOut --> {  } --> networkLayer.udpIn;
        udp.ipIn <-- {  } <-- networkLayer.udpOut;

        networkLayer.pingOut --> {  } --> pingApp.pingIn;
        networkLayer.pingIn <-- {  } <-- pingApp.pingOut;

        // connections to network outside
        for i=0..sizeof(pppg)-1 {
		//Edited by Manoj
            //in[i] --> {  } --> ppp[i].phys$i;
            //out[i] <-- {  } <-- ppp[i].phys$o;
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

