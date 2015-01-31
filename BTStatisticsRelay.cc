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

#include "BTStatisticsRelay.h"
#include "BTLogImpl.h"



Define_Module(BTStatisticsRelay);

BTStatisticsRelay::BTStatisticsRelay() {
    // TODO Auto-generated constructor stub

}

BTStatisticsRelay::~BTStatisticsRelay() {
    // TODO Auto-generated destructor stub
}

void BTStatisticsRelay::handleMessage(cMessage* msg)
{
    switch (msg->getKind())
    {
        case BT_STATS_RELAY_DWL:
        {
            BTStatisticsDWLMsg* dwMsg = dynamic_cast<BTStatisticsDWLMsg*>(msg);
            double dwTime = dwMsg->downloadTime();
            double rmBlocks = dwMsg->remainingBlocks();
            if (rmBlocks == 0 )
            {
                dwSuccess_Relay->collect(dwTime);
                dwSuccess_Relay_vec.record(dwTime);
            }
            else
            {
                numBlockFail_Relay->collect(rmBlocks);
                numBlockFail_Relay_vec.record(rmBlocks);
            }
            delete msg;
            break;
        }

        default:
        {
            BTStatistics::handleMessage(msg);
            break;
        }
    }

}

void BTStatisticsRelay::finish()
{
    BT_LOG_INFO(btLogSinker,"BTStatisticsRelay::doFinish","Finishing ...");

    dwSuccess_Relay->record();
    numBlockFail_Relay->record();
}
