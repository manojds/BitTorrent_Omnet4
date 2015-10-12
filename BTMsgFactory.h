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

class BTMsgFactory:public BTSimpleObjFactory<cMessage>
{
public:
    BTMsgFactory():BTSimpleObjFactory<cMessage>(1000){}
    virtual ~BTMsgFactory(){}

    cMessage* getMessageObj(const char * _pName, short _kind)
    {
        cMessage* pRet = BTSimpleObjFactory<cMessage>::getObject();

        pRet->setName(_pName);
        pRet->setKind(_kind);

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
