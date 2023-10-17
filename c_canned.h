/*******************************************************************************
 ** Name: c_canned.h
 ** Purpose:  Wrapps can sockets.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 14.06.2020  JE    Created version 0.1.2
 *******************************************************************************/


//******************************************************************************
//* header

#ifndef C_CANNED_H
#define C_CANNED_H


//******************************************************************************
//* includes

#define __USE_MISC  1 // For 'struct ifreq'.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
//#include <linux/can/raw.h>

#include "c_string.h"


//******************************************************************************
//* defines and macros

#define C_CANNED_OK         0x00
#define C_CANNED_ERR_SOCKET 0x01
#define C_CANNED_ERR_READ   0x02
#define C_CANNED_ERR_WRITE  0x03
#define C_CANNED_ERR_DATA   0x04
#define C_CANNED_ERR        (-1)


//******************************************************************************
//* type definition

// Central struct, which defines a can 'object'.
typedef struct s_can {
  int                 hSocket;
  cstr                csIfName;
  struct sockaddr_can sockAddrCan;
  struct can_frame    canFrame;
  struct ifreq        ifRequest;
  int                 iErr;
  cstr                csErr;
} t_can;


//******************************************************************************
//* function forward declarations
//* For a better function's arrangement.

// Internal functions.
static void can_init(t_can* ptCan, const char* pcIfName);

// External functions.

// Init & destroy.
int  canNew(t_can* ptCan, const char* pcIfName);
int canFree(t_can* ptCan);

// Interface.
int  canRead(t_can* ptCan);
int canWrite(t_can* ptCan);


//******************************************************************************
//* private functions

/*******************************************************************************
 * Name: can_init
 *******************************************************************************/
void can_init(t_can* ptCan, const char* pcIfName) {
  ptCan->hSocket     = 0;
  ptCan->sockAddrCan = (struct sockaddr_can) {0};
  ptCan->canFrame    = (struct can_frame) {0};
  ptCan->ifRequest   = (struct ifreq) {0};
  ptCan->csIfName    = csNew(pcIfName);
  ptCan->iErr        = C_CANNED_OK;
  ptCan->csErr       = csNew("");
}


//******************************************************************************
//* public functions


//******************************************************************************
//* Init & destroy functions.

/*******************************************************************************
 * Name: canNew
 *******************************************************************************/
int canNew(t_can* ptCan, const char* pcIfName) {
  can_init(ptCan, pcIfName);

  if ((ptCan->hSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    ptCan->iErr = C_CANNED_ERR_SOCKET;
    csSet(&ptCan->csErr, "Couldn't open socket");
    return C_CANNED_ERR;
  }

  strcpy(ptCan->ifRequest.ifr_name, pcIfName);
  ioctl(ptCan->hSocket, SIOCGIFINDEX, &ptCan->ifRequest);

  ptCan->sockAddrCan.can_family  = AF_CAN;
  ptCan->sockAddrCan.can_ifindex = ptCan->ifRequest.ifr_ifindex;

  if (bind(ptCan->hSocket, (struct sockaddr*) &ptCan->sockAddrCan, sizeof(ptCan->sockAddrCan)) < 0) {
    ptCan->iErr = C_CANNED_ERR_SOCKET;
    csSet(&ptCan->csErr, "Couldn't bind socket");
    return C_CANNED_ERR;
  }

  return C_CANNED_OK;
}

/*******************************************************************************
 * Name: csFree
 * Purpose: Deletes cstr object and frees memory used.
 *******************************************************************************/
int canFree(t_can* ptCan) {
  if(close(ptCan->hSocket) < 0) {
    ptCan->iErr = C_CANNED_ERR_SOCKET;
    csSet(&ptCan->csErr, "Couldn't close socket");
    return C_CANNED_ERR;
  }
  return C_CANNED_OK;
}


//******************************************************************************
//* Interface.

/*******************************************************************************
 * Name: canRead
 *******************************************************************************/
int canRead(t_can *ptCan) {
  if (read(ptCan->hSocket, &ptCan->canFrame, sizeof(struct can_frame)) < 0) {
    ptCan->iErr = C_CANNED_ERR_READ;
    csSet(&ptCan->csErr, "Couldn't read data");
    return C_CANNED_ERR;
  }
  return C_CANNED_OK;
}

/*******************************************************************************
 * Name: canWrite
 *******************************************************************************/
int canWrite(t_can* ptCan) {
  if (write(ptCan->hSocket, &ptCan->canFrame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
    ptCan->iErr = C_CANNED_ERR_WRITE;
    csSet(&ptCan->csErr, "Couldn't write data");
    return C_CANNED_ERR;
  }
  return C_CANNED_OK;
}

#endif // C_CANNED_H
