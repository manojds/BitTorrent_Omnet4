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

#include "BTTrackerClientHandlerRelayEnbled.h"
#include "BTLogImpl.h"

Register_Class(BTTrackerClientHandlerRelayEnbled);

BTTrackerClientHandlerRelayEnbled::BTTrackerClientHandlerRelayEnbled() {
    // TODO Auto-generated constructor stub

}

BTTrackerClientHandlerRelayEnbled::~BTTrackerClientHandlerRelayEnbled() {
    // TODO Auto-generated destructor stub
}

BTTrackerRelayEnabled* BTTrackerClientHandlerRelayEnbled::getHostModule()
{
    // get the host module and check its type
    BTTrackerRelayEnabled* hostMod = check_and_cast<BTTrackerRelayEnabled*>(TCPServerThreadBase::getHostModule());

    // return the correct module
    return hostMod;
}


/**
 * Handle an announce.
 * Returns the status of the process (i.e., a constant value which indicates what should the tracker reply with).
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
int BTTrackerClientHandlerRelayEnbled::processAnnounce(BTTrackerMsgAnnounce* amsg)
{
    //we act only if the info hash is equal to relay info hash
    //otherwise we let super class to handle it
    if(amsg->infoHash() == getHostModule()->relayInfoHash())
    {
        BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlRB::processAnnounce", "Announce request for relay hash from client[address="
                << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established");

        // temporary peer struct with the announce info
        BTTrackerStructBase* tpeer = NULL;
        // a peer from the pool
        BTTrackerStructBase* peer = NULL;
        // the position of the peer in the pool
        cPeer = -1;

        // sanity checks - failures

        // no peer id
        if (strlen(amsg->peerId()) == 0) {
            return A_INVALID_PEERID;
        }
        // invalid port
        if (amsg->peerPort() == 0) {
            return A_INVALID_PORT;
        }

        // init the temp peer struct
        tpeer = new BTTrackerStructBase(
                IPvXAddress(getSocket()->getRemoteAddress()),
                string(amsg->peerId()), amsg->peerPort(), string(amsg->key()),
                simTime(), (amsg->event() == A_COMPLETED) ? true : false);

        // search to find if the peer exists in the pool or not
        cPeer = getHostModule()->containsRelay(tpeer);

        // differentiate based on the actual message event field
        switch (amsg->event()) {
        // started event
        case A_STARTED:

            // insert the peer to the peers pool
            if (cPeer == -1) // do the magic only if the peer does not exist
                    {
                cPeer = getHostModule()->relayPeers().add(tpeer);
                getHostModule()->setRealyPeersNum(getHostModule()->realyPeersNum() + 1);
            } else // the peer exists, update its fields
            {
                // get the peer
                peer = (BTTrackerStructBase*) getHostModule()->relayPeers()[cPeer];
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
            if (cPeer == -1) {
                // completed event with no started event before from the same peer
                return A_NO_STARTED;
            }

            // ok the peer is in the pool, update his status and proceed like a normal announce

            // get the peer
            peer = (BTTrackerStructBase*) getHostModule()->relayPeers()[cPeer];

            // update the peer's status and the seeds' count only for the first completed event
            if (!peer->isSeed()) {
                peer->setIsSeed(true);
                //Commented by MAnoj . 2015-01-25
                // TODO :: uncomment this and correct the behavior if relays
                //can be seeders with respect to relay hash.
                //getHostModule()->setSeeds(getHostModule()->seeds() + 1);
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
            if (cPeer == -1) {
                // normal announce event with no started event before from the same peer
                return A_NO_STARTED;
            }

            // get the peer
            peer = (BTTrackerStructBase*) getHostModule()->relayPeers()[cPeer];

            // update
            peer->setIpAddress(tpeer->ipAddress());
            peer->setPeerId(tpeer->peerId());
            peer->setPeerPort(tpeer->peerPort());
            peer->setKey(tpeer->key());
            peer->setLastAnnounce(tpeer->lastAnnounce());
            peer->setIsSeed(false);

            //Ntinos Katsaros: 22/11/2008
            delete tpeer;

            // valid normal announce
            return A_VALID_NORMAL;
            break;

            // stopped event
        case A_STOPPED:

            // check if the peer is in the peers pool, the peer should have sent an announce with started event before
            if (cPeer == -1) {
                // stopped event with no started event before from the same peer
                return A_NO_STARTED;
            }

            // get the peer
            peer = (BTTrackerStructBase*) getHostModule()->relayPeers()[cPeer];

            // remove him and if the peer was a seeder change the seeds count

            //Commented by MAnoj . 2015-01-25
            // TODO :: uncomment this and correct the behavior if relays
            //can be seeders with respect to relay hash.

//            if (peer->isSeed()) {
//                getHostModule()->setSeeds(getHostModule()->seeds() - 1);
//            }
            //end of the commented block by MAnoj.

            //getHostModule()->peers().remove(cPeer);
            getHostModule()->cleanRemovePeer(cPeer);
            getHostModule()->setRealyPeersNum(getHostModule()->realyPeersNum() - 1);

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
    else
    {
        return BTTrackerClientHandlerBase::processAnnounce( amsg);
    }
}

/**
 * Fill the response with peers.
 */
void BTTrackerClientHandlerRelayEnbled::fillPeersInResponse(BTTrackerMsgResponse* rmsg, bool seed, bool no_peer_id)
{
    // get the peers pool
    cArray& peers               = getHostModule()->peers();
    // peers added
    set<int> added_peers            = set<int>();
    // iterator for the added_peers
    set<int>::iterator it;
    // the number of peers in the reply
    size_t max_peers            = 0;
    // temporary peer from the peers pool
    BTTrackerStructBase* tpeer;
    // temporary peer to add to the response
    PEER ttpeer;
    // random peer
    int rndpeer             = -1;

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

    //calculate the count of relay peers.
//    size_t relayPeerCount=(getHostModule()->relayPeerPropotionInReply())*();

    // random selection
    while(added_peers.size() < max_peers)
    {
        // get a random peer
        rndpeer = intrand(peers.size());

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
            ttpeer.peerId       = tpeer->peerId().c_str();
        }
        ttpeer.peerPort     = tpeer->peerPort();
        ttpeer.ipAddress    = tpeer->ipAddress();

        // insert the peer to the response
        rmsg->setPeers(--max_peers, ttpeer);
    }

    //TODO : this was added temporarily.
    // remove this and add relay peers according to the proportion


    cArray& relayPeers=getHostModule()->relayPeers();
    int iTruePeerMark=added_peers.size();
    rmsg->setPeersArraySize(added_peers.size()+relayPeers.size());

    for(int i=0; i <relayPeers.size(); i++)
    {
        // get the peer from the pool
        tpeer = (BTTrackerStructBase*)relayPeers[i];

        // copy some fields/values
        if(!no_peer_id)
        {
            ttpeer.peerId       = tpeer->peerId().c_str();
        }
        ttpeer.peerPort     = tpeer->peerPort();
        ttpeer.ipAddress    = tpeer->ipAddress();

        // insert the peer to the response
        rmsg->setPeers(iTruePeerMark+i, ttpeer);
    }
}



