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

#include "BTTrackerRelayEnabled.h"

Define_Module(BTTrackerRelayEnabled);

BTTrackerRelayEnabled::BTTrackerRelayEnabled() {
    // TODO Auto-generated constructor stub

}

BTTrackerRelayEnabled::~BTTrackerRelayEnabled() {
    // TODO Auto-generated destructor stub
}

void BTTrackerRelayEnabled::initialize()
{
    BTTrackerBase::initialize();
    relayPeerPropotionInReply_var   = (double)par("relayPeerPropotionInReply");
    realyIfoHash                    =  par("realyInfoHash").stdstringValue ();


    realyPeersNum_var   = 0;

    WATCH(relayPeerPropotionInReply_var);
    WATCH(realyPeersNum_var);
    WATCH_OBJ(relayPeers_var);
}

/**
 * Returns the position of obj inside the relay peers container (if it is contained)
 * or -1 if obj is not found.
 */
int BTTrackerRelayEnabled::containsRelay(BTTrackerStructBase* obj) const
{
    // temp peer
    BTTrackerStructBase* tpeer;

    // traverse the peers pool to find obj
    for(int i=0; i<relayPeers_var.size(); i++)
    {
        // get peer i
        tpeer = (BTTrackerStructBase*)relayPeers_var[i];
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
 * Get the maximum number of peers which can be included in a response.
 */
double BTTrackerRelayEnabled::relayPeerPropotionInReply() const
{
    return relayPeerPropotionInReply_var;
}

/**
 * Set the maximum number of peers which can be included in a response.
 */
void BTTrackerRelayEnabled::setRelayPeerPropotionInReply(double relayPeerPropotionInReply)
{
    relayPeerPropotionInReply_var = relayPeerPropotionInReply;
}

/**
 * Get the relay peers count.
 */
size_t BTTrackerRelayEnabled::realyPeersNum() const
{
    return realyPeersNum_var;
}

/**
 * Set the relay peers count.
 */
void BTTrackerRelayEnabled::setRealyPeersNum(size_t peersNum)
{
    realyPeersNum_var = peersNum;
}

/**
 * Get the relay info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
const string& BTTrackerRelayEnabled::relayInfoHash() const
{
    return realyIfoHash;
}

/**
 * Set the relay info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
void BTTrackerRelayEnabled::setRelayInfoHash(const string& infoHash)
{
    realyIfoHash = infoHash;
}

/**
 * Get the relay peers container.
 */
cArray& BTTrackerRelayEnabled::relayPeers()
{
    return relayPeers_var;
}
