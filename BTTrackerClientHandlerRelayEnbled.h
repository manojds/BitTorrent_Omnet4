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

#ifndef BTTRACKERCLIENTHANDLERRELAYENBLED_H_
#define BTTRACKERCLIENTHANDLERRELAYENBLED_H_
#include "BTTrackerRelayEnabled.h"

class INET_API BTTrackerClientHandlerRelayEnbled :public BTTrackerClientHandlerBase {
public:
    BTTrackerClientHandlerRelayEnbled();
    virtual ~BTTrackerClientHandlerRelayEnbled();

    BTTrackerRelayEnabled* getHostModule();

protected:
    // overrides from BTTrackerClientHandlerBase
    virtual int processAnnounce(BTTrackerMsgAnnounce*);
    virtual void fillPeersInResponse(BTTrackerMsgAnnounce* amsg, BTTrackerMsgResponse*, bool, bool);    // selects the peers which will included in the response
    // end of overrides from BTTrackerClientHandlerBase


};

#endif /* BTTRACKERCLIENTHANDLERRELAYENBLED_H_ */
