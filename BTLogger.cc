

#include "BTLogInterface.h"
#include "BTLogger.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <stdexcept>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <omnetpp.h>


using std::cerr;
using std::endl;


BTLogger::BTLogger() :
i_LogTokenWidth(LOG_MSG_TOKEN_WIDTH),
i_LogMsgMaxLength(LOG_MSG_DEFUALT_MAX_LENGTH),
pc_FileNamePrefix(NULL),
fp_Log(NULL),
st_MaxFileBytes(20971520), // Default size = 20MB
pc_FullLogFName(NULL),
l_FullLogFNameLen(MAX_FILE_NAME_APPEND_LENGTH),
i_LogLevel(INT_MAX),
b_Continue(true),
s_Buffer("") {
}

BTLogger::~BTLogger() {


    if (pc_FileNamePrefix != NULL) {
        delete [] pc_FileNamePrefix;
        pc_FileNamePrefix = NULL;
    }
    
    if(pc_FullLogFName != NULL)
        delete [] pc_FullLogFName;    

        if (fp_Log != NULL) {
            fclose(fp_Log);
            fp_Log = NULL;
        }

}

bool BTLogger::Initialize(const char *_szFileNamePrefix, int _iLogLevel, int _iMaxFileMBSize,int _iTokenWidth, int _iLogMsgLength ) {

    bool bRet = false;
    size_t lPrefixLen;

    try {
        if(_iTokenWidth > 0)
                i_LogTokenWidth=_iTokenWidth;
        if(_iLogMsgLength > 0)
                i_LogMsgMaxLength=_iLogMsgLength;

        if (_szFileNamePrefix != NULL) {

            lPrefixLen = strlen(_szFileNamePrefix);
            pc_FileNamePrefix = new char[lPrefixLen + 1]; 
            strncpy(pc_FileNamePrefix, _szFileNamePrefix, lPrefixLen);
            pc_FileNamePrefix[lPrefixLen] = '\0';            
          
            
            l_FullLogFNameLen = strlen(pc_FileNamePrefix) + MAX_FILE_NAME_APPEND_LENGTH;
            pc_FullLogFName=new char[l_FullLogFNameLen];
    

            if (_iMaxFileMBSize != -1) {
                SetMaxFileSize(_iMaxFileMBSize);
            }

            if (_iLogLevel != -1) {
                SetLogLevel(_iLogLevel);
            }


            if (OpenFile() == true) {
                bRet = true;

                int iBufferLen = strlen(s_Buffer.c_str());
                if (iBufferLen > 0) {
                    WriteInFile(s_Buffer.c_str());
                    s_Buffer = "";
                }
            }
            else
            {
                std::cerr << "BTLogger::Initialize ; Failed to Open File:"<<pc_FullLogFName<<std::endl;
            }

        }

    } catch (std::exception & e) {
        std::cerr << "BTLogger::Init ;   EXCEPTION:" << e.what() << std::endl;
    }

    return bRet;
}



void BTLogger::SetMaxFileSize(size_t _lMaxFileMBSize) {
    st_MaxFileBytes = _lMaxFileMBSize * 1048576;
}

void BTLogger::SetLogLevel(int _iLogLevel) {
    i_LogLevel = _iLogLevel;
}

bool BTLogger::OpenFile() {

    bool bRet = false;

    try {

        if (fp_Log != NULL) {
            fclose(fp_Log);
            fp_Log = NULL;
        }

        SetFileTimeStamp();

        if (pc_FullLogFName != NULL) {
            fp_Log = fopen(pc_FullLogFName, "wt");
            if (fp_Log != NULL) {
                bRet = true;
            } else {
                std::cerr << "Failed to open the file :" << pc_FullLogFName << std::endl;
            }
        }

    } catch (std::exception & e) {
        std::cout << "BTLogger::Init ;   EXCEPTION:" << e.what() << std::endl;
    }
    return bRet;
}

void BTLogger::SetFileTimeStamp() {

    
    struct timeb tbNow;
    char szNow[MAX_FILE_NAME_APPEND_LENGTH];

    ftime(&tbNow);
    strftime(szNow, sizeof (szNow), "%m%d_%H%M%S", localtime(&tbNow.time));
#ifndef WINNT
    snprintf(pc_FullLogFName, l_FullLogFNameLen,"%s_%s%03u.log", pc_FileNamePrefix, szNow, tbNow.millitm);
#else
    _snprintf(pc_FullLogFName, l_FullLogFNameLen,"%s_%s%03u.log", pc_FileNamePrefix, szNow, tbNow.millitm);
#endif /* WINNT */
}

void BTLogger::Log(const char *_pszMsgType, const char *_pszMsgKey, const char *_pszText) {

//    struct timeb tbNow;
//    char szNow[16];
    char szLog[i_LogMsgMaxLength];
    int iPrintfRes;

        // acquire the current time
//        ftime(&tbNow);
//        strftime(szNow, sizeof (szNow), "%H:%M:%S", localtime(&tbNow.time));
        // prepares the complete string to write to the file
        double dSimTime = simTime().dbl();

#ifndef WINNT
        iPrintfRes = snprintf(szLog, i_LogMsgMaxLength, "%.6f  %-3.3s %-*.*s  %s \n\n",
                dSimTime,  _pszMsgType, i_LogTokenWidth, i_LogTokenWidth-3, _pszMsgKey,  _pszText);

#else
        iPrintfRes = _snprintf(szLog, i_LogMsgMaxLength, "%.6f  %-3.3s %-*.*s  %s \n\n",
                dSimTime,  _pszMsgType, i_LogTokenWidth, i_LogTokenWidth-3, _pszMsgKey,  _pszText);

#endif /* WINNT */

        if (iPrintfRes > 0 )
            WriteInFile(szLog);
        else
            throw std::runtime_error("Failed to prepare the log statement");


}

void BTLogger::WriteInFile(const char *_pszMsg) {

    if (fp_Log != NULL) {
        struct stat st;

        if ((stat(pc_FullLogFName, &st) != 0) || ((size_t) ftell(fp_Log) > st_MaxFileBytes)) {
            OpenFile();
        }

        if(fputs(_pszMsg, fp_Log) == EOF )
        {
            if(ferror(fp_Log))
            {
                throw std::runtime_error("Failed to write in the log");
            }            
        }    
        
        if(fflush(fp_Log)== EOF )
        {
            if(ferror(fp_Log))
            {
                throw std::runtime_error("Failed to write in the log");
            }            
        }        

    } else {
        s_Buffer.append(_pszMsg);
    }
}


