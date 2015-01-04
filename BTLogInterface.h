/* 
 * File:   BTLogInterface.h
 * Author: ManojD
 *
 * Created on December 27, 2014, 9:31 AM
 */

#ifndef BTLOGINTERFACE_H
#define	BTLOGINTERFACE_H

#include <sstream>


#define BT_LOG(level, logsink, token, message)        \
do{                                                     \
    if (level > logsink.getLogLevel());                 \
    else{                                               \
        LogEvent logEvt(level, token, logsink);         \
        logEvt.Get()<<message;                          \
    }                                                   \
}                                                       \
while(0)


#define BT_LOG_ERROR(logger, token, logEvent)         \
    BT_LOG(logERROR, logger, token, logEvent)

#define BT_LOG_WARN(logger, token, logEvent)          \
    BT_LOG(logWARNING, logger, token, logEvent)

#define BT_LOG_INFO(logger, token, logEvent)          \
    BT_LOG(logINFO,  logger, token, logEvent)

#define BT_LOG_DEBUG(logger, token, logEvent)         \
    BT_LOG(logDEBUG, logger, token, logEvent)

#define BT_LOG_DETAIL(logger, token, logEvent)        \
    BT_LOG(logDETAIL, logger, token, logEvent)

#define BT_LOG_VERBOSE(logger, token, logEvent)       \
    BT_LOG(logVERBOSE, logger, token, logEvent)

enum LogLevel_t {
    logOFF=1,
    logERROR, 
    logWARNING, 
    logINFO, 
    logDEBUG, 
    logDETAIL,
    logVERBOSE
};

const char * logLevelToString(LogLevel_t _level);


class LogSinkInterface{
public:
    LogSinkInterface(){}
    virtual ~LogSinkInterface(){}
    virtual void        doLog(LogLevel_t _level, const char * _pToken, const char  * _pMessage)=0;
    virtual LogLevel_t  getLogLevel()=0;
    virtual bool        isLogLevelActive(LogLevel_t _level)=0;
};

class LogEvent
{
    public:
       LogEvent(LogLevel_t _level, const char * _pToken, LogSinkInterface & _logSink);
       virtual ~LogEvent();
       std::ostringstream& Get();

    protected:
       std::ostringstream os;
    private:
       LogEvent(const LogEvent&);
       LogEvent& operator =(const LogEvent&);
    private:
       LogLevel_t   m_Level;
       const char * p_Token;
       LogSinkInterface &    m_Sink;
};

inline std::ostringstream& LogEvent::Get()
{
   return os;
}

inline LogEvent::LogEvent(LogLevel_t _level, const char * _pToken, LogSinkInterface & _logSink):
        m_Level(_level),
        p_Token(_pToken),
        m_Sink(_logSink)
{
}

inline LogEvent::~LogEvent()
{
  m_Sink.doLog(m_Level, p_Token, os.str().c_str());
}

inline const char * logLevelToString(LogLevel_t _level)
{
    const char * pRet(NULL);
    switch(_level)
    {
        case logERROR:
            pRet="ERR";
            break;
            
        case logWARNING:
            pRet="WRN";
            break;  
            
        case logINFO:
            pRet="INF";
            break;  
            
        case logDEBUG:
            pRet="DBG";
            break;  
            
        case logDETAIL:
            pRet="DTL";
            break;   
            
        case logVERBOSE:
            pRet="VRB";
            break; 
            
        default:
            pRet="UNK";
            break;
            
    }
    
    return pRet;
}

#endif	/* BTLOGINTERFACE_H */

