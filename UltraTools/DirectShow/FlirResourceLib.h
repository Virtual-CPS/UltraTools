/***********************************************************************
 *
 * Project:   Matrix (PC)
 * $Workfile: res_lib.h $
 *
 * Description of file:
 *    C interface to Matrix camera resources over TCP/IP sockets
 *
 * Copyright: FLIR Systems
 ***********************************************************************/

#ifndef _RES_LIB_H
#define _RES_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FlirResProtocol.h"
    
/* Resource type defintions
 *
 */

#define RES_BOOL	    0x1
#define RES_INT32	    0x2
#define RES_DOUBLE	    0x3
#define RES_TEXT	    0x4
#define RES_ENTRY	    0x5
#define RES_INDEX		0x6
#define RES_UNICODE		0x7
    
/* Resource value
 */
typedef struct
{
    char type;
    union
    {
        int			b;  /* bool */
        int			i;  /* int32 */
        double		d;  /* double */
        char*		t;  /* text */
        wchar_t*	u;  /* unicode text */
    };
} ResVal;

/* Resource attributes
 */
typedef struct
{
    char type;
    char getSupported;
    char setSupported;
    char createSupported;
    char deleteSupported;
    char subscrSupported;
    char persistent;
    char sync;
    unsigned int openCount;
    unsigned int subscrCount;
} ResAttr;

#define RES_COMM_OK               0

#define RES_COMM_ERR_BASE         -100

#define RES_COMM_ERR_WSASTARTUP   (RES_COMM_ERR_BASE - 1)
#define RES_COMM_ERR_RESOLVE      (RES_COMM_ERR_BASE - 2)
#define RES_COMM_ERR_UNKNOWN_TYPE (RES_COMM_ERR_BASE - 3)

/*
 *  ResVal manipulation functions
 *
 */

/* Create empty value */
ResVal* ResCreateVal();

/* Create boolean value */
ResVal* ResCreateBoolVal(char val);

/* Create integer value */
ResVal* ResCreateIntVal(int val);

/* Create double precision floating point value */
ResVal* ResCreateDoubleVal(double val);

/* Create text value */
ResVal* ResCreateTextVal(char* val);

/* Create unicode text value */
ResVal* ResCreateUnicodeVal(wchar_t *val);

/* Delete value */
void    ResDeleteVal(ResVal* pVal);

/* Print value to stdout */
int     ResPrintVal(ResVal* pVal);

/*
 *  Socket connection functions
 *
 */

/* Open session to camera */
int  ResOpenSocket(char* pzAddr, unsigned short usPort);

/* Close local session */
void ResCloseSocket(int ulSock);

/*
 *  Resource access functions
 *
 */

/* Read data */
int  ResReadData(int ulSock, char* pzRes, ResVal** ppVal, int offs);

/* Write data */
int  ResWriteData(int ulSock, char* pzRes, ResVal* pVal, int offs);

/* Create instance */
int  ResCreateInstance(int ulSock, char* pzRes, char** ppzNewName);

/* Delete instance */
int  ResDeleteInstance(int ulSock, char* pzRes);

/* Read attributes */
int  ResReadAttributes(int ulSock, char* pzRes, ResAttr* pAttr);

/* Send authentication */
int  ResSendAuth(int ulSock, char* username, char* passwd, int offs);

/* Add subscriber notification */
int  ResAddSubscriber(int ulSock, char* pzRes, int bForever, unsigned long ulMagic,
                      unsigned short usPort, int offs);

/* Remove subscriber notification */
int  ResRemoveSubscriber(int ulSock, char* pzRes, int bForever, unsigned long ulMagic,
                      unsigned short usPort, int offs);

/* This call will block while waiting for notify */
int  ResWaitForNotify(unsigned short usPort, 
                      struct in_addr* pAddr,
                      unsigned long* pulMagic,
                      ResVal** ppVal);

#ifdef __cplusplus
}
#endif

#endif /* _RES_LIB_H */
