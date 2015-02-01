/*
 * BTPeerWireBaseRelay.cc
 *
 *  Created on: Jan 21, 2015
 *      Author: manojd
 */

#include "BTPeerWireBaseRelay.h"
#include "BTPeerWireClientHandlerBase.h"
#include "BTLogImpl.h"
#include "BTStatisticsRelay.h"

Define_Module(BTPeerWireBaseRelay);

BTPeerWireBaseRelay::BTPeerWireBaseRelay():
        b_TrackerCommIsEnbled(false)
{
    // TODO Auto-generated constructor stub

}

BTPeerWireBaseRelay::~BTPeerWireBaseRelay() {
    // TODO Auto-generated destructor stub
}

void BTPeerWireBaseRelay::initialize()
{
    BTPeerWireBase::initialize();
    evtRelayTrackerComm = new cMessage(toString(INTERNAL_TRACKER_REALY_COM_MSG), INTERNAL_TRACKER_REALY_COM_MSG);
    //TODO :: uncomment this to enble communication with tracker as relay peer
    //          after respective modifications done in the tracker.
    scheduleAt(simTime(), evtRelayTrackerComm);
}

void BTPeerWireBaseRelay::handleMessage(cMessage *msg)
{
    if ((!msg->isSelfMessage()) && msg->arrivedOn("relayTrackerIn"))
    {
        handleMsgFromRelayTracker(msg);
        delete msg;

    }
    else
    {
        //This is not related to us. let the super handle this message.
        BTPeerWireBase::handleMessage(msg);
    }
}

void BTPeerWireBaseRelay::handleMsgFromRelayTracker(cMessage *msg)
{
    if (getState() < EXITING)
    {
        BT_LOG_DEBUG( btLogSinker,
                        "BTPeerWireBaseRelay::handleMsgFromRelayTracker",
                        "["<< this->getParentModule()->getFullName()<<"] tracker response arrived.");

        BTTrackerMsgResponse* trackerResponse_msg =
                check_and_cast<BTTrackerMsgResponse*>(msg);

        double      relayAnnounceInterval_var=
                trackerResponse_msg->announceInterval();

        //Scheduling next contact with the tracker.
        scheduleAt(simTime() + relayAnnounceInterval_var, evtRelayTrackerComm);

    }

}

void BTPeerWireBaseRelay::handleSelfMessage(cMessage* msg)
 {
    switch (msg->getKind())
    {
    case INTERNAL_TRACKER_REALY_COM_MSG:
        BT_LOG_INFO(
                btLogSinker,
                "BTPeerWireBaseRelay::handleSelfMessage",
                "["<< this->getParentModule()->getFullName()<<"] instructing communication with the tracker...");

        switch (getState())
        {
        case NORMAL:
        case ENDGAME:
        case SEEDER:
        case COMPLETED:
        case SEEDING:
        {
            send(new cMessage(toString(EVT_CONN), EVT_CONN), "relayTrackerOut");
            break;
        }

        case EXITING:
        {
            send(new cMessage(toString(EVT_STOP), EVT_STOP), "relayTrackerOut");
            break;
        }

        default:
            error("%s:%d at %s() Invalid client state (STATE = %d). \n",
                    __FILE__, __LINE__, __func__, getState());
            break;

        }

        break;

    case INTERNAL_EXIT_SAFE_MSG:
    {
        cancelAndDelete(evtRelayTrackerComm);
        BTPeerWireBase::handleSelfMessage(msg);
        break;
    }

    default:
        BTPeerWireBase::handleSelfMessage(msg);
        break;

    }
}

void BTPeerWireBaseRelay::scheduleTrackerCommAt(simtime_t t)
{
    if(b_TrackerCommIsEnbled)
    {
        BTPeerWireBase::scheduleTrackerCommAt(t);
    }

}

void BTPeerWireBaseRelay::newConnectionFromPeerEstablished(PEER peer, TCPServerThreadBase* thread)
{
    BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::newConnectionFromPeerEstablished",
            "["<< this->getParentModule()->getFullName()<<"] ConnMngmnt - New connection arrived from peer ["<<peer.peerId<<"]");

    bool bInitiateTrackerComm(false);

    if(initiatedPeers.size() == 0)
        bInitiateTrackerComm=true;

    std::map<IPvXAddress, PEER>::iterator itr = initiatedPeers.find(peer.ipAddress);
    if(itr == initiatedPeers.end())
    {
        initiatedPeers[peer.ipAddress]=peer;
    }
    else
    {
        std::stringstream ss;
        ss<<"["<< this->getParentModule()->getFullName()<<"] ConnMngmnt - Connection came from the same peer  twice. PeerID ["<<
                peer.peerId<<"] IPaddress ["<<peer.ipAddress<<"]";

        BT_LOG_ERROR( btLogSinker, "BTPeerWireBaseRelay::newConnectionFromPeerEstablished", ss.str().c_str());


        throw cRuntimeError(ss.str().c_str());
    }

    if(bInitiateTrackerComm)
    {
        BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::newConnectionFromPeerEstablished",
                "Starting to act as Relay. Initiating TRacker Communication for true hash");

        enableTrackerComm();
    }
}

void BTPeerWireBaseRelay::newConnectionToPeerEstablished(PEER peer, TCPServerThreadBase* thread)
{
    BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::newConnectionToPeerEstablished",
            "["<< this->getParentModule()->getFullName()<<"] ConnMngmnt - Connection Established with peer ["<<peer.peerId<<"]");
}

void BTPeerWireBaseRelay::connectionLostFromPeer(PEER peer)
{
    BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::connectionLostFromPeer",
            "["<< this->getParentModule()->getFullName()<<"] ConnMngmnt - Connection Lost with peer ["<<peer.ipAddress<<"]");


    //we remove this peer from initiator list if it is a initiator
    initiatedPeers.erase(peer.ipAddress);

    if(initiatedPeers.size() == 0)
    {
        BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::connectionLostFromPeer","["<<this->getParentModule()->getFullName()<<"]"
                "There is no more connections to act as Relay. Stopping participating in swarm");
        disableTrackerComm();
        pauseChokingAlgos();
        closeAllConnections();

    }
}

void BTPeerWireBaseRelay::pauseChokingAlgos()
{
    BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::pauseChokingAlgos","["<<this->getParentModule()->getFullName()<<"]"
            "Pausing Choking Algorithms...");

    if(evtChokeAlg->isScheduled())
        cancelEvent(evtChokeAlg);

    if(evtOptUnChoke->isScheduled())
        cancelEvent(evtOptUnChoke);

}

void BTPeerWireBaseRelay::enableTrackerComm()
{
    if(b_TrackerCommIsEnbled == false)
    {
        BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::enableTrackerComm","["<<this->getParentModule()->getFullName()<<"]"
                "Enabling Tracker Communication...");

        b_TrackerCommIsEnbled=true;

        if(evtTrackerComm->isScheduled() == false)
        {
            scheduleTrackerCommAt(simTime());

        }
    }
}

void BTPeerWireBaseRelay::disableTrackerComm()
{
    if(b_TrackerCommIsEnbled == true)
    {
        BT_LOG_INFO( btLogSinker, "BTPeerWireBaseRelay::disableTrackerComm","["<<this->getParentModule()->getFullName()<<"]"
                "Disabling Tracker Communication...");

        b_TrackerCommIsEnbled=false;
        if(evtTrackerComm->isScheduled() == true)
        {
            cancelEvent(evtTrackerComm);
        }
    }
}

void BTPeerWireBaseRelay::writeStats()
{
    BTStatisticsDWLMsg* msgDWL = new BTStatisticsDWLMsg("BT_STATS_RELAY_DWL",BT_STATS_RELAY_DWL);
    msgDWL->setDownloadTime(SIMTIME_DBL(downloadDuration()));
    msgDWL->setRemainingBlocks(localBitfield()->numRemainingBlocks());
    sendDirect(msgDWL,  btStatistics, btStatistics->findGate("direct_in"));


}

const char* BTPeerWireBaseRelay::toString(int type)
{
    switch (type)
    {
        case INTERNAL_TRACKER_REALY_COM_MSG :
            return "INTERNAL_TRACKER_REALY_COM_MSG";
            break;
        default:
            return BTPeerWireBase::toString(type);
            break;
    }
    return NULL;
}

