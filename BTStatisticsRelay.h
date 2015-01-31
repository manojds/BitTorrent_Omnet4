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

#ifndef BTSTATISTICSRELAY_H_
#define BTSTATISTICSRELAY_H_
#include "BTStatistics.h"


/*!
 * class which collects statistics of BTTorrent Relays Hosts
 */
class INET_API BTStatisticsRelay :public BTStatistics
{
public:
    BTStatisticsRelay();
    virtual ~BTStatisticsRelay();

    virtual void handleMessage(cMessage* msg);

    virtual void finish();

protected:
    cStdDev*    dwSuccess_Relay;
    cOutVector  dwSuccess_Relay_vec;

    cStdDev*    numBlockFail_Relay;
    cOutVector  numBlockFail_Relay_vec;
};

#endif /* BTSTATISTICSRELAY_H_ */
