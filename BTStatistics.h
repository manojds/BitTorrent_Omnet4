//
// Copyright 2009 Konstantinos V. Katsaros
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


#ifndef __BTStatistics_H__
#define __BTStatistics_H__


#include <omnetpp.h>
#include <map>
#include <string>
#include "BTStatisticsMsg_m.h"
//edited by Manoj - 2014-12-22
//# define BT_STATS_DWL			59760
//# define BT_STATS_PP			59761
//# define BT_STATS_NSB			59762
//# define BT_STATS_EXIT			59763

# define BT_STATS_DWL			760
# define BT_STATS_PP			761
# define BT_STATS_NSB			762
# define BT_STATS_EXIT			763
# define BT_PER_PEER_STAT       766
//end of the edited code

# define BT_STATS_MSG_TIME 		20000

using namespace std;

class PerPeerStatItem
{
public:
    unsigned int ui_BlockCount;
    unsigned int ui_PieceCount;
    double       d_LastDwlTime;
};

/**
 * Module that calculates the multicast groups sizes and randomly selects their participants (receivers and sender).
 * It is also used for global statistics measurement such as stretch.
 *
 * @author Konstantinos Katsaros
 */

class INET_API BTStatistics : public cSimpleModule
{
	public:
    BTStatistics();
    /**
     * Destructor
     */
    virtual ~BTStatistics();

    void updatePerPeerBlockCount(const string & _sPeerID, unsigned int _uiDownloadedBlocks, unsigned int _uiTotalBlocks ,
                                            unsigned int _uiDownloadedPieces, unsigned int _uiTotalPieces, double _dLastDwlTime);

    static void fillFileNameWithTimeStamp(const char * _pFileNameIn, char * _pFileNameOut);

protected:
    int currentTerminalNum;
    int targetOverlayTerminalNum;
    //Added by Manoj, 2015-04-17. to be able to control the simulation time
    //because this parameters's hard coded value is too high for
    //small file downloads
    int simulationFinishDelay;

    cStdDev* dwSuccess;
    cOutVector dwSuccess_vec;

    cStdDev* numBlockFail;
    cOutVector numBlockFail_vec;

    cStdDev* dataProviders;
    cOutVector dataProviders_vec;

    cStdDev* numSeederBlocks;
    cOutVector numSeederBlocks_vec;
    /**
     * Check whether the statistics collection has completed
     */
    void checkFinish();

    /**
     * Init member function of module
     */
    virtual void initialize();

    /**
     * HandleMessage member function of module
     */
    virtual void handleMessage(cMessage* msg);

    /**
     * Finish member function of module
     */
    virtual void finish();

    /**
     * Do the actual finish() call and record scalars
     */
    virtual void doFinish();

    virtual void BTPerPeerStatTimerFired();

    int             i_PerPeerStatinterval;
    bool            b_RecordPerPeerStats;
    unsigned int    ui_TotalBlockCount;
    unsigned int    ui_TotalPieceCount;
    unsigned int    ui_CumDownloadedBlockCount;
    unsigned int    ui_CumDownloadedPieceCount;

    cMessage*       p_PerPeerStatMsg;
    std::string     s_BTPerPeerStatFileName;
    map<string, PerPeerStatItem*> map_PerPeerStats;
};

#endif
