/*
 * BTMsgFactory.h
 *
 *  Created on: Oct 11, 2015
 *      Author: manojd
 */

#ifndef BTMSGFACTORY_H_
#define BTMSGFACTORY_H_

#include <omnetpp.h>

#include "BTSimpleObjFactory.h"

class BTMsgOwnerShipTakerInterface
{
public:
    BTMsgOwnerShipTakerInterface(){}
    virtual ~BTMsgOwnerShipTakerInterface(){}

    virtual void takeOwnerShip(cMessage* _pMsg) = 0 ;
};


class BTMsgFactory:public BTSimpleObjFactory<cMessage>
{
public:
    BTMsgFactory():BTSimpleObjFactory<cMessage>(1000){}
    virtual ~BTMsgFactory(){}

    cMessage* getMessageObj(const char * _pName, short _kind, BTMsgOwnerShipTakerInterface* _pOwner)
    {
        cMessage* pRet = BTSimpleObjFactory<cMessage>::getObject();

        pRet->setName(_pName);
        pRet->setKind(_kind);

        _pOwner->takeOwnerShip(pRet);

        return pRet;
    }

    static BTMsgFactory* getInstance()
    {
        return &m_Instance;
    }

private:
    static BTMsgFactory m_Instance;

};

#endif /* BTMSGFACTORY_H_ */
