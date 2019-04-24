/***********************************************************************
 *
 * Project:   Matrix
 * $Workfile: res_protocol.h $
 *
 * Description of file:
 *    Binary protocol definitions for PC - Matrix Camera communication
 *
 * 
 * Copyright: FLIR Systems
 ***********************************************************************/

#ifndef _FLIR_RES_PROTOCOL_H
#define _FLIR_RES_PROTOCOL_H

/* Port number
 */
#define RESLIB_COMM_PORT               0x5678

/* Commands and responses
 */
#define RESLIB_COMM_READ_DATA            0x01
#define RESLIB_COMM_READ_RESPONSE        0x02

#define RESLIB_COMM_WRITE_DATA           0x11
#define RESLIB_COMM_WRITE_RESPONSE       0x12
#define RESLIB_COMM_WRITE_DATA_WAIT      0x13

#define RESLIB_COMM_CREATE_INSTANCE      0x21
#define RESLIB_COMM_CREATE_RESPONSE      0x22
#define RESLIB_COMM_CREATE_INSTANCE_WAIT 0x23

#define RESLIB_COMM_DELETE_INSTANCE      0x31
#define RESLIB_COMM_DELETE_RESPONSE      0x32
#define RESLIB_COMM_DELETE_INSTANCE_WAIT 0x33

#define RESLIB_COMM_READ_ATTRIBUTES      0x41
#define RESLIB_COMM_READ_ATTR_RESPONSE 	 0x42

#define RESLIB_COMM_SET_ATTRIBUTES       0x43
#define RESLIB_COMM_SET_ATTR_RESPONSE 	 0x44

#define RESLIB_COMM_ADD_CHILD  		 0x51
#define RESLIB_COMM_ADD_CHILD_RESPONSE 	 0x52

#define RESLIB_COMM_DUMP_RES   	         0x61
#define RESLIB_COMM_DUMP_RES_RESPONSE 	 0x62

#define RESLIB_COMM_RELOAD   	         0x71
#define RESLIB_COMM_RELOAD_RESPONSE 	 0x72

#define RESLIB_COMM_COPY   	         0x81
#define RESLIB_COMM_COPY_RESPONSE 	 0x82

#define RESLIB_COMM_RENAME               0x91
#define RESLIB_COMM_RENAME_RESPONSE      0x92

#define RESLIB_COMM_ADD_SUBSCR           0xA0
#define RESLIB_COMM_ADD_SUBSCR_RESPONSE  0xA1

#define RESLIB_COMM_REMOVE_SUBSCR           0xA2
#define RESLIB_COMM_REMOVE_SUBSCR_RESPONSE  0xA3

#define RESLIB_COMM_GET_COMPLETE_RESOURCE_TREE   0xB0
/* No response for this? */

#define RESLIB_COMM_SESSION_OPEN         0xC0
#define RESLIB_COMM_SESSION_OPEN_RESPONSE 0xC1

#define RESLIB_COMM_SESSION_CLOSE        0xC2
#define RESLIB_COMM_SESSION_CLOSE_RESPONSE 0xC3

#define RESLIB_COMM_SESSION_COMMAND      0xC4
#define RESLIB_COMM_SESSION_COMMAND_RESPONSE 0xC5

#define RESLIB_COMM_AUTH_HOST            0xD0
#define RESLIB_COMM_AUTH_HOST_RESPONSE   0xD1

#define RESLIB_COMM_ERROR_RESPONSE       0xFF

/* Error codes
 */
#define RESLIB_COMM_PATH_NOT_FOUND       0xA0
#define RESLIB_COMM_GET_NOT_SUPPORTED    0xA1
#define RESLIB_COMM_SET_NOT_SUPPORTED    0xA2
#define RESLIB_COMM_CREATE_NOT_SUPPORTED 0xA3
#define RESLIB_COMM_DELETE_NOT_SUPPORTED 0xA4
#define RESLIB_COMM_OPERATION_FAILED     0xA5
#define RESLIB_COMM_OPCODE_NOT_FOUND     0xA6
#define RESLIB_COMM_UNKNOWN_TYPE         0xA7
#define RESLIB_COMM_TYPE_MISMATCH        0xA8
#define RESLIB_COMM_WRONG_INDATA_SIZE    0xA9
#define RESLIB_COMM_PERMISSION_DENIED    0xAA
#define RESLIB_COMM_AUTH_FAILED          0xAB
#define RESLIB_COMM_SESSION_FAILED       0xAC

/* Structures
 */

#define RESLIB_SUBSCR_CHAR_MAXLEN  7
#define RESLIB_SUBSCR_WCHAR_MAXLEN 3

/* Subscriber notification message
   size = 4 + 4 + 8 = 16 byte */
typedef struct ReslibSubscrMsg
{
    /** This is the type of the triggered resource */
    long type;
    
    /** The magic number as given when subscription was set up */
    unsigned long magic;
    
    union
    {
        long   i;
        long   b;
        double d;
        char   c[RESLIB_SUBSCR_CHAR_MAXLEN + 1];
        unsigned short w[RESLIB_SUBSCR_WCHAR_MAXLEN + 1];
    };
} ReslibSubscrMsg;


#endif /* _FLIR_RES_PROTOCOL_H */
