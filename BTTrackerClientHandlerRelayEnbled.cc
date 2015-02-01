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
    //mjp
    // return the correct module
    return hostMod;
}


/**
 * Handle an announce for a relay Hash.
 * Returns the status of the process (i.e., a constant value which indicates what should the tracker reply with).
 */
int BTTrackerClientHandlerRelayEnbled::processAnnounce(BTTrackerMsgAnnounce* amsg)
{
    //we act only if the info hash is equal to relay info hash
    //otherwise we let super class to handle it
    if(amsg->infoHash() == getHostModule()->relayInfoHash())
    {
        BT_LOG_INFO(btLogSinker, "BTTrackerClntHndlRE::processAnnounce", "Announce request for relay hash from client[address="
                << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] with event ["<<amsg->event()<<"]");

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
                getHostModule()->setRealyPeersNum(
                        getHostModule()->realyPeersNum() + 1);
            }
            else // the peer exists, update its fields
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
            getHostModule()->cleanRemoveRelayPeer(cPeer);
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
 * for true hash
 */
void BTTrackerClientHandlerRelayEnbled::fillPeersInResponse(BTTrackerMsgAnnounce* amsg, BTTrackerMsgResponse* rmsg, bool seed, bool no_peer_id)
{
    //if it is relay hash we don't fill peers.
    if(strcmp(amsg->infoHash(), getHostModule()->relayInfoHash().c_str()) == 0)
    {
        BT_LOG_DETAIL(btLogSinker, "BTTrackerClntHndlRE::fillPeersInResponse", "Avoiding filling peers for relay hash announce. "
                "Client details [address="<< getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]");
        return;
    }

    BTTrackerClientHandlerBase::fillPeersInResponse(amsg, rmsg, seed, no_peer_id);

    int iTruePeerCount=rmsg->peersArraySize();

    cArray& relayPeers=getHostModule()->relayPeers();

    BT_LOG_DEBUG(btLogSinker, "BTTrackerClntHndlRE::fillPeersInResponse",
            "filling peers, number of true peers in response ["<< iTruePeerCount<<
            "], number of available relay peers ["<<relayPeers.size()<<"]");



    // temporary peer from the peers pool
    BTTrackerStructBase* tpeer;
    // temporary peer to add to the response
    PEER ttpeer;

    // peers added
    set<int> added_peers            = set<int>();

    //TODO : this was added temporarily.
    // remove this and add relay peers according to the proportion
    //currently we add at most same number of relay peers as true peers.


    int iMaxRelayPeers(0);
    if( iTruePeerCount < relayPeers.size())
        iMaxRelayPeers=iTruePeerCount;
    else
        iMaxRelayPeers= relayPeers.size();

    for (int i=0; (added_peers.size() < iMaxRelayPeers) && (i < relayPeers.size()) ; i++ )
    {
        //This announcing peer is also could be a relay peer.
        //So this particular index may be give us the same peer from relay peer array.
        //We should not add the same peer requesting in the response
        if(amsg->peerId()  == ((BTTrackerStructBase*)relayPeers[i])->peerId())
            continue;

        //if there is peer at this index add it
        if(relayPeers[i] != NULL)
            added_peers.insert(i);
    }



    rmsg->setPeersArraySize(iTruePeerCount+added_peers.size());

    set<int>::iterator it= added_peers.begin();
    int i=0;
    for( ; it != added_peers.end(); it++, i++)
    {
        // get the peer from the pool
        tpeer = (BTTrackerStructBase*)relayPeers[*it];


        // copy some fields/values
        if(!no_peer_id)
        {
            ttpeer.peerId       = tpeer->peerId().c_str();
        }
        ttpeer.peerPort     = tpeer->peerPort();
        ttpeer.ipAddress    = tpeer->ipAddress();

        // insert the peer to the response
        rmsg->setPeers(iTruePeerCount+i, ttpeer);
    }
}



