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
    realyIfoHash_var        =  par("realyInfoHash").stdstringValue ();

}



/**
 * Get the relay info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
const string& BTTrackerRelayEnabled::relayInfoHash() const
{
    return realyIfoHash_var;
}

/**
 * Set the relay info hash of tracker.
 * This is the info hash of the .torrent file which the tracker monitors.
 */
void BTTrackerRelayEnabled::setRelayInfoHash(const string& infoHash)
{
    realyIfoHash_var = infoHash;
}

