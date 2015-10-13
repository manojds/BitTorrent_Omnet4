#ifndef SIMPLEOBJFACTORY_H_
#define SIMPLEOBJFACTORY_H_

#include <list>

using std::list;

template <class T>
class BTSimpleObjFactory
{
public:
    BTSimpleObjFactory(int _iChunkSize, bool _bReleaseAllOnDestruction = false);
    virtual ~BTSimpleObjFactory();


    T *     getObject();
    void    releaseObject(T* &_pNode);

protected:
    void        checkForRenewal();
    void        renewTheChunk();
    void        releaseChunks();

    bool        b_ReleaseAllOnDestruction;
    int         i_NextAvailableSlot;
    int         i_ChunkSize;
    T *         p_ActiveChunk;
    list<T*>    lst_FreeList;
    list<T*>    lst_ActiveChunks;
};

template <class T>
BTSimpleObjFactory<T>::BTSimpleObjFactory(int _iChunkSize, bool _bReleaseAllOnDestruction) :
    b_ReleaseAllOnDestruction(_bReleaseAllOnDestruction),
    i_NextAvailableSlot(0),
    i_ChunkSize(_iChunkSize),
    p_ActiveChunk(NULL)
{
}

template <class T>
BTSimpleObjFactory<T>::~BTSimpleObjFactory()
{
    releaseChunks();
}

template <class T>
void BTSimpleObjFactory<T>::releaseChunks()
{
    typename std::list<T*>::iterator itr = lst_ActiveChunks.begin();
    for( ; lst_ActiveChunks.end() != itr ; ++itr )
    {
        T* t = *itr;
        delete [] t;
    }
    lst_ActiveChunks.clear();

}

template <class T>
T * BTSimpleObjFactory<T>::getObject()
{
    T *  pRet(NULL);

    if (lst_FreeList.size() > 0 )
    {
        pRet = lst_FreeList.front();
        lst_FreeList.pop_front();
    }
    else
    {
        checkForRenewal();

        pRet = p_ActiveChunk + i_NextAvailableSlot;

        i_NextAvailableSlot++;
    }

    return pRet;
}


template <class T>
void BTSimpleObjFactory<T>::releaseObject(T* &_pNode)
{
    if (_pNode == NULL)
        return;
    else
    {
        lst_FreeList.push_back(_pNode);
        _pNode = 0;
    }
}

template <class T>
void BTSimpleObjFactory<T>::checkForRenewal()
{
    if ((p_ActiveChunk == NULL) || (i_NextAvailableSlot >= i_ChunkSize ))
        renewTheChunk();

}

template <class T>
void BTSimpleObjFactory<T>::renewTheChunk()
{
    p_ActiveChunk = new T[i_ChunkSize];
    
    lst_ActiveChunks.push_back(p_ActiveChunk);
    i_NextAvailableSlot = 0;
}


#endif /* SIMPLEOBJFACTORY_H_ */

