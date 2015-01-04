
#include "BTLogSink.h"
#include "BTLogger.h"
#include <limits.h>
#include <stdlib.h>
#include <omnetpp.h>

BTLogSink::BTLogSink():
        t_LogLevel((LogLevel_t)INT_MAX),     // I set log level as high as I can
        p_Logger(NULL)
{
    
}

bool BTLogSink::initialize(const char* _LogFilename, LogLevel_t _tLevel, int iMaxFileMBSize, int _iTokenWidth, int _iLogMsgLength)
{
    t_LogLevel  =_tLevel;
    p_Logger =new BTLogger();
    
    return p_Logger->Initialize(_LogFilename, _tLevel, iMaxFileMBSize, _iTokenWidth,  _iLogMsgLength);
}

BTLogSink::~BTLogSink()
{
    delete p_Logger;
}

void BTLogSink::doLog(LogLevel_t _level, const char * _pToken, const char  * _pMessage)
{
    try
    {
        if(p_Logger)
                p_Logger->Log(logLevelToString(_level), _pToken, _pMessage);

	EV<<"["<<_pToken<<"] - "<<_pMessage<<std::endl;
    }
    catch(std::exception & ex)
    {
        abort();
    }
}

LogLevel_t BTLogSink::getLogLevel()
{
    return t_LogLevel;
}

bool BTLogSink::isLogLevelActive(LogLevel_t _level)
{
    if(_level<= t_LogLevel)
        return true;
    else
        return false;
}
