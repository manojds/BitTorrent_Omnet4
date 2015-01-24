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


Define_Module(BTTrackerClientHandlerRelayEnbled);
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
        // TODO ::this switch case added temporary.
        //remove this and modify commented block accordingly
        switch (amsg->event())
        {
        case A_STARTED:
            return A_VALID_STARTED;
            break;

        case A_COMPLETED:
            return A_VALID_COMPLETED;
            break;

        case A_NORMAL:
            return A_VALID_NORMAL;
            break;

        case A_STOPPED:
            return A_VALID_STOPPED;
            break;
        default:
            return A_INVALID_EVENT;
            break;


        }
//        // temporary peer struct with the announce info
//        BTTrackerStructBase* tpeer = NULL;
//        // a peer from the pool
//        BTTrackerStructBase* peer = NULL;
//        // the position of the peer in the pool
//        cPeer = -1;
//
//        // sanity checks - failures
//
//        // no peer id
//        if (strlen(amsg->peerId()) == 0) {
//            return A_INVALID_PEERID;
//        }
//        // invalid port
//        if (amsg->peerPort() == 0) {
//            return A_INVALID_PORT;
//        }
//
//        // init the temp peer struct
//        tpeer = new BTTrackerStructBase(
//                IPvXAddress(getSocket()->getRemoteAddress()),
//                string(amsg->peerId()), amsg->peerPort(), string(amsg->key()),
//                simTime(), (amsg->event() == A_COMPLETED) ? true : false);
//
//        // search to find if the peer exists in the pool or not
//        cPeer = getHostModule()->contains(tpeer);
//
//        // differentiate based on the actual message event field
//        switch (amsg->event()) {
//        // started event
//        case A_STARTED:
//
//            // insert the peer to the peers pool
//            if (cPeer == -1) // do the magic only if the peer does not exist
//                    {
//                cPeer = getHostModule()->peers().add(tpeer);
//                getHostModule()->setPeersNum(getHostModule()->peersNum() + 1);
//            } else // the peer exists, update its fields
//            {
//                // get the peer
//                peer = (BTTrackerStructBase*) getHostModule()->peers()[cPeer];
//                // update
//                peer->setIpAddress(tpeer->ipAddress());
//                peer->setPeerId(tpeer->peerId());
//                peer->setPeerPort(tpeer->peerPort());
//                peer->setKey(tpeer->key());
//                peer->setLastAnnounce(tpeer->lastAnnounce());
//                peer->setIsSeed(tpeer->isSeed());
//                //Ntinos Katsaros: 22/11/2008
//                delete tpeer;
//            }
//
//            // valid announce with started event
//            return A_VALID_STARTED;
//            break;
//
//            // completed event
//        case A_COMPLETED:
//
//            // check if the peer is in the peers pool, the peer should have sent an announce with started event before
//            if (cPeer == -1) {
//                // completed event with no started event before from the same peer
//                return A_NO_STARTED;
//            }
//
//            // ok the peer is in the pool, update his status and proceed like a normal announce
//
//            // get the peer
//            peer = (BTTrackerStructBase*) getHostModule()->peers()[cPeer];
//
//            // update the peer's status and the seeds' count only for the first completed event
//            if (!peer->isSeed()) {
//                peer->setIsSeed(true);
//                getHostModule()->setSeeds(getHostModule()->seeds() + 1);
//            }
//
//            // update
//            peer->setIpAddress(tpeer->ipAddress());
//            peer->setPeerId(tpeer->peerId());
//            peer->setPeerPort(tpeer->peerPort());
//            peer->setKey(tpeer->key());
//            peer->setLastAnnounce(tpeer->lastAnnounce());
//
//            //Ntinos Katsaros: 22/11/2008
//            delete tpeer;
//
//            // valid announce with completed event
//            return A_VALID_COMPLETED;
//            break;
//
//            // normal announce
//        case A_NORMAL:
//
//            // check if the peer is in the peers pool, the peer should have sent an announce with started event before
//            if (cPeer == -1) {
//                // normal announce event with no started event before from the same peer
//                return A_NO_STARTED;
//            }
//
//            // get the peer
//            peer = (BTTrackerStructBase*) getHostModule()->peers()[cPeer];
//
//            // update
//            peer->setIpAddress(tpeer->ipAddress());
//            peer->setPeerId(tpeer->peerId());
//            peer->setPeerPort(tpeer->peerPort());
//            peer->setKey(tpeer->key());
//            peer->setLastAnnounce(tpeer->lastAnnounce());
//            peer->setIsSeed(false);
//
//            //Ntinos Katsaros: 22/11/2008
//            delete tpeer;
//
//            // valid normal announce
//            return A_VALID_NORMAL;
//            break;
//
//            // stopped event
//        case A_STOPPED:
//
//            // check if the peer is in the peers pool, the peer should have sent an announce with started event before
//            if (cPeer == -1) {
//                // stopped event with no started event before from the same peer
//                return A_NO_STARTED;
//            }
//
//            // get the peer
//            peer = (BTTrackerStructBase*) getHostModule()->peers()[cPeer];
//
//            // remove him and if the peer was a seeder change the seeds count
//            if (peer->isSeed()) {
//                getHostModule()->setSeeds(getHostModule()->seeds() - 1);
//            }
//            //getHostModule()->peers().remove(cPeer);
//            getHostModule()->cleanRemovePeer(cPeer);
//            getHostModule()->setPeersNum(getHostModule()->peersNum() - 1);
//
//            //Ntinos Katsaros: 22/11/2008
//            delete tpeer;
//
//            // valid announce with stopped event
//            return A_VALID_STOPPED;
//            break;
//
//            // invalid message event field
//        default:
//
//            //Ntinos Katsaros: 22/11/2008
//            delete tpeer;
//
//            return A_INVALID_EVENT;
//        }
    }
    else
    {
        return BTTrackerClientHandlerBase::processAnnounce( amsg);
    }
}
