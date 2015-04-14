/* 
 * File:   BTLogger.h
 * Author: ManojD
 *
 * Created on December 27, 2014, 9:40 AM
 */

/*!
 * default value for log message maximum length. you can override this when initializing the log Sink
 */
#define LOG_MSG_DEFUALT_MAX_LENGTH              4096
/*!
 * default value for the log message token width.
 * 
 */
#define LOG_MSG_TOKEN_WIDTH                     32

#define MAX_FILE_NAME_APPEND_LENGTH     64


#ifndef BTLOGGER_H
#define	BTLOGGER_H



class BTLogger 
{
public:
    BTLogger();
    virtual ~BTLogger();

    bool Initialize(const char *pszFileNamePrefix, int nLogLevel = -1, int iMaxFileMBSize = -1,
                            int _iTokenWidth=LOG_MSG_TOKEN_WIDTH, int _iLogMsgLength = LOG_MSG_DEFUALT_MAX_LENGTH );
    //bool StartRunTimeLevelUpd(const char *pszLevelFilePath, const char *pszLevelKeyName, unsigned long ulFileCheckPeriodMs);
    void SetMaxFileSize(size_t lMaxFileMBSize);
    void SetLogLevel(int nLogLevel);

    void Log(const char *pszMsgType, const char *pszMsgKey, const char *pszText);


private:
    bool OpenFile();

    void WriteInFile(const char *pszMsg);
    bool UpdateLogLevel();
    void CheckLevelFile();
    void Run();
    void SetFileTimeStamp();
    static void* LevelFileCheckThreadProc(void *_pArg);


    int i_LogTokenWidth;
    int i_LogMsgMaxLength;
    // fixed part of the file name in which to write the log
    char *pc_FileNamePrefix;
    // pointer to the log file
    FILE *fp_Log;

    // maximum size of the log file in bytes
    size_t st_MaxFileBytes;

    //Log file name with the timestamp
    char * pc_FullLogFName;
    //Length of full Log file name
    size_t l_FullLogFNameLen;
    //log level
    int i_LogLevel;
    //thread continuation condition 
    bool b_Continue;
    //buffer any messages before initialize the log
    std::string s_Buffer;


}; 



#endif	/* BTLOGGER_H */

