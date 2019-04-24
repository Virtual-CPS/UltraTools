#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "FlirResourceLib.h"

#define PRINTF printf
#define BUFLEN 255

/* #define RES_DEBUG */

/* network-to-host-double */
double ntohd(double d)
{
    double dconv;
    char* p = (char*) &d;
    char* q = (char*) &dconv;
    int i = 0;

    for (i = 0; i < sizeof(d); i++)
        q[i] = p[sizeof(d) - i - 1];

    return dconv;
}

/* host-to-network-double */
double htond(double d)
{
    return ntohd(d);
}

/* Create values
 */
ResVal* ResCreateVal()
{
    return (ResVal* ) malloc(sizeof(ResVal));
}

ResVal* ResCreateBoolVal(char val)
{
    ResVal* rval = ResCreateVal();
    rval->type = RES_BOOL;
    rval->b = val;
    return rval;
}

ResVal* ResCreateIntVal(int val)
{
    ResVal* rval = ResCreateVal();
    rval->type = RES_INT32;
    rval->i = val;
    return rval;
}

ResVal* ResCreateDoubleVal(double val)
{
    ResVal* rval = ResCreateVal();
    rval->type = RES_DOUBLE;
    rval->d = val;
    return rval;
}

ResVal* ResCreateTextVal(char* val)
{
    ResVal* rval = ResCreateVal();
    rval->type = RES_TEXT;
    rval->t = (char*) malloc(strlen(val) + 1);
    strcpy(rval->t, val);
    return rval;
}

ResVal* ResCreateUnicodeVal(wchar_t *val)
{
    ResVal* rval = ResCreateVal();
    rval->type = RES_UNICODE;
	size_t len = wcslen(val) + 1;
	len = len * sizeof(wchar_t);
	rval->u = (wchar_t*) malloc(len);
    wcscpy(rval->u, val);
    return rval;
}

/* Delete value
 */
void ResDeleteVal(ResVal* pVal)
{
    if (pVal->type == RES_TEXT && pVal->t != NULL)
        free(pVal->t);

    free(pVal);
}


/* Print value
 */
int ResPrintVal(ResVal* pVal)
{
    switch (pVal->type)
    {
    case RES_BOOL:
        if (pVal->b)
            PRINTF("true");
        else
            PRINTF("false");
        break;
        
    case RES_INT32:
        PRINTF("%d", pVal->i);        
        break;
        
    case RES_DOUBLE:
        PRINTF("%lf", pVal->d);
        break;
        
    case RES_TEXT:
        PRINTF("%s", pVal->t);        
        break;

    case RES_UNICODE:
        PRINTF("%S", pVal->u);        
        break;
	}

    return 0;
}

/* Create and connect socket to camera
 */
int ResOpenSocket(char* pzAddr, unsigned short usPort)
{
    SOCKET s;
    int i;
    unsigned long iaddr = 0;
    struct sockaddr_in sa;
    WSADATA WSA;
    fd_set fds;
    struct timeval ti;
    unsigned long sockarg;

    if (WSAStartup (MAKEWORD(2,0), &WSA) != 0) 
    {
        PRINTF ("WSAStartup failed!");
        return RES_COMM_ERR_WSASTARTUP;
    }

#ifdef RES_DEBUG
    PRINTF ("WSA initialized\n");
#endif

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        PRINTF ("socket: h_errno = %d\n", h_errno);
        return (int)s;
    }

    /* Set socket in nonblocking mode */
    sockarg = 1;
    if (i = ioctlsocket(s, FIONBIO, &sockarg) < 0)
    {
        PRINTF ("ioctlsocket: h_errno = %d\n", h_errno);
        closesocket(s);
        return i;
    }

    /* Set socket in fd struct */
    FD_ZERO(&fds);
    FD_SET(s, &fds);
    
#ifdef RES_DEBUG
    PRINTF ("Socket created (%d)\n", s);
#endif
        
    if ((iaddr = inet_addr(pzAddr)) == ~0)
    {
        struct hostent* hent = gethostbyname(pzAddr);
        if (hent)
        {
            memcpy(&iaddr, hent->h_addr, 4);
        }
        else
        {
            PRINTF("gethostbyname: can't resolve host\n");
            return RES_COMM_ERR_RESOLVE;
        }
    }

#ifdef RES_DEBUG
    /* Print resolved IP in network byte order */
    PRINTF ("INET address: %d.%d.%d.%d\n",
            iaddr & 0xff, (iaddr >> 8) & 0xff, (iaddr >> 16) & 0xff, iaddr >> 24);
#endif
    
    sa.sin_addr.s_addr = iaddr;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(usPort);
    for (i = 0; i < sizeof(sa.sin_zero); i++)
        sa.sin_zero[i] = 0;

    /* Set select timeout to 2 seconds */
    ti.tv_sec = 2;
    ti.tv_usec = 0;
    
    if ((i = connect(s, (struct sockaddr *)&sa, sizeof(sa))) < 0)
    {
        if (h_errno == WSAEWOULDBLOCK)
        {
            i = select(1, NULL, &fds, NULL, &ti);
            if (i < 0)
            {
#ifdef RES_DEBUG
                PRINTF ("select: h_errno = %d\n", h_errno);
#endif
                closesocket(s);
                return i;
            }
            else if (i == 0)
            {
                /* PRINTF ("select: timeout\n"); */
                closesocket(s);
                return -1;
            }
        }
        else
        {
#ifdef RES_DEBUG
            PRINTF ("connect: h_errno = %d\n", h_errno);
#endif
            closesocket(s);
            return i;
        }
    }

#ifdef RES_DEBUG
    PRINTF ("Connected to camera\n");
#endif

    /* Reset socket to blocking mode */
    sockarg = 0;
    if (i = ioctlsocket(s, FIONBIO, &sockarg) < 0)
    {
        PRINTF ("ioctlsocket: h_errno = %d\n", h_errno);
        closesocket(s);
        return i;
    }
    
    return (int)s;
}


/* Close socket
 */
void ResCloseSocket(int ulSock)
{
    closesocket(ulSock);
    WSACleanup();
}

/* Read resource value
 */
int ResReadData(int ulSock, char* pzRes, ResVal** ppVal, int offs)
{
    int   res;
    char* sendbuf;
    unsigned char buf[BUFLEN];
    int   len;

    len = offs + (int)strlen(pzRes) + 2;
	sendbuf = (char *) malloc(len);

    /* Write header + resource path */
	if (offs > 0)
	{
	    sendbuf[0] = (char)0xFE;
		sendbuf[1] = 0;
	    sendbuf[2] = 0;
		sendbuf[3] = 0;
		long l = htonl(len - offs);
		memcpy(&sendbuf[4], (char *)&l, sizeof(long));
	}

	sendbuf[offs] = RESLIB_COMM_READ_DATA;
    strcpy (&sendbuf[offs + 1], pzRes);

    if ((res = send(ulSock, sendbuf, len, 0)) < 0)
    {
#ifdef RES_DEBUG
        PRINTF ("send: h_errno = %d\n", h_errno);
#endif
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif
        
    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return len;
    }

#ifdef RES_DEBUG
    PRINTF ("Data received from camera (opcode 0x%X, datatype 0x%X, len %d)\n",
		buf[offs] & 0xff, buf[offs + 1] & 0xff, (len-offs));
#endif
        
    if (buf[offs] == RESLIB_COMM_READ_RESPONSE && ((len - offs) > 2))
    {
        switch (buf[offs + 1])
        {
        case RES_BOOL:
            *ppVal = ResCreateBoolVal(buf[offs + 2]);
            break;

        case RES_INT32:
            *ppVal = ResCreateIntVal(ntohl(* (int*) &buf[offs + 2]));
            break;

        case RES_DOUBLE:
            *ppVal = ResCreateDoubleVal(ntohd(* (double*) &buf[offs + 2]));
            break;

        case RES_TEXT:
            *ppVal = ResCreateTextVal((char *)&buf[offs + 2]);
            break;

	    case RES_UNICODE:
            *ppVal = ResCreateUnicodeVal((wchar_t *)&buf[offs + 2]);
			break;

        case RES_ENTRY:
        case RES_INDEX:
        {
            int c = offs + 2;

            /* Replace NULLs between strings with newline */
            while (c < ((len - offs) - 1))
            {
                if (buf[c] == '\0')
                    buf[c] = '\n';
                c++;
            }

            *ppVal = ResCreateTextVal((char *)&buf[offs + 2]);

            break;
        }
        default:
            return RES_COMM_ERR_UNKNOWN_TYPE;
            break;
        }
    }
    else if (buf[offs] ==  RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[offs + 1];
    }
    else
    {
        return RES_COMM_ERR_UNKNOWN_TYPE;
    }

    return RES_COMM_OK;
}

/* Write value to resource
 */
int ResWriteData(int ulSock, char* pzRes, ResVal* pVal, int offs)
{
    int   res;
    char* sendbuf;
    unsigned char buf[BUFLEN];
    size_t   len;

    /* Calculate length for header + resource path + NULL */
    len = offs + 2 + strlen(pzRes) + 1;

    /* Allocate buffer and copy in value */
    switch (pVal->type)
    {
    case RES_BOOL:
        sendbuf = (char *) malloc(len + sizeof(char));
        sendbuf[len] = pVal->b;
        len += sizeof(char);
        break;

    case RES_INT32:
    {
        int val = htonl(pVal->i);
        sendbuf = (char *) malloc(len + sizeof(int));
        memcpy(&sendbuf[len], &val, sizeof(int));
        len += sizeof(int);
        break;
    }

    case RES_DOUBLE:
    {
        double val = htond(pVal->d);
        sendbuf = (char *) malloc(len + sizeof(double));
        memcpy(&sendbuf[len], &val, sizeof(double));
        len += sizeof(double);
        break;
    }

    case RES_TEXT:
        sendbuf = (char *) malloc(len + strlen(pVal->t) + 1);
        strcpy(&sendbuf[len], pVal->t);
        len += strlen(pVal->t) + 1;
        break;

    case RES_UNICODE:
	{
		size_t strlen;
		strlen = (wcslen(pVal->u) + 1) * sizeof(wchar_t);
		sendbuf = (char *) malloc(len + strlen);
        memcpy(&sendbuf[len], pVal->u, strlen);
        len += strlen;
		break;
	}

    default:
        return RES_COMM_ERR_UNKNOWN_TYPE;
        break;
    }

    /* Write header + resource path */
	if (offs > 0)
	{
	    sendbuf[0] = (char)0xFE;
		sendbuf[1] = 0;
	    sendbuf[2] = 0;
		sendbuf[3] = 0;
		long l = htonl((u_long)(len - offs));
		memcpy(&sendbuf[4], (char *)&l, sizeof(long));
	}

    sendbuf[offs] = RESLIB_COMM_WRITE_DATA;
    sendbuf[offs+1] = pVal->type;
    strcpy (&sendbuf[offs+2], pzRes);

    if ((res = send(ulSock, sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif
        
    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

    /* Check ack */
    if ((buf[offs] == RESLIB_COMM_WRITE_RESPONSE) && (buf[offs+1] == 0x00))
    {
        return RES_COMM_OK;
    }
    else if (buf[offs] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[offs+1] & 0xff;
    }

    return RES_COMM_ERR_UNKNOWN_TYPE;
}


/* Create new instance of resource
 */
int ResCreateInstance(int ulSock, char* pzRes, char** ppzNewName)
{
    int   res;
    char* sendbuf;
    unsigned char buf[BUFLEN];
    size_t   len;

    len = strlen(pzRes) + 2;
    sendbuf = (char *) malloc(len);

    sendbuf[0] = RESLIB_COMM_CREATE_INSTANCE;
    strcpy (&sendbuf[1], pzRes);

    if ((res = send(ulSock, sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif

    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

#ifdef RES_DEBUG
    PRINTF ("Data received from camera (opcode 0x%X, errcode 0x%X)\n", buf[0] & 0xff, buf[1] & 0xff);
#endif

    if (buf[0] == RESLIB_COMM_CREATE_RESPONSE)
    {
	    if (buf[1] == 0x00)
		{
	        *ppzNewName = (char *) malloc(strlen((char *)&buf[2]) + 1);
		    strcpy(*ppzNewName, (char *)&buf[2]);
		}
		else
		{
	        return buf[1] & 0xff;
		}
    }
    else if (buf[0] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[1] & 0xff;
    }
    else
    {
        return RES_COMM_ERR_UNKNOWN_TYPE;
    }

    return RES_COMM_OK;
}

/* Delete resource instance
 */
int ResDeleteInstance(int ulSock, char* pzRes)
{
    int   res;
    char* sendbuf;
    unsigned char buf[BUFLEN];
    size_t   len;

    len = strlen(pzRes) + 2;
    sendbuf = (char *) malloc(len);

    sendbuf[0] = RESLIB_COMM_DELETE_INSTANCE;
    strcpy (&sendbuf[1], pzRes);

    if ((res = send(ulSock, sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif
        
    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

#ifdef RES_DEBUG
    PRINTF ("Data received from camera (opcode 0x%X, errcode 0x%X)\n", buf[0] & 0xff, buf[1] & 0xff);
#endif

    if ((buf[0] == RESLIB_COMM_DELETE_RESPONSE) && (buf[1] == 0x00))
    {
        return RES_COMM_OK;
    }
    else if (buf[0] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[1] & 0xff;
    }
    else
    {
        return RES_COMM_ERR_UNKNOWN_TYPE;
    }
}

/* Get resource attributes
 */
int ResReadAttributes(int ulSock, char* pzRes, ResAttr* pAttr)
{
    int   res;
    char* sendbuf;
    unsigned char buf[BUFLEN];
    size_t   len;

    len = strlen(pzRes) + 2;
    sendbuf = (char *) malloc(len);

    sendbuf[0] = RESLIB_COMM_READ_ATTRIBUTES;
    strcpy (&sendbuf[1], pzRes);

    if ((res = send(ulSock, sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif
        
    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

#ifdef RES_DEBUG
    PRINTF ("Data received from camera (opcode 0x%X, datatype 0x%X)\n", buf[0] & 0xff, buf[1] & 0xff);
#endif

    if (len > 16 && buf[0] == RESLIB_COMM_READ_ATTR_RESPONSE)
    {
        pAttr->type            = buf[1];
        pAttr->getSupported    = buf[2];
        pAttr->setSupported    = buf[3];
        pAttr->createSupported = buf[4];
        pAttr->deleteSupported = buf[5];
        pAttr->subscrSupported = buf[6];
        pAttr->persistent      = buf[7];
        memcpy(&(pAttr->openCount), &buf[8], 4);
        pAttr->openCount = ntohl(pAttr->openCount);
        memcpy(&(pAttr->subscrCount), &buf[12], 4);
        pAttr->subscrCount = ntohl(pAttr->subscrCount);
        pAttr->sync            = buf[16];
    }
    else if (buf[0] == RESLIB_COMM_ERROR_RESPONSE)
    {
#ifdef RES_DEBUG
        if (len > 2)
            PRINTF ("Read attr response was 0x%x 0x%x 0x%x\n", buf[0], buf[1], buf[2]);
#endif
        return buf[1];
    }
    else
    {
        return RES_COMM_ERR_UNKNOWN_TYPE;
    }

    return RES_COMM_OK;
}

/* Send host authorization
 */
int ResSendAuth(int ulSock, char* username, char* passwd, int offs)
{
    int   res;
    PBYTE sendbuf;
    unsigned char buf[BUFLEN];
    size_t   len;
    
    /* Calculate length for header + user + passwd */
    len = 1 + offs + 2 + strlen(username) + strlen(passwd);

    /* Allocate buffer and copy in value */
    sendbuf = (PBYTE) malloc(len + sizeof(int));
	memset(sendbuf, 0, len);

    /* Write header + user + passwd */
    sendbuf[offs] = (UCHAR)RESLIB_COMM_AUTH_HOST;
    sendbuf[offs+1] = (BYTE)strlen(username);
    strcpy ((char *)&sendbuf[offs+2], username);
    sendbuf[offs + 2 + sendbuf[offs+1]] = (BYTE)strlen(passwd);
    strcpy ((char *)&sendbuf[offs + 2 + sendbuf[offs+1] + 1], passwd);

	if (offs > 0)
	{
		sendbuf[0] = (char)0xFE;
		sendbuf[1] = 0;
	    sendbuf[2] = 0;
		sendbuf[3] = 0;
		long l = htonl((u_long)(len - offs));
		memcpy(&sendbuf[4], (char *)&l, sizeof(long));
	}

    if ((res = send(ulSock, (const char *)sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif
        
    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

    /* Check ack */
    if ((buf[offs] == RESLIB_COMM_AUTH_HOST_RESPONSE) && (buf[offs+1] == 0x00))
    {
        return RES_COMM_OK;
    }
    else if (buf[offs] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[offs+1] & 0xff;
    }

    return RES_COMM_ERR_UNKNOWN_TYPE;
}

int  ResAddSubscriber(int ulSock, char* pzRes, int bForever, unsigned long ulMagic,
                      unsigned short usPort, int offs)
{
    int   res;
    PBYTE sendbuf;
    unsigned char buf[BUFLEN];
    size_t  len;
    
    /* Calculate length for header + resname */
    len = offs + 1 + 9 + strlen(pzRes);
    
	/* Allocate buffer and copy in value */
    sendbuf = (PBYTE)malloc(len);

	if (offs > 0)
	{
		sendbuf[0] = (char)0xFE;
		sendbuf[1] = 0;
	    sendbuf[2] = 0;
		sendbuf[3] = 0;
		long l = htonl((u_long)(len - offs));
		memcpy(&sendbuf[4], (char *)&l, sizeof(long));
	}

    /* Write header + resname */
    ulMagic = htonl(ulMagic);
    usPort = htons(usPort);
    sendbuf[offs + 0] = (UCHAR)RESLIB_COMM_ADD_SUBSCR;
    memcpy(&(sendbuf[offs + 1]), &ulMagic, sizeof(long));
    memcpy(&(sendbuf[offs + 5]), &usPort, sizeof(short));
    sendbuf[offs + 7] = bForever;
    sendbuf[offs + 8] = 0; /* UDP */
    sendbuf[offs + 9] = (BYTE)strlen(pzRes);
    strncpy((char *)&(sendbuf[offs + 10]), pzRes, strlen(pzRes));

    if ((res = send(ulSock, (char *)sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif

    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

    /* Check ack */
    if ((buf[offs + 0] == RESLIB_COMM_ADD_SUBSCR_RESPONSE) &&
		(buf[offs + 1] == 0x00))
    {
        return RES_COMM_OK;
    }
    else if (buf[offs + 0] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[offs + 1] & 0xff;
    }

    return RES_COMM_ERR_UNKNOWN_TYPE;
}


int  ResRemoveSubscriber(int ulSock, char* pzRes, int bForever, unsigned long ulMagic,
                        unsigned short usPort, int offs)
{
    int   res;
    PBYTE sendbuf;
    BYTE buf[BUFLEN];
    size_t   len;
    
    /* Calculate length for header + resname */
    len = offs + 1 + 9 + strlen(pzRes);
    
    /* Allocate buffer and copy in value */
    sendbuf = (PBYTE)malloc(len);

	if (offs > 0)
	{
		sendbuf[0] = (char)0xFE;
		sendbuf[1] = 0;
	    sendbuf[2] = 0;
		sendbuf[3] = 0;
		long l = htonl((u_long)(len - offs));
		memcpy(&sendbuf[4], (char *)&l, sizeof(long));
	}

	/* Write header + resname */
    ulMagic = htonl(ulMagic);
    usPort = htons(usPort);
    sendbuf[offs + 0] = (UCHAR)RESLIB_COMM_REMOVE_SUBSCR;
    memcpy(&(sendbuf[offs + 1]), &ulMagic, sizeof(long));
    memcpy(&(sendbuf[offs + 5]), &usPort, sizeof(short));
    sendbuf[offs + 7] = bForever;
    sendbuf[offs + 8] = 0; /* UDP */
    sendbuf[offs + 9] = (BYTE)strlen(pzRes);
    strncpy((char *)&(sendbuf[offs + 10]), pzRes, strlen(pzRes));

    if ((res = send(ulSock, (const char *)sendbuf, (int)len, 0)) < 0)
    {
        PRINTF ("send: h_errno = %d\n", h_errno);
        return res;
    }

#ifdef RES_DEBUG
    PRINTF ("Request sent to camera\n");
#endif

    free(sendbuf);

    if ((len = recv(ulSock, (char *)buf, BUFLEN, 0)) < 0)
    {
        PRINTF ("recv: h_errno = %d\n", h_errno);
        return (int)len;
    }

    /* Check ack */
    if ((buf[offs + 0] == RESLIB_COMM_REMOVE_SUBSCR_RESPONSE) &&
		(buf[offs + 1] == 0x00))
    {
        return RES_COMM_OK;
    }
    else if (buf[offs + 0] == RESLIB_COMM_ERROR_RESPONSE)
    {
        return buf[offs + 1] & 0xff;
    }

    return RES_COMM_ERR_UNKNOWN_TYPE;
}


int  ResWaitForNotify(unsigned short usPort, 
                      struct in_addr* pAddr,
                      unsigned long* pulMagic,
                      ResVal** ppVal)
{
    ReslibSubscrMsg msg;
    SOCKET s;
    WSADATA WSA;
    struct sockaddr_in sa, ca;
    int clen = sizeof(ca);
    int   i;
    
    if (WSAStartup (MAKEWORD(2,0), &WSA) != 0) 
    {
        PRINTF ("WSAStartup failed!");
        return RES_COMM_ERR_WSASTARTUP;
    }

#ifdef RES_DEBUG
    PRINTF ("WSA initialized\n");
#endif

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        PRINTF ("socket: h_errno = %d\n", h_errno);
        return INVALID_SOCKET;
    }

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(usPort);
    for (i = 0; i < sizeof(sa.sin_zero); i++)
        sa.sin_zero[i] = 0;

    if (bind(s, (struct sockaddr*) &sa, sizeof(sa)) < 0)
    {
        PRINTF ("bind: h_errno = %d\n", h_errno);
        return -1;
    }

    i = recvfrom(s, (char*) &msg, sizeof(ReslibSubscrMsg), 0, (struct sockaddr*) &ca, &clen);
    closesocket(s);
    
    if (i == sizeof(ReslibSubscrMsg))
    {
        *pAddr = ca.sin_addr;

        /* set magic */
        *pulMagic = ntohl(msg.magic);
        
        switch (ntohl(msg.type))
        {
        case RES_BOOL:
            *ppVal = ResCreateBoolVal((char) ntohl(msg.b));
            break;

        case RES_INT32:
            *ppVal = ResCreateIntVal(ntohl(msg.i));
            break;

        case RES_DOUBLE:
            *ppVal = ResCreateDoubleVal(ntohd(msg.d));
            break;

        case RES_TEXT:
            *ppVal = ResCreateTextVal(msg.c);
            break;
        }

        return 0;
    }

    return -1;
}
