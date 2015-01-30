/*
 * BTPeerWireBaseRelay.h
 *
 *  Created on: Jan 21, 2015
 *      Author: manojd
 */

#ifndef BTPEERWIREBASERELAY_H_
#define BTPEERWIREBASERELAY_H_
#include "BTPeerWireBase.h"

#define INTERNAL_TRACKER_REALY_COM_MSG                  208

class INET_API BTPeerWireBaseRelay :public BTPeerWireBase
{
public:
    BTPeerWireBaseRelay();
    virtual ~BTPeerWireBaseRelay();

protected:
    /* Redefined methods from BTPeerWireBase */
    virtual void initialize();

    virtual void handleMessage(cMessage *msg);

    virtual void handleSelfMessage(cMessage* msg);
    virtual void scheduleTrackerCommAt(simtime_t t);

    virtual void newConnectionFromPeerEstablished(PEER peer, TCPServerThreadBase* thread);
    virtual void newConnectionToPeerEstablished(PEER peer, TCPServerThreadBase* thread);
    virtual void connectionLostFromPeer(PEER peer);
    /* End of redefined methods from BTPeerWireBase */

    virtual void enbaleTrackerComm();
    virtual void disableTrackerComm();

    const char* toString(int);

private:
    void handleMsgFromRelayTracker(cMessage *msg);

    bool        b_TrackerCommIsEnbled;
    cMessage*   evtRelayTrackerComm;   //Timer to schedule communication with the Tracker for Relay purposes
    PeerState   initiatedPeers;     //Peer who initiated connections to this relay peer

};

#endif /* BTPEERWIREBASERELAY_H_ */
