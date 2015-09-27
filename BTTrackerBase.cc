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


#include "BTTrackerBase.h"
#include "BTLogImpl.h"

#define BEV	EV << "[BitTorrent_mjp] [BitTorrent Tracker]:"

Register_Class(BTTrackerClientHandlerBase);
Define_Module(BTTrackerBase);

/**
 * Constructor.
 */
BTTrackerBase::BTTrackerBase()
{
}

/**
 * Destructor.
 */
BTTrackerBase::~BTTrackerBase()
{
	// clean any pending messages
	cancelAndDelete(clean);
	cleanUpPeers();
}

/**
 * Deallocating memory used for the peer set.
 */
void BTTrackerBase::cleanUpPeers()
{
	for(int i=0; i<peers().size(); i++)
	{
		// get an entry
		BTTrackerStructBase* peer = (BTTrackerStructBase*)peers()[i];
		peers().remove(i);
		delete peer;
	}
}

/**
 * Remove a peer from the peer set ensuring memory deallocation.
 */
void BTTrackerBase::cleanRemovePeer(BTTrackerStructBase* peer)
{
	cleanRemovePeer(peers().find(peer));
}

/**
 * Remove a peer from the peer set ensuring memory deallocation.
 */
void BTTrackerBase::cleanRemovePeer(int index)
{
	if (index>=0)
	{
		BTTrackerStructBase* peer = (BTTrackerStructBase*)peers()[index];
		peers().remove(index);
		delete peer;
	}
	else
		opp_error("Cannnot delete peer entry. Indicated peer not found in the set.");
}

/**
 * Called after the module creation.
 */
void BTTrackerBase::initialize()
{
	// parent initialize
	TCPSrvHostApp::initialize();
	// member init
	infoHash_var		= /*(string)*/ par("infoHash").stdstringValue ();
	trackerId_var		= /*(string)*/ par("trackerId").stdstringValue ();
	alwaysSendTrackerId_var = (bool)par("alwaysSendTrackerId");
	compactSupport_var	= (bool)par("compactSupport");
	maxPeersInReply_var	= (size_t)par("maxPeersInReply");
	announceInterval_var	= (size_t)par("announceInterval");
	cleanupInterval_var	= (size_t)par("cleanupInterval");
	sessionTimeout_var 	= (size_t)par("sessionTimeout");

	peers_var		= cArray();
	seeds_var		= 0;
	peersNum_var	= 0;
	clean			= new cMessage(NULL, EVT_CLN);

	// watches
	WATCH(infoHash_var);
	WATCH(trackerId_var);
	WATCH(alwaysSendTrackerId_var);
	WATCH(compactSupport_var);
	WATCH(maxPeersInReply_var);
	WATCH(announceInterval_var);
	WATCH(cleanupInterval_var);
	WATCH(sessionTimeout_var);
	WATCH_OBJ(peers_var);
	WATCH(seeds_var);
	WATCH(peersNum_var);

	// schedule the first cleanup event
	scheduleAt(simTime() + (simtime_t)cleanupInterval_var, clean);

	BT_LOG_ESSEN(btLogSinker, "BTTrackerClientHandlerB::initialize", "Tracker ["<<this<<"] Initialized");
	std::cout<<"****** Tracker Initialized *******"<<std::endl;
}

/**
 * Called each time a new message is received.
 */
void BTTrackerBase::handleMessage(cMessage* msg)
{
	// local handling of the cleanup message
	if(msg->getKind() == EVT_CLN)
	{

		// traverse the peers pool and remove the inactive entries
		for(int i=0; i<peers_var.size(); i++)
		{
			// get an entry
			BTTrackerStructBase* tpeer = (BTTrackerStructBase*)peers_var[i];
			// the entry is not null and is inactive
			if(tpeer && (simTime() - tpeer->lastAnnounce() >= (simtime_t)cleanupInterval_var))
			{
				// remove
				if(tpeer->isSeed()) // if the peer was a seeder change the seeds count
				{
					seeds_var--;
				}
				//peers_var.remove(tpeer);
				cleanRemovePeer(tpeer);
				peersNum_var--;
			}

		}


		// schedule the next cleanup event
		scheduleAt(simTime() + (simtime_t)cleanupInterval_var, clean);
	}
	else
	{
		// default handling
		TCPSrvHostApp::handleMessage(msg);
	}
}

/**
 * Get the info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
const string& BTTrackerBase::infoHash() const
{
	return infoHash_var;
}

/**
 * Set the info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
void BTTrackerBase::setInfoHash(const string& infoHash)
{
	infoHash_var = infoHash;
}

/**
 * Get the tracker id.
 * This value is returned back to a client after the first announce
 * and should be included in future announces.
 */
const string& BTTrackerBase::trackerId() const
{
	return trackerId_var;
}

/**
 * Set the tracker id.
 * This value is returned back to a client after the first announce
 * and should be included in future announces.
 */
void BTTrackerBase::setTrackerId(const string& trackerId)
{
	trackerId_var = trackerId;
}

/**
 * Get the alwaysSendTrackerId flag.
 * If the flag is true the tracker id will be returned in every response.
 * If the flag is set to false the tracker id will be returned to the
 * client only in the first response.
 */
bool BTTrackerBase::alwaysSendTrackerId() const
{
	return alwaysSendTrackerId_var;
}

/**
 * Set the alwaysSendTrackerId flag.
 * If the flag is true the tracker id will be returned in every response.
 * If the flag is set to false the tracker id will be returned to the
 * client only in the first response.
 */
void BTTrackerBase::setAlwaysSendTrackerId(bool alwaysSendTrackerId)
{
	alwaysSendTrackerId_var = alwaysSendTrackerId;
}

/**
 * Get the compactSupport flag.
 * Support for compact responses or not.
 */
bool BTTrackerBase::compactSupport() const
{
	return compactSupport_var;
}

/**
 * Set the compactSupport flag.
 * Support for compact responses or not.
 */
void BTTrackerBase::setCompactSupport(bool compactSupport)
{
	compactSupport_var = compactSupport;
}

/**
 * Get the maximum number of peers which can be included in a response.
 */
size_t BTTrackerBase::BTTrackerBase::maxPeersInReply() const
{
	return maxPeersInReply_var;
}

/**
 * Set the maximum number of peers which can be included in a response.
 */
void BTTrackerBase::setMaxPeersInReply(size_t maxPeersInReply)
{
	maxPeersInReply_var = maxPeersInReply;
}

size_t BTTrackerBase::announceInterval() const
{
	return announceInterval_var;
}

/**
 * Set the announce interval (in seconds).
 */
void BTTrackerBase::setAnnounceInterval(size_t announceInterval)
{
	announceInterval_var = announceInterval;
}

/**
 * Get the announce interval (in seconds).
 */
size_t BTTrackerBase::cleanupInterval() const
{
	return cleanupInterval_var;
}

/**
 * Set the cleanup interval (in seconds).
 */
void BTTrackerBase::setCleanupInterval(size_t cleanupInterval)
{
	cleanupInterval_var = cleanupInterval;
}

/**
 * Get the cleanup interval (in seconds).
 */
size_t BTTrackerBase::sessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout value (in seconds).
 */
void BTTrackerBase::setSessionTimeout(size_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}

/**
 * Get the peers container.
 */
cArray& BTTrackerBase::peers()
{
	return peers_var;
}

/**
 * Returns the position of obj inside the peers container (if it is contained)
 * or -1 if obj is not found.
 *
 * This lame method was inserted because the find() method of the cArray
 * class is so stupid that does not use the overloaded == operator.
 */
int BTTrackerBase::contains(BTTrackerStructBase* obj) const
{
	// temp peer
	BTTrackerStructBase* tpeer;

	// traverse the peers pool to find obj
	for(int i=0; i<peers_var.size(); i++)
	{
		// get peer i
		tpeer = (BTTrackerStructBase*)peers_var[i];
		// user operator ==
		if(tpeer && obj && (*obj == *tpeer))
		{
			return i;
		}
	}

	// not found
	return -1;
}

/**
 * Get the seeds count.
 */
size_t BTTrackerBase::seeds() const
{
	return seeds_var;
}

/**
 * Set the seeds count.
 */
void BTTrackerBase::setSeeds(size_t seeds)
{
	seeds_var = seeds;
}

/**
 * Get the peers count.
 */
size_t BTTrackerBase::peersNum() const
{
	return peersNum_var;
}

/**
 * Set the peers count.
 */
void BTTrackerBase::setPeersNum(size_t peersNum)
{
	peersNum_var = peersNum;
}




/**
 * Constructor.
 */
BTTrackerClientHandlerBase::BTTrackerClientHandlerBase()
{
	// members init
	evtTout		= new cMessage(NULL, EVT_TOUT);
	state_var	= PENDING;
	cPeer		= -1;
}

/**
 * Destructor.
 */
BTTrackerClientHandlerBase::~BTTrackerClientHandlerBase()
{
	// cancel any pending messages
	getHostModule()->cancelAndDelete(evtTout);
}

/**
 * Handle an announce.
 * Returns the status of the process (i.e., a constant value which indicates what should the tracker reply with).
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
int BTTrackerClientHandlerBase::processAnnounce(BTTrackerMsgAnnounce* amsg)
{
    BT_LOG_INFO(btLogSinker, "BTTrackerClientHandlerB::processAnnounce", "Announce request from client[address="
            << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] with event ["<<amsg->event()<<"] Client ["<<amsg->peerId()<<"]");

	// temporary peer struct with the announce info
	BTTrackerStructBase* tpeer 	= NULL;
	// a peer from the pool
	BTTrackerStructBase* peer 	= NULL;
	// the position of the peer in the pool
	cPeer					= -1;

	// sanity checks - failures
	// invalid info hash
	if(amsg->infoHash() != getHostModule()->infoHash())
	{
		return A_INVALID_IHASH;
	}
	// no peer id
	if(strlen(amsg->peerId()) == 0)
	{
		return A_INVALID_PEERID;
	}
	// invalid port
	if(amsg->peerPort() == 0)
	{
		return A_INVALID_PORT;
	}

	// init the temp peer struct
	tpeer = createTrackerStructObj(amsg);

	// search to find if the peer exists in the pool or not
	cPeer = getHostModule()->contains(tpeer);

	// differentiate based on the actual message event field
	switch(amsg->event())
	{
		// started event
		case A_STARTED:

			// insert the peer to the peers pool
			if(cPeer == -1) // do the magic only if the peer does not exist
			{
				cPeer = getHostModule()->peers().add(tpeer);
				getHostModule()->setPeersNum(getHostModule()->peersNum() + 1);
			}
			else // the peer exists, update its fields
			{
				// get the peer
				peer = (BTTrackerStructBase*)getHostModule()->peers()[cPeer];
				// update
				peer->setIpAddress(tpeer->ipAddress());
				peer->setPeerId(tpeer->peerId());
				peer->setPeerPort(tpeer->peerPort());
				peer->setKey(tpeer->key());
				peer->setLastAnnounce(tpeer->lastAnnounce());
				peer->setIsSeed(tpeer->isSeed());
				//Ntinos Katsaros: 22/11/2008
				delete tpeer;
			}

			// valid announce with started event
			return A_VALID_STARTED;
			break;

		// completed event
		case A_COMPLETED:

			// check if the peer is in the peers pool, the peer should have sent an announce with started event before
			if(cPeer == -1)
			{
				// completed event with no started event before from the same peer
				return A_NO_STARTED;
			}

			// ok the peer is in the pool, update his status and proceed like a normal announce

			// get the peer
			peer = (BTTrackerStructBase*)getHostModule()->peers()[cPeer];

			// update the peer's status and the seeds' count only for the first completed event
			if(!peer->isSeed())
			{
				peer->setIsSeed(true);
				getHostModule()->setSeeds(getHostModule()->seeds() + 1);
			}

			// update
			peer->setIpAddress(tpeer->ipAddress());
			peer->setPeerId(tpeer->peerId());
			peer->setPeerPort(tpeer->peerPort());
			peer->setKey(tpeer->key());
			peer->setLastAnnounce(tpeer->lastAnnounce());

			//Ntinos Katsaros: 22/11/2008
			delete tpeer;

			// valid announce with completed event
			return A_VALID_COMPLETED;
			break;

		// normal announce
		case A_NORMAL:

			// check if the peer is in the peers pool, the peer should have sent an announce with started event before
			if(cPeer == -1)
			{
				// normal announce event with no started event before from the same peer
				return A_NO_STARTED;
			}

			// get the peer
			peer = (BTTrackerStructBase*)getHostModule()->peers()[cPeer];

			// update
			peer->setIpAddress(tpeer->ipAddress());
			peer->setPeerId(tpeer->peerId());
			peer->setPeerPort(tpeer->peerPort());
			peer->setKey(tpeer->key());
			peer->setLastAnnounce(tpeer->lastAnnounce());
			//removed by Manoj. We don't need this because it is already initialized as not a seed.
			//removed since this created problems when hiding leachers( sedning only seeders in the reponse)
			//peer->setIsSeed(false);

			//Ntinos Katsaros: 22/11/2008
			delete tpeer;

			// valid normal announce
			return A_VALID_NORMAL;
			break;

		// stopped event
		case A_STOPPED:

			// check if the peer is in the peers pool, the peer should have sent an announce with started event before
			if(cPeer == -1)
			{
				// stopped event with no started event before from the same peer
				return A_NO_STARTED;
			}

			// get the peer
			peer = (BTTrackerStructBase*)getHostModule()->peers()[cPeer];

			// remove him and if the peer was a seeder change the seeds count
			if(peer->isSeed())
			{
				getHostModule()->setSeeds(getHostModule()->seeds() - 1);
			}
			//getHostModule()->peers().remove(cPeer);
			getHostModule()->cleanRemovePeer(cPeer);
			getHostModule()->setPeersNum(getHostModule()->peersNum() - 1);

			//Ntinos Katsaros: 22/11/2008
			delete tpeer;

			// valid announce with stopped event
			return A_VALID_STOPPED;
			break;

		// invalid message event field
		default:

			//Ntinos Katsaros: 22/11/2008
			delete tpeer;

			return A_INVALID_EVENT;
	}
}

/**
 * Send a response to a previous announce.
 * Gets the announce message and the output of the processAnnounce() method.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::sendResponse(int acode, BTTrackerMsgAnnounce* amsg)
{
	// the response message
	BTTrackerMsgResponse* rmsg 	= new BTTrackerMsgResponse();
	// failule happened
	ostringstream fail;
	// warning happened
	bool warning				= false;
	string warning_msg			= "";
	// compact response requested, affects the size of the response
	bool compact				= false;
	// no peer id requested, affects the size of the response
	bool no_peer_id				= amsg->noPeerId();
	// reponse to a started event
	bool started				= false;
	// reponse to a completed event, affects the peers returned
	bool completed				= false;

	// check the validity of the announce and send the appropriate response
	switch(acode)
	{
		// failures, set the failure message/kind and send the reponse
		case A_INVALID_IHASH:
			// build the f*cki*g failure message
			fail << "invalid info hash(info_hash=" << amsg->infoHash() << ")";
			// set the message fields
			rmsg->setFailure(fail.str().c_str());
			rmsg->setKind(R_FAIL);
			break;
		case A_INVALID_PEERID:
			// build the f*ck*ng failure message
			fail << "invalid peer id(peer_id=" << amsg->peerId() << ")";
			// set the message fields
			rmsg->setKind(R_FAIL);
			rmsg->setFailure(fail.str().c_str());
			break;
		case A_INVALID_PORT:
			// build the f*cki*g failure message
			fail << "invalid peer port(peer_port=" << amsg->peerPort() << ")";
			// set the message fields
			rmsg->setKind(R_FAIL);
			rmsg->setFailure(fail.str().c_str());
			break;
		case A_NO_STARTED:
			// set the message fields
			fail << "announce with no started event before";
			rmsg->setKind(R_FAIL);
			rmsg->setFailure(fail.str().c_str());
			break;

		// everything is ok -- send a normal response
		// reponse to a valid announce with started event
		case A_VALID_STARTED:
			// the started event probably did not include a tracker id, set it properly to avoid the sanity checks
			rmsg->setTrackerId(getHostModule()->trackerId().c_str());
			// proceed to a normal announce response but set the a flag to have the rest of the code informed about this
			started = true;
		// reponse to a valid announce with completed event
		case A_VALID_COMPLETED:
			// set the completed flag
			if(!started)
			{
				completed = true;
			}
		// reponse to a valid normal announce
		case A_VALID_NORMAL:
			// compact response requested
			if(amsg->compact())
			{
				// there is support for compact responses
				if(getHostModule()->compactSupport())
				{
					// set the compact flag
					compact = true;
				}
				else // no support for compact responses
				{
					// include a warning to the reponse
					warning = true;
					warning_msg.append("compact response requested but not supported");
				}
			}

			// no tracker id included in the announce or invalid tracker id
			if((amsg->event() != A_STARTED) && (strlen(amsg->trackerId()) == 0 || strcmp(amsg->trackerId(), getHostModule()->trackerId().c_str()) != 0))
			{
				// incldue a warning to the response

				// there is already a warning
				if(warning) // append
				{
					warning_msg.append(", no tracker id included in the announce or invalid tracker id");
				}
				else // new warn
				{
					warning = true;
					warning_msg.append("no tracker id included in the announce or invalid tracker id");
				}
			}

			// reponse to a started event - include the tracker id
			if(started)
			{
				rmsg->setTrackerId(getHostModule()->trackerId().c_str());
			}
			else // normal response
			{
				// always send the tracker id
				if(getHostModule()->alwaysSendTrackerId())
				{
					rmsg->setTrackerId(getHostModule()->trackerId().c_str());
				}
			}

			//TODO -Manoj:: take following three method calls into functions such that
			//they can be overridden in subclasses.

			// set the announce interval
			rmsg->setAnnounceInterval(getHostModule()->announceInterval());
			// set the seeders count
			rmsg->setComplete(getHostModule()->seeds());
			// set the leechers count
			rmsg->setIncomplete(getHostModule()->peersNum() - rmsg->complete());
			// set the peers
			//changed by Manoj. 2015-01-01
			//Pass new argument (frist one) since functino signature has been changed.
			fillPeersInResponse(amsg, rmsg, completed, no_peer_id);
			// set the message kind
			if(warning) // something happened
			{
				rmsg->setKind(R_WARN);
				rmsg->setWarning(warning_msg.c_str());
			}
			else // valid response
			{
				rmsg->setKind(R_VALID);
			}
			break;

		// no response to a stopped event
		case A_VALID_STOPPED:
			// stop
			return;
			break;

		// default dispose
		default:
			// report the error
			getHostModule()->error("%s:%d at %s() invalid announce code passed(acode=%d)\n", __FILE__, __LINE__, __func__, acode);
	}

	// logging
	BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlB::sendResponse", "sending reply to client["<< amsg->peerId()<<", address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]");

	// set the response size
	findAndSetResponseSize(rmsg, compact, no_peer_id);

	//send the response
	getSocket()->send(rmsg);
}

/**
 * Fill the response with peers.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 * function signature changed by Manoj -2015-02-01
 * Added new parameter at the beginning, bcz in subclasses when filling peers,
 *  we need to know whether we are filling peers for a relay hash
 */
void BTTrackerClientHandlerBase::fillPeersInResponse(BTTrackerMsgAnnounce* amsg, BTTrackerMsgResponse* rmsg, bool seed, bool no_peer_id)
{
	// get the peers pool
	cArray& peers 				= getHostModule()->peers();
    BT_LOG_INFO(btLogSinker, "BTTrackerClientHandlerB::fillPeersInResponse",
            "filling peers, current number of available true peers["<< peers.size()<<"]");
	// peers added
	set<int> added_peers			= set<int>();
	// iterator for the added_peers
	set<int>::iterator it;
	// the number of peers in the reply
	size_t max_peers			= 0;
	// temporary peer from the peers pool
	BTTrackerStructBase* tpeer;
	// temporary peer to add to the response
	PEER ttpeer;
	// random peer
	int rndpeer				= -1;

	if(seed) // response to a seed
	{
		// how many peers we have to add, -1 to exclude the requesting peer
		if(getHostModule()->peersNum() - getHostModule()->seeds() > 1)
		{
			max_peers = (getHostModule()->peersNum() - getHostModule()->seeds() <= getHostModule()->maxPeersInReply()) ? getHostModule()->peersNum() - getHostModule()->seeds(): getHostModule()->maxPeersInReply();
		}
		else
		{
			return;
		}
	}
	else // response to a normal peer
	{
		// how many peers we have to add, -1 to exclude the requesting peer
		if(peers.size() > 1)
		{
			max_peers = (getHostModule()->peersNum() - 1 <= getHostModule()->maxPeersInReply()) ? getHostModule()->peersNum() - 1 : getHostModule()->maxPeersInReply();
		}
		else // no available peers
		{
			return;
		}
	}

	// random selection
	while(added_peers.size() < max_peers)
	{
		// get a random peer
	    rndpeer = intrand(peers.size());

//	    BT_LOG_DEBUG(btLogSinker, "BTTrackerClientHandlerB::fillPeersInResponse",
//	            "filling peers, peer ID ["<<amsg->peerId()<<"] index is ["<< rndpeer<<"]");

		// the random peer is the peer that made the announce, ignore
		if(rndpeer == cPeer)
		{
			continue;
		}
		// the random peer is already added, ignore
		if(added_peers.find(rndpeer) != added_peers.end())
		{
			continue;
		}
		// the response is returned to a seed (i.e., do not include seeds in the response)
		if(seed && (BTTrackerStructBase*)peers[rndpeer] && ((BTTrackerStructBase*)peers[rndpeer])->isSeed())
		{
			continue;
		}

		// add the peer to the "added peers" pool
		if((BTTrackerStructBase*)peers[rndpeer])
		{
			added_peers.insert(rndpeer);
		}
//        BT_LOG_DEBUG(btLogSinker, "BTTrackerClientHandlerB::fillPeersInResponse",
//                "filling peers, peer ID ["<<amsg->peerId()<<"] added index is ["<< rndpeer<<"]");
	}

	// traverse the set and fill the response
	rmsg->setPeersArraySize(added_peers.size());
	for(it = added_peers.begin(); it != added_peers.end(); it++)
	{
		// get the peer from the pool
		tpeer = (BTTrackerStructBase*)peers[*it];

		// copy some fields/values
		if(!no_peer_id)
		{
			ttpeer.peerId 		= tpeer->peerId().c_str();
		}
		ttpeer.peerPort		= tpeer->peerPort();
		ttpeer.ipAddress 	= tpeer->ipAddress();

		// insert the peer to the response
		rmsg->setPeers(--max_peers, ttpeer);
	}
}

/**
 * Set the size of the response message.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::findAndSetResponseSize(BTTrackerMsgResponse* rmsg, bool compact, bool no_peer_id)
{
	// string length counter
	size_t stlen 	= 0;
	// total bytes of the message
	size_t total	= 0;
	// counter
	size_t i;
	// temporary peer
	PEER tpeer;

	// differentiate based on the actual reply type
	switch(rmsg->getKind())
	{
		// only the failure string has to be included
		case R_FAIL:
			// count the length of the string
			stlen = strlen(rmsg->failure());
			// total size: bytes of the failure string + 1 (char ":") + the bytes of the length count as string (i.e, 30 = 2 bytes) - bencoded
			total += (sizeOfStrInt(stlen) + 1 + stlen);
			// calc the size of the failure key in the dictionary + 2 chars for the dictionary it self
			total += FAIL_PADD; // e.g., d14:failure reason12:failure teste --> key: failure reason, value: failure test
			break;

		// like failure, but proceed afterwards with normal reply
		case R_WARN:
			// count the length of the string
			stlen = strlen(rmsg->warning());
			// total size
			total += (sizeOfStrInt(stlen) + 1 + stlen);
			// calc the size of the warning key in the dictionary
			total += WARN_PADD; // e.g., 15:warning message9:warn test --> key: warning message, value: warn test
		case R_VALID:
			// interval field, "i" + announce interval + "e" - bencoded integer
			total += (sizeOfStrInt(rmsg->announceInterval()) + INT_PADD);
			// tracker id field, count the length of the string (only if it is set) - bencoded string
			if((stlen = strlen(rmsg->trackerId())) > 0)
			{
				total += (sizeOfStrInt(stlen) + 1 + stlen);
			}
			// complete field - bencoded integer
			total += (sizeOfStrInt(rmsg->complete()) + INT_PADD);
			// incomplete field - bencoded integer
			total += (sizeOfStrInt(rmsg->incomplete()) + INT_PADD);
			// compact reply
			if(compact)
			{
				// the peers are encoded as a large string
				stlen = rmsg->peersArraySize() * CMPCT_PEER;
				total += (sizeOfStrInt(stlen) + 1 + stlen);
			}
			else // normal reply, the peers are bencoded as a list of dictionaries
			{
				// calc the list
				total += LIST_PADD; // i.e, l and e chars

				// traverse the peers array
				for(i=0; i<rmsg->peersArraySize(); i++)
				{
					// calc the subdictionary
					total += DICT_PADD; // i.e, d and e chars

					// get the temp peer
					tpeer = rmsg->peers(i);
					// no_peer_id reply handling
					if(!no_peer_id)
					{
						// peer id - bencoded string
						stlen = strlen(tpeer.peerId.c_str());
						total += (sizeOfStrInt(stlen) + 1 + stlen);
						total += PEERID_PADD; // i.e., peer id key
					}
					// ip address - bencoded string
					stlen = tpeer.ipAddress.str().size();
					total += (sizeOfStrInt(stlen) + 1 + stlen);
					total += IPADDR_PADD; // i.e., ip key
					// port - bencoded
					total += (sizeOfStrInt(tpeer.peerPort) + INT_PADD);
					total += PORT_PADD; // i.e., port key
				}
			}
			// calc the size of the dictionary
			total += RDICT_PADD; // i.e, interval, tracker id, complete, incomplete and peers keys are bencoded strings + 2 chars for the dictionary
			break;

		// invalid reply type
		default:
			// report the error
			getHostModule()->error("%s:%d at %s() invalid repy type in response(REPLY_TYPE=%d)\n", __FILE__, __LINE__, __func__, rmsg->getKind());
	}

	// set the actual size
	string httpHeader = "Content-Type: text/plain; \n\n";
	rmsg->setByteLength((httpHeader.length() * sizeof(char)) + total);
	//rmsg->setByteLength(total);
}

/**
 * Get the size of an integer when being in ASCII form (i.e., 10 = 2 bytes, 100 = 3 bytes).
 */
size_t BTTrackerClientHandlerBase::sizeOfStrInt(size_t uint) const
{
	// digit counter
	size_t dgts = 0;

	// lame count
	while(uint > 0)
	{
		uint /= 10;
		dgts++;
	}

	// return the counted digits
	return dgts;
}

//added by Manoj - 2015-06-06.
//to achieve a polymophic behaviour for BTTrackerStructBase object creation
BTTrackerStructBase * BTTrackerClientHandlerBase::createTrackerStructObj(BTTrackerMsgAnnounce* amsg)
{

    BTTrackerStructBase * pRet = new BTTrackerStructBase(IPvXAddress(getSocket()->getRemoteAddress()), string(amsg->peerId()),
            amsg->peerPort(), string(amsg->key()), simTime(), (amsg->event() == A_COMPLETED) ? true : false);

    return pRet;
}

/**
 * Initializes the session variables and performs some logging.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::established()
{
	// start the session timer
	scheduleAt(simTime() + (simtime_t)getHostModule()->sessionTimeout(), evtTout);

	// logging
	BT_LOG_DEBUG(btLogSinker, "BTTrackerClntHndlB::established", "connection with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established");
	BT_LOG_DEBUG(btLogSinker, "BTTrackerClntHndlB::established", "starting session timer[" << getHostModule()->sessionTimeout() << " secs] for client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]");

	// change state
	state_var 	= ESTABLISHED;

	// reset the current peer pointer
	cPeer 		= -1;
}

/**
 * Handles the reception of an announce.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::dataArrived(cMessage* msg, bool urgent)
{
	// logging
	BT_LOG_DEBUG(btLogSinker, "BTTrackerClntHndlB::dataArrived", "announce received from client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]");

	// check the message type
	BTTrackerMsgAnnounce* mmsg = dynamic_cast<BTTrackerMsgAnnounce*>(msg);

	// the processAnnounce() response
	int res = -1;

	if(!mmsg) // inalid message
	{
		// report the error
		getHostModule()->error("%s:%d at %s() invalid message, should be of BTTrackerMsgAnnounce type\n", __FILE__, __LINE__, __func__);
	}

	// process the message and send a reply
	if((res = processAnnounce(mmsg)) != A_INVALID_EVENT)
	{
		// call sendResponse() to send back something
		sendResponse(res, mmsg);
	}
	else
	{
		// received message with invalid event field
		// report the error
		getHostModule()->error("%s:%d at %s() invalid announce event(event=%d)\n", __FILE__, __LINE__, __func__, mmsg->event());
	}

	// change state
	state_var = TEARDOWN;

	// cleanup
	delete msg;
}

/**
 * Handles a session timeout event.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::timerExpired(cMessage* msg)
{
	// timer expired while waiting

	// logging
	BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlB::timerExpired", "session with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] expired");
	// perform close()
	getSocket()->close();
}

/**
 * Handles the teardown of the thread.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTTrackerClientHandlerBase::peerClosed()
{

	// different actions for different states
	switch(state_var)
	{
		// preliminary close
		case ESTABLISHED:
			// logging
			BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlB::peerClosed", "session with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] died unexpectedly");
			// fire the session timer
			cancelEvent(evtTout);
			scheduleAt(simTime(), evtTout);
			break;

		// normal close
		case TEARDOWN:
			// logging
			BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlB::peerClosed", "session with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] ended");
			// cancel the session timer
			cancelEvent(evtTout);
			// default handler
			TCPServerThreadBase::peerClosed();
			break;

		// bad, bad, bad...
		case PENDING:
		default:
			// report the error
			getHostModule()->error("%s:%d at %s() invalid state occured(state_var=%d)\n", __FILE__, __LINE__, __func__, state_var);
        }
}

/**
 * Get the state of the session/thread.
 */
size_t BTTrackerClientHandlerBase::tstate() const
{
	return state_var;
}

/**
 * Set the state of the session/thread.
 */
void BTTrackerClientHandlerBase::setTstate(size_t state)
{
	state_var = state;
}

/**
 * Get the host getModule(i.e., the instance of BTTrackerBase which spawns the thread).
 */
BTTrackerBase* BTTrackerClientHandlerBase::getHostModule()
{
	// get the host module and check its type
	BTTrackerBase* hostMod = dynamic_cast<BTTrackerBase*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/**
 * Constructor
 */
BTTrackerStructBase::BTTrackerStructBase(const IPvXAddress& ipAddress, const string& peerId, size_t peerPort, const string& key, simtime_t lastAnnounce, bool isSeed)
{
	ipAddress_var 		= ipAddress;
	peerId_var		= peerId;
	peerPort_var		= peerPort;
	key_var			= key;
	lastAnnounce_var	= lastAnnounce;
	isSeed_var		= isSeed;
}

/**
 * Destructor
 */
BTTrackerStructBase::~BTTrackerStructBase()
{
}

/**
 * Operator ==.
 * Two entries are considered equal if the have the same key or if they have the same IP address and peer id.
 */
bool BTTrackerStructBase::operator==(const BTTrackerStructBase& rhs)
{
	// the key value is set, compare with this
	if(rhs.key_var != "")
	{
		return (key_var == rhs.key_var) ? true : false;
	}
	else // the key value is not set, do the comparison with the rest
	{
		return (ipAddress_var == rhs.ipAddress_var && peerId_var == rhs.peerId_var) ? true : false;
	}
}

/**
 * Get the IP address.
 */
const IPvXAddress& BTTrackerStructBase::ipAddress() const
{
	return ipAddress_var;
}

/**
 * Set the IP address.
 */
void BTTrackerStructBase::setIpAddress(const IPvXAddress& ipAddress)
{
	ipAddress_var = ipAddress;
}

/**
 * Get the peer id.
 */
const string& BTTrackerStructBase::peerId() const
{
	return peerId_var;
}

/**
 * Set the peer id.
 */
void BTTrackerStructBase::setPeerId(const string& peerId)
{
	peerId_var = peerId;
}

/**
 * Get the peer port.
 */
size_t BTTrackerStructBase::peerPort() const
{
	return peerPort_var;
}

/**
 * Set the peer port.
 */
void BTTrackerStructBase::setPeerPort(size_t peerPort)
{
	peerPort_var = peerPort;
}

/**
 * Get the key.
 */
const string& BTTrackerStructBase::key() const
{
	return key_var;
}

/**
 * Set the key.
 */
void BTTrackerStructBase::setKey(const string& key)
{
	key_var = key;
}

/**
 * Get the last announce.
 */
simtime_t BTTrackerStructBase::lastAnnounce() const
{
	return lastAnnounce_var;
}

/**
 * Set the last announce.
 */
void BTTrackerStructBase::setLastAnnounce(simtime_t lastAnnounce)
{
	lastAnnounce_var = lastAnnounce;
}

/**
 * Get the value of the isSeed flag.
 */
bool BTTrackerStructBase::isSeed() const
{
	return isSeed_var;
}

/**
 * Set the value of the isSeed flag.
 */
void BTTrackerStructBase::setIsSeed(bool isSeed)
{
	isSeed_var = isSeed;
}
