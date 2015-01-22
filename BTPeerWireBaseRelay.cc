/*
 * BTPeerWireBaseRelay.cc
 *
 *  Created on: Jan 21, 2015
 *      Author: manojd
 */

#include "BTPeerWireBaseRelay.h"
#include "BTPeerWireClientHandlerBase.h"
#include "BTLogImpl.h"

Define_Module(BTPeerWireBaseRelay);

BTPeerWireBaseRelay::BTPeerWireBaseRelay() {
    // TODO Auto-generated constructor stub

}

BTPeerWireBaseRelay::~BTPeerWireBaseRelay() {
    // TODO Auto-generated destructor stub
}

void BTPeerWireBaseRelay::initialize()
{
    BTPeerWireBase::initialize();
    evtRelayTrackerComm = new cMessage(toString(INTERNAL_TRACKER_REALY_COM_MSG), INTERNAL_TRACKER_REALY_COM_MSG);
    scheduleAt(simTime(), evtRelayTrackerComm);
}

void BTPeerWireBaseRelay::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage() && msg->arrivedOn("relayTrackerIn"))
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
                            trackerResponse()->announceInterval();

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

