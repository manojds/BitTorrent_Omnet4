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
//
// Module for storing global simulation parameters
//
simple BTStatistics
{
    parameters:
        //modifeid by Manoj - BTR-012 - 2015-03-01
        //volatile double targetOverlayTerminalNum;
        int numDownloadersToComplete;
        //Added by Manoj for Logging
		volatile int logLevel;
		//Added by Manoj, 2015-04-17. to be able to control the simulation time
		//because this parameters's hard coded value is too high for
		//small file downloads
		int simulationFinishDelay=default(20000);
		
		bool 	recordPerPeerStats= default(false);
		int		perPeerStatInterval = default (25);
    gates:
        input direct_in;
}
