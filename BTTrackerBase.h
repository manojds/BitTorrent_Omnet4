//
// Copyright 2008, 2009 Vasileios P. Kemerlis (aka vpk)
//                              vpk@cs.columbia.edu
//                              http://www.cs.columbia.edu/~vpk
// Copyright 2008, 2009 Konstantinos V. Katsaros
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


#ifndef __BTTRACKERBASE_H_
#define __BTTRACKERBASE_H_

#include <string>
#include <sstream>
#include <set>
#include <omnetpp.h>
#include "../tcpapp/TCPSrvHostApp.h"
#include "BTTrackerMsg_m.h"

using namespace std;

// event constants
#define EVT_CLN		0 // cleanup
#define EVT_TOUT	1 // timeout
#define EVT_STAT    2 // cleanup

// server states
#define PENDING		0 // pending state
#define ESTABLISHED	1 // established state
#define TEARDOWN	2 // closing state

// announce validation
#define A_VALID_NORMAL		0 // valid nomral announce
#define A_VALID_STARTED		1 // valid announce with started event
#define A_VALID_COMPLETED	2 // valid announce with completed event
#define A_VALID_STOPPED		3 // valid announce with stopped event
#define A_INVALID_EVENT		4 // invalid event field in the announce
#define A_INVALID_IHASH		5 // invalid info hash in the announce
#define A_INVALID_PEERID	6 // no peer id in the announce
#define A_INVALID_PORT		7 // invalid port in the announce
#define A_NO_STARTED		8 // announce with no started event before

// bencoded sizes
#define CMPCT_PEER		6 	// size of each peer in compact form (i.e., 6 bytes)
#define FAIL_PADD		19 	// padding for failure (includes padding for the dictionary that has everything inside)
#define WARN_PADD		18 	// padding for warning (not including the padding for the dictionary)
#define INT_PADD		2 	// padding for bencoded integers
#define LIST_PADD		2 	// padding for bencoded lists
#define DICT_PADD		2 	// padding for bencoded dictionaries
#define RDICT_PADD		51 + DICT_PADD // padding for the response dictionary (including the std fields)
#define PEERID_PADD		9	// padding for the peer id key
#define IPADDR_PADD		4	// padding for the ip key
#define PORT_PADD		6	// padding for the port key

class BTTrackerStructBase;

/**
 * BitTorrent protocol.
 * Implements a threaded tracker as described at http://wiki.theory.org/BitTorrentSpecification
 */
class INET_API BTTrackerBase : public TCPSrvHostApp
{
	protected:
		/* New members in BitTorrentTrackerBase */
		// start of members documented in .ned file
		string infoHash_var;
		string trackerId_var;
		bool alwaysSendTrackerId_var;
		bool compactSupport_var;
		size_t maxPeersInReply_var;
		size_t announceInterval_var;
		size_t cleanupInterval_var;
		size_t sessionTimeout_var;

		// end of members documented in .ned file
		cArray peers_var; 	// peers container (i.e., the peers in the swarm)
		std::map<std::string, int> map_Peers;   //map which map the key or peer ID (if key is empty peer ID will be used ) to the peer index in peer array.
		size_t seeds_var; 	// seeds counter
		//following two variables are added by Manoj
		size_t completed_count_var;
		size_t started_count_var;
		size_t peersNum_var; 	// peers counter
		cMessage* clean; 	// cleanup event
		cMessage* statMsg;    // cleanup event
		size_t currClean_exit_var; //Ntinos Katsaros 14/09/2008: Current number of subsequent clean up intervals
					  // in which no peer has sent an announce.


	public:
		/* Constructor */
		BTTrackerBase();
		/* Destructor */
		virtual ~BTTrackerBase();
		/* set and get methods */
		const string& infoHash() const;
		void setInfoHash(const string&);
		const string& trackerId() const;
		void setTrackerId(const string&);
		bool alwaysSendTrackerId() const;
		void setAlwaysSendTrackerId(bool);
		bool compactSupport() const;
		void setCompactSupport(bool);
		size_t maxPeersInReply() const;
		void setMaxPeersInReply(size_t);
		size_t announceInterval() const;
		void setAnnounceInterval(size_t);
		size_t cleanupInterval() const;
		void setCleanupInterval(size_t);
		size_t sessionTimeout() const;
		void setSessionTimeout(size_t);
		cArray& peers();
		int contains(BTTrackerStructBase*) const;
		int containsPeer(const std::string & _sPeerID) const;
		size_t seeds() const;
		void setSeeds(size_t);
		void incrementCompletedCount();
		void incrementStartedCount();
		size_t peersNum() const;
		void setPeersNum(size_t);

		void insertPeerIntoMap(const std::string & _sPeerID, int _iIndex);
		void removePeerFromtheMap(const std::string & _sPeerID);



		//Ntinos Katsaros 24/11/2008
		void cleanUpPeers();
		void cleanRemovePeer(BTTrackerStructBase*);
		bool cleanRemovePeer(int);


	protected:
		/* Redefined methods from TCPSrvHostApp */
		virtual void initialize();
		virtual void handleMessage(cMessage*);

		virtual void writeStats();
};

/**
 * BitTorrent protocol.
 * The connection handler thread.
 * Spawned from BitTorrentTrackerBase each time a new announce is made.
 */
class INET_API BTTrackerClientHandlerBase : public TCPServerThreadBase
{
	protected:
		/* New members in BitTorrentTrackerClientHandlerBase */
		cMessage* evtTout;	// session timeout
		size_t state_var;	// the state of the thread
		int cPeer;		// index to the peer that made the announce (i.e., its position in the vector container)
		string peer_id;


		/* Reflection methods */
		virtual int processAnnounce(BTTrackerMsgAnnounce*); 			// announce processing method
		virtual void sendResponse(int, BTTrackerMsgAnnounce*);			// response sending method
		//function signature changed by Manoj. 2015-02-01
		virtual void fillPeersInResponse(BTTrackerMsgAnnounce* amsg, BTTrackerMsgResponse*, bool, bool);	// selects the peers which will included in the response
		virtual void findAndSetResponseSize(BTTrackerMsgResponse*, bool, bool); // set the size of the response
		size_t sizeOfStrInt(size_t) const;						// count the length of an integer in ASCII representation

		//added by Manoj - 2015-06-06.
		//to achieve a polymophic behaviour for BTTrackerStructBase object creation
		virtual BTTrackerStructBase * createTrackerStructObj(BTTrackerMsgAnnounce* amsg);

		void updatePeerDetailsFromAnnounceMsg(BTTrackerMsgAnnounce* _pMsg, int _iPeerIndex, bool _bSetIsSeeder);

	public:
		/* Constructor */
		BTTrackerClientHandlerBase();
		/* Destructor */
		virtual ~BTTrackerClientHandlerBase();
		/* set and get methods */
		size_t tstate() const;
		void setTstate(size_t state);
		/* Redefined method from TCPServerThreadBase */
		virtual BTTrackerBase* getHostModule();


	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void failure(int code);
};

/**
 * BitTorrent protocol.
 * Implements an entry in the peers\' vector.
 */
class INET_API BTTrackerStructBase : public cObject
{
	protected:
		/* New members in BitTorrentTrackerStructBase */
		string peerId_var;		// peer id
		IPvXAddress ipAddress_var;	// ip address
		size_t peerPort_var;		// port
		string key_var;			// key
		simtime_t lastAnnounce_var;	// time of the last announce we got from that peer
		bool isSeed_var;		// flag - the peer is seed or not

	public:
		/* Constructor */
		BTTrackerStructBase(const IPvXAddress&, const string&, size_t, const string&, simtime_t, bool);
		/* Destructor */
		virtual ~BTTrackerStructBase();
		/* operator == */
		bool operator==(const BTTrackerStructBase&);
		/* set and get methods */
		const IPvXAddress& ipAddress() const;
		void setIpAddress(const IPvXAddress&);
		const string& peerId() const;
		void setPeerId(const string&);
		size_t peerPort() const;
		void setPeerPort(size_t);
		const string& key() const;
		void setKey(const string&);
		simtime_t lastAnnounce() const;
		void setLastAnnounce(simtime_t);
		bool isSeed() const;
		void setIsSeed(bool);
};

#endif
