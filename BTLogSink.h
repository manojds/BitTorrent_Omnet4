/* 
 * File:   BTLogSink.h
 * Author: ManojD
 *
 * Created on December 27, 2014, 9:33 AM
 */

#ifndef BTLOGSINK_H
#define	BTLOGSINK_H
#include "BTLogInterface.h"

class BTLogger;


class BTLogSink: public LogSinkInterface
{
public:
    BTLogSink();
    ~BTLogSink();

    bool            initialize(const char * _LogFilename, LogLevel_t _tLevel, int iMaxFileMBSize=-1, int _iTokenWidth=-1,
                            int _iLogMsgLength = -1);
    void            doLog(LogLevel_t _level, const char * _pToken, const char  * _pMessage);
    LogLevel_t      getLogLevel();
    bool            isLogLevelActive(LogLevel_t _level);

private:
    LogLevel_t      t_LogLevel;
    BTLogger *          p_Logger;

};

#endif	/* BTLOGSINK_H */

