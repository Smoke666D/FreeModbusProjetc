/*
 * FreeModbus Libary: lwIP Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttcp.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>

#include "port.h"

/* ----------------------- lwIP includes ------------------------------------*/
#include "eth_driver.h"
#include "wchnet.h"
#include "main.h"
#include "string.h"
#include "task.h"
#include "event_groups.h"
//#include "lwip/api.h"
//#include "lwip/tcp.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502 /* TCP listening port. */
#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */



u8 MACAddr[6];                                          //MAC address
u8 IPAddr[4] = {192, 168, 1, 10};                       //IP address
u8 GWIPAddr[4] = {192, 168, 1, 1};                      //Gateway IP address
u8 IPMask[4] = {255, 255, 255, 0};

#define PROG                    "FreeModbus"
//u8 MACAddr[6];                                              //MAC address
                          //destination IP address
u16 desport = 1000;                                         //destination port
//u16 srcport = 1000;                                         //source port

#define MAX_SOCKET_NUM WCHNET_MAX_SOCKET_NUM
#define MB_TCP_CLIENTS  MAX_SOCKET_NUM


u8 SocketIdForListen = 0xFF;
u8 SocketIdClient = 0xFF;
                          //Save the currently connected socket
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];
//u8 SocketRecvBuf[RECE_BUF_LEN];      //socket receive buffer
//u8 MyBuf[RECE_BUF_LEN];
static UCHAR    aucTCPBuf[MB_TCP_BUF_SIZE];
static USHORT   usTCPBufPos;

static UCHAR    outTCPBuf[MB_TCP_BUF_SIZE];

static EventGroupHandle_t xPortTCPOSEventGroup;


void prvvMBPortReleaseClient(u8 * SocketID );

void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}



void vNetInit()
{
    u8 i;
    struct _KEEP_CFG cfg;
    WCHNET_GetMacAddr(MACAddr);                                   //get the chip MAC address
    printf("mac addr:");
    for( i = 0; i < 6; i++)
          printf("%x ",MACAddr[i]);
          printf("\n");
     TIM2_Init();

     i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);           //Ethernet library initialize
     mStopIfError(i);
     if (i == WCHNET_ERR_SUCCESS)
           printf("WCHNET_LibInit Success\r\n");
      cfg.KLIdle = 20000;
      cfg.KLIntvl = 15000;
      cfg.KLCount = 9;
      WCHNET_ConfigKeepLive(&cfg);
      xPortTCPOSEventGroup= xGetOSEvent();

}


void InitNet()
{


}
void mStopIfError1(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

uint32_t Len;
u8 * pData;
/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;
    u32 len;
    USHORT          usLength;
    u8 sss = socketid;
    if (intstat & SINT_STAT_RECV)                               //receive data
    {

        u32 endAddr = SocketInf[socketid].RecvStartPoint + SocketInf[socketid].RecvBufLen;
        if ((SocketInf[socketid].RecvReadPoint + SocketInf[socketid].RecvRemLen) > endAddr) {    //Calculate the length of the received data
            len = endAddr - SocketInf[socketid].RecvReadPoint;
        }
        else {
            len = SocketInf[socketid].RecvRemLen;
        }




        /* Check for internal buffer overflow. In case of an error drop the
         * client. */
       if( ( usTCPBufPos + len) >= MB_TCP_BUF_SIZE )
        {
            prvvMBPortReleaseClient( &socketid);
            //error = ERR_OK;
        }
        else
        {

            WCHNET_SocketRecv(socketid,&aucTCPBuf[usTCPBufPos],&len);
            usTCPBufPos +=len;

            /* If we have received the MBAP header we can analyze it and calculate
             * the number of bytes left to complete the current request. If complete
             * notify the protocol stack.
             */
           if( usTCPBufPos >= MB_TCP_FUNC )
            {
                /* Length is a byte count of Modbus PDU (function code + data) and the
                 * unit identifier. */
                usLength = aucTCPBuf[MB_TCP_LEN] << 8U;
                usLength |= aucTCPBuf[MB_TCP_LEN + 1];

                /* Is the frame already complete. */
                if( usTCPBufPos < ( MB_TCP_UID + usLength ) )
                {
                }
                else if( usTCPBufPos == ( MB_TCP_UID + usLength ) )
                {
                    u8 J;
                    ( void )xMBPortEventPost( EV_FRAME_RECEIVED );
                    xEventGroupWaitBits(xPortTCPOSEventGroup,DATA_READY,pdTRUE,pdTRUE,portMAX_DELAY);

                    J = WCHNET_SocketSend(sss, outTCPBuf, &Len );

                    mStopIfError(J);
                        if ( J == WCHNET_ERR_SUCCESS )
                        {


                           //  bFrameSent = TRUE;
                         }
                         else
                         {
                             mStopIfError(J);

                             prvvMBPortReleaseClient( &socketid  );
                         }
                        xEventGroupSetBits(xPortTCPOSEventGroup,DATA_SEND);
                }
                else
                {
                    /* This should not happen. We can't deal with such a client and
                     * drop the connection for security reasons.
                     */
                   prvvMBPortReleaseClient( &socketid );
                }
            }
        }


    }
    if (intstat & SINT_STAT_CONNECT)                            //connect successfully
    {
       if (SocketIdClient==0xFF)
       {
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);

        SocketIdClient = socketid;

        printf("TCP Connect Success\r\n");
        printf("socket id: %d\r\n", SocketIdClient);
       }
    }
    if (intstat & SINT_STAT_DISCONNECT)                         //disconnect
    {
        SocketIdClient =0xFF;
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                            //timeout disconnect
    {
        SocketIdClient =0xFF;
        printf("TCP Timeout\r\n");
        //WCHNET_CreateTcpSocket();
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();                            //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                            //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                          //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }
    if (intstat & GINT_STAT_PHY_CHANGE)                         //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
        {
            printf("PHY Link Success\r\n");
           // xEventGroupSetBits(xPortTCPOSEventGroup,TCP_START_TASK);
        }
    }
    if (intstat & GINT_STAT_SOCKET) {                           //socket related interrupt
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}


//u8 socket[WCHNET_MAX_SOCKET_NUM];
//
void MBTCP_task(void *pvParameters)
{

     eMBErrorCode    xStatus;
    // memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
   //  WCHNET_CreateTcpSocketListen();
     for( ;; )
     {

      //   printf("task1 entry\r\n");
       //  GPIO_SetBits(GPIOA, GPIO_Pin_0);
         vTaskDelay(1);
       //  GPIO_ResetBits(GPIOA, GPIO_Pin_0);
      //   vTaskDelay(250);
       //  xEventGroupWaitBits( xPortTCPOSEventGroup,TCP_START_TASK,pdFALSE,pdTRUE,portMAX_DELAY);
      if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
        {
            printf("can't initialize modbus stack!\r\n" );
        }
        else if( eMBEnable(  ) != MB_ENOERR )
            {
                fprintf( stderr, "%s: can't enable modbus stack!\r\n", PROG );
            }
            else
            {
                printf("stack ok!" );
                do
                {
                    xStatus = eMBPoll(  );
                }
                while( xStatus == MB_ENOERR );
            }
     }

    ( void )eMBDisable(  );
   ( void )eMBClose(  );

}

/* ----------------------- Prototypes ---------------------------------------*/
void            vMBPortEventClose( void );
void            vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule,
                            const CHAR * szFmt, ... );

/* ----------------------- Static variables ---------------------------------*/
//static struct tcp_pcb *pxPCBListen;
//static struct tcp_pcb *pxPCBClient;


/* ----------------------- Static functions ---------------------------------*/
//static err_t    prvxMBTCPPortAccept( void *pvArg, struct tcp_pcb *pxPCB, err_t xErr );
//static err_t    prvxMBTCPPortReceive( void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p,
    //                                  err_t xErr );
//static void     prvvMBTCPPortError( void *pvArg, err_t xErr );

/* ----------------------- Begin implementation -----------------------------*/
BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
   // struct tcp_pcb *pxPCBListenNew, *pxPCBListenOld;
    BOOL            bOkay = FALSE;
    USHORT          usPort;
u8 i;

    if( usTCPPort == 0 )
    {
        usPort = MB_TCP_DEFAULT_PORT;
    }
    else
    {
        usPort = ( USHORT ) usTCPPort;
    }
    //§³§à§Ù§Ñ§Õ§Ñ§ß§Ú§Ö §ã§à§Ü§Ö§ä§Ñ

    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = usPort;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;


    if ( WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf) !=WCHNET_ERR_SUCCESS)
    {
        bOkay = FALSE;
    }

    else
    {
        i = WCHNET_SocketListen(SocketIdForListen);
        mStopIfError(i);
       if ( i!= WCHNET_ERR_SUCCESS)
      {
        WCHNET_SocketClose(SocketIdForListen,TCP_CLOSE_ABANDON);
        bOkay = FALSE;
       }
       else
       {

         printf("ListnerSoceketCreate\n\r");
         printf("socket id: %d\r\n", SocketIdForListen);
        //tcp_accept( pxPCBListenNew, prvxMBTCPPortAccept );
        //SocketIdClient = SocketIdForListen;
        bOkay = TRUE;
      }
    }

    return bOkay;
}

void
prvvMBPortReleaseClient(u8 * SocketID )
{
    if( SocketID != NULL )
    {
        WCHNET_SocketClose(*SocketID,TCP_CLOSE_RST);
        SocketIdClient =0xFF;
    }
}
void
vMBTCPPortClose(  )
{
    /* Shutdown any open client sockets. */
   prvvMBPortReleaseClient( &SocketIdForListen );
    /* Shutdown or listening socket. */
 //   WCHNET_SocketClose(SocketIdForListen,TCP_CLOSE_ABANDON);
//    prvvMBPortReleaseClient( &SocketIdClient );
    /* Release resources for the event queue. */
    vMBPortEventClose( );
}

void
vMBTCPPortDisable( void )
{
    prvvMBPortReleaseClient( &SocketIdForListen);
}



/* Called in case of an unrecoverable error. In any case we drop the client
 * connection. */



BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{

      *ppucMBTCPFrame = &aucTCPBuf[0];
      *usTCPLength = usTCPBufPos;

      /* Reset the buffer. */
      usTCPBufPos= 0;

      return TRUE;

}

BOOL
xMBTCPPortSendResponse( const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
    BOOL            bFrameSent = FALSE;


   // if( SocketIdClient!=0xFF )
    {

        /*Make sure we can send the packet. */
        memcpy( &outTCPBuf[0],pucMBTCPFrame, usTCPLength  );

        Len = usTCPLength ;
        pData = outTCPBuf;
        xEventGroupSetBits(xPortTCPOSEventGroup,DATA_READY);
        xEventGroupWaitBits(xPortTCPOSEventGroup,DATA_SEND,pdTRUE,pdTRUE,portMAX_DELAY);

   /*     J = WCHNET_SocketSend(SocketIdForListen,pData, (uint32_t *)&Len );
       if ( J == WCHNET_ERR_SUCCESS )
       {
*/

            bFrameSent = TRUE;
  /*      }
        else
        {
            mStopIfError(J);

            prvvMBPortReleaseClient( &SocketIdClient );
        }*/
    }

    return bFrameSent;
}



