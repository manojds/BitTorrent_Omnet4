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


#include "BTStatistics.h"
#include "BTLogImpl.h"
#include <iostream>
#include <fstream>

Define_Module(BTStatistics);

BTStatistics::BTStatistics():
        ui_TotalBlockCount(0),
        ui_TotalPieceCount(0),
        p_PerPeerStatMsg(NULL),
        s_BTPerPeerStatFileName("BT_Stats"){

}

BTStatistics::~BTStatistics()
{
	delete dwSuccess;
	delete numBlockFail;
	delete dataProviders;
	delete numSeederBlocks;

	cancelAndDelete(p_PerPeerStatMsg);
}


void BTStatistics::initialize()
{
	int iLogLevel=par("logLevel");
    btLogSinker.initialize("BTLog",(LogLevel_t)iLogLevel);

    BT_LOG_ESSEN(btLogSinker, "BTStatistics::initialize", "Log Initialized. log level is ["<<iLogLevel<<"]");

    simulationFinishDelay = par("simulationFinishDelay");
	
	currentTerminalNum = 0;
	//modifeid by Manoj - BTR-012 - 2015-03-01
	//targetOverlayTerminalNum = par("targetOverlayTerminalNum");
	targetOverlayTerminalNum = par("numDownloadersToComplete");

	BT_LOG_INFO(btLogSinker, "BTStatistics::initialize", "target download completion count is ["<<targetOverlayTerminalNum<<"]");

	dwSuccess = new  cStdDev("BitTorrent:Download Duration");
	dwSuccess_vec.setName("BitTorrent:Download Duration");

	numBlockFail = new  cStdDev("BitTorrent:Failed Downloads:Number of Completed Blocks");
	numBlockFail_vec.setName("BitTorrent:Failed Downloads:Number of Completed Blocks");

	dataProviders = new cStdDev("BitTorrent:Number of Distinct Data Providing Peers");
	dataProviders_vec.setName("BitTorrent:Number of Distinct Data Providing Peers");

	numSeederBlocks = new cStdDev("BitTorrent:Number of Blocks Download From Seeder");
	numSeederBlocks_vec.setName("BitTorrent:Number of Blocks Download From Seeder");

	i_PerPeerStatinterval = par("perPeerStatInterval");
	b_RecordPerPeerStats = par("recordPerPeerStats");

	if (b_RecordPerPeerStats )
	{

	    p_PerPeerStatMsg = new cMessage("BT_PER_PEER_STAT", BT_PER_PEER_STAT);

	    ofstream myfile (s_BTPerPeerStatFileName.c_str());
        if (myfile.is_open() == false)
        {
          throw cRuntimeError("Failed to open file [%s] for stat writing", s_BTPerPeerStatFileName.c_str());
          return;
        }

        myfile<<" ************** BT Per Peer Stats *********************"<<endl<<endl;

        myfile.close();

        BTPerPeerStatTimerFired();
	}
}


void BTStatistics::handleMessage(cMessage* msg)
{
	switch (msg->getKind())
	{
		case BT_STATS_DWL:
		{
			BTStatisticsDWLMsg* dwMsg = dynamic_cast<BTStatisticsDWLMsg*>(msg);
			double dwTime = dwMsg->downloadTime();
			double rmBlocks = dwMsg->remainingBlocks();
			if (rmBlocks == 0 )
			{
				dwSuccess->collect(dwTime);
				dwSuccess_vec.record(dwTime);

				BT_LOG_ESSEN(btLogSinker, "BTStatistics::handleMessage",
                        "Download success count is now ["<<dwSuccess->getCount()<<
                        "] just completed node ["<<msg->getSenderModule()->getParentModule()->getFullName()<<"] Current average ["
                        <<dwSuccess->getMean()<<"] just completed value ["<<dwTime <<"]");
			}
			else
			{
				numBlockFail->collect(rmBlocks);
				numBlockFail_vec.record(rmBlocks);
			}
			checkFinish();
			delete msg;
			break;
		}
		case BT_STATS_PP:
		{
			BTStatisticsNumProvidersMsg* ppMsg = dynamic_cast<BTStatisticsNumProvidersMsg*>(msg);
			dataProviders->collect(ppMsg->numPeers());
			dataProviders_vec.record(ppMsg->numPeers());
			delete msg;
			break;
		}
		case BT_STATS_NSB:
		{
			BTStatisticsNumSeederBlocksMsg* nsbMsg = dynamic_cast<BTStatisticsNumSeederBlocksMsg*>(msg);
			numSeederBlocks->collect(nsbMsg->numSeederBlocks());
			numSeederBlocks_vec.record(nsbMsg->numSeederBlocks());
			delete msg;
			break;
		}

		case BT_STATS_EXIT:
		{
			delete msg;
			doFinish();
			break;
		}
        case BT_PER_PEER_STAT:
            BTPerPeerStatTimerFired();
            break;
		default:
		{
			opp_error("Unknown message type %d",msg->getKind());
			break;
		}
	}
}

void BTStatistics::checkFinish()
{
	currentTerminalNum++;
	if (currentTerminalNum == targetOverlayTerminalNum)
	{
	    //edited by Manoj. 2015-04-17
		//scheduleAt(simTime()+BT_STATS_MSG_TIME, new cMessage(NULL,BT_STATS_EXIT));
	    scheduleAt(simTime()+simulationFinishDelay, new cMessage(NULL,BT_STATS_EXIT));
	}
}

void BTStatistics::updatePerPeerBlockCount(const string & _sPeerID,unsigned int _uiDownloadedBlocks, unsigned int _uiTotalBlocks ,
        unsigned int _uiDownloadedPieces, unsigned int _uiTotalPieces, double _dLastDwlTime)
{

    ui_TotalBlockCount = _uiTotalBlocks;
    ui_TotalPieceCount = _uiTotalPieces;

    PerPeerStatItem* pStatItem(NULL);
    map<string,PerPeerStatItem*>::iterator itr = map_PerPeerStats.find(_sPeerID);

    if (itr == map_PerPeerStats.end())
    {
        pStatItem = new PerPeerStatItem();
        map_PerPeerStats[_sPeerID] = pStatItem;
    }
    else
    {
        pStatItem = itr->second;
    }

    pStatItem->ui_BlockCount = _uiDownloadedBlocks;
    pStatItem->ui_PieceCount = _uiDownloadedPieces;
    pStatItem->d_LastDwlTime = _dLastDwlTime;
}

void BTStatistics::BTPerPeerStatTimerFired()
{
    if (!b_RecordPerPeerStats)
        return ;

    ofstream myfile (s_BTPerPeerStatFileName.c_str(), ios::app);
    if (myfile.is_open() == false)
    {
      throw cRuntimeError("Failed to open file [%s] for stat writing", s_BTPerPeerStatFileName.c_str());
      return;
    }

    unsigned int uiCumBlockCount = ui_TotalBlockCount*map_PerPeerStats.size();
    unsigned int uiCumPieceCount = ui_TotalPieceCount*map_PerPeerStats.size();

    unsigned int uiCumDownloadedBlockCount(0);
    unsigned int uiCumDownloadedPieceCount(0);

    myfile<<"Time - "<<simTime()<<endl;

    map<string,PerPeerStatItem*>::iterator itr =  map_PerPeerStats.begin();
    for( ; itr !=  map_PerPeerStats.end() ; itr++ )
    {
        myfile<<itr->first<<" , Block Count , "<<itr->second->ui_BlockCount<<" , Total Block Count , "<<ui_TotalBlockCount<<
                " , Piece Count , "<<itr->second->ui_PieceCount << " , Total Piece Count , "<<ui_TotalPieceCount<<
                " , Last Download Time , "<<itr->second->d_LastDwlTime<<endl;

        uiCumDownloadedBlockCount += itr->second->ui_BlockCount;
        uiCumDownloadedPieceCount += itr->second->ui_PieceCount;

    }

    myfile<<simTime()<<" , **Cumulative Stat** , Total Blocks Downloaded ,"<<uiCumDownloadedBlockCount<<" , Total Blocks ,"<<uiCumBlockCount<<" ,  Total Pieces Downloaded  ,"
            <<uiCumDownloadedPieceCount<<" , Total Pieces , "<<uiCumPieceCount<<endl;

    myfile.close();

    scheduleAt( simTime() + i_PerPeerStatinterval, p_PerPeerStatMsg);


}

void BTStatistics::doFinish()
{
	std::cout<<"Finishing Simulation.."<<std::endl;
	BT_LOG_ESSEN(btLogSinker,"BTStatistics::doFinish","Finishing Simulation..");
	recordScalar("Simulation duration", simTime());

	BT_LOG_ESSEN(btLogSinker,"BTStatistics::doFinish","Simulation time ["<<simTime()<<"]");


	BT_LOG_ESSEN(btLogSinker,"BTStatistics::doFinish","Download duration Mean ["<<dwSuccess->getMean()<<
	        "] STD Dev["<<dwSuccess->getStddev()<<"] Count ["<<dwSuccess->getCount()<<"]");

	dwSuccess->record();
	numBlockFail->record();
	dataProviders->record();
	numSeederBlocks->record();
	endSimulation();
}

void BTStatistics::finish()
{

}

