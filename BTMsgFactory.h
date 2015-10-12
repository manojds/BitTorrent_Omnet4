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
    BTMsgFactory(){}
    virtual ~BTMsgFactory(){}

    cMessage* getMessageObj(const char * _pName, short _kind)
    {
        cMessage* pRet = BTSimpleObjFactory<cMessage>::getObject();

        pRet->setName(_pName);
        pRet->setKind(_kind);

        return pRet;
    }

};

#endif /* BTMSGFACTORY_H_ */
