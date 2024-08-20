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
 * File: $Id: portevent.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- lwIP ---------------------------------------------*/
//#include "lwip/api.h"
//#include "lwip/sys.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "system_init.h"
#include "main.h"

/* ----------------------- Variables ----------------------------------------*/

EventGroupHandle_t xSlaveOsEventGroupHandle;
/* ----------------------- Start implementation -----------------------------*/


EventGroupHandle_t  * xGetOSEvent()
{
    return (&xSlaveOsEventGroupHandle);
}

BOOL
xMBPortEventInit( void )
{
  //  xEventInQueue = FALSE;
   // xSlaveOsEventGroupHandle = xGetOSEvent();
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    BaseType_t xHigherPriorityTaskWoken, xResult;
          switch (eEvent)
            {
            case EV_READY:
            case EV_FRAME_RECEIVED:

                    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
                    xHigherPriorityTaskWoken = pdFALSE;

                     /* Set bit 0 and bit 4 in xEventGroup. */
                     xResult = xEventGroupSetBitsFromISR(
                                                     xSlaveOsEventGroupHandle,   /* The event group being updated. */
                                                     eEvent , /* The bits being set. */
                                                  &xHigherPriorityTaskWoken );

                      /* Was the message posted successfully? */
                      if( xResult != pdFAIL )
                      {
                          /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
                          switch should be requested.  The macro used is port specific and will
                          be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
                          the documentation page for the port being used. */
                          portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                      }

                break;
            case EV_EXECUTE:

            case EV_FRAME_SENT:
                xHigherPriorityTaskWoken = pdFALSE;
                xResult =xEventGroupSetBitsFromISR(
                         xSlaveOsEventGroupHandle,   /* The event group being updated. */
                         eEvent , /* The bits being set. */
                     &xHigherPriorityTaskWoken );
                 if( xResult != pdFAIL )
                                  {
                                      /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
                                      switch should be requested.  The macro used is port specific and will
                                      be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
                                      the documentation page for the port being used. */
                                      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                                  }
                    //  xEventGroupSetBits(xSlaveOsEventGroupHandle,eEvent);
                break;
            }

    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    /* waiting forever OS event */
        EventBits_t recvedEvent;
        recvedEvent = xEventGroupWaitBits(xSlaveOsEventGroupHandle,    EV_READY | EV_FRAME_RECEIVED | EV_EXECUTE | EV_FRAME_SENT,   pdTRUE, pdFALSE, portMAX_DELAY );
      // rt_event_recv(&xSlaveOsEvent,
      //         EV_READY | EV_FRAME_RECEIVED | EV_EXECUTE | EV_FRAME_SENT,
      //         RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER,
      //         &recvedEvent);
       switch (recvedEvent)
       {
       case EV_READY:
           *eEvent = EV_READY;
           break;
       case EV_FRAME_RECEIVED:
           *eEvent = EV_FRAME_RECEIVED;
           break;
       case EV_EXECUTE:
           *eEvent = EV_EXECUTE;
           break;
       case EV_FRAME_SENT:
           *eEvent = EV_FRAME_SENT;
           break;
       }
       return TRUE;
   }

