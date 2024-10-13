/*
 * user_process_service.c
 *
 *  Created on: Oct 9, 2024
 *      Author: i.dymov
 */


#include "user_process_service.h"



/* §¶§å§ß§Ü§è§Ú§Ú §ä§Ñ§Û§Þ§Ö§â§Ñ §à§é§Ú§ã§ä§Ü§Ú*/
static CLEAN_TIMER_t CleanTimer;
 void InitCleanTimer()
{
    CleanTimer.timer_counter = 0;
    CleanTimer.tumer_on = 0;
    CleanTimer.control_state = 0;
    return;
}

 void CleanTimerFuncton(  DISCRET_STATE_t * state)
{
    CleanTimer.control_state =  getReg8( CONTROL_TYPE ) ==  MKV_MB_DIN ? ucDinGet(INPUT_5) : getReg8(LIGTH );
    if (CleanTimer.tumer_on == 0 )
    {
        if ( CleanTimer.control_state == 1)
        {
                CleanTimer.tumer_on  = 1;
        }
     }
     else
     {
         //§¦§ã§Ý§Ú §á§â§Ú§ê§Ö§Ý §ß§à§Ó§í§Û §á§à§Ý§à§Ø§Ú§ä§Ö§Ý§î§ß§í§Û §æ§â§à§ß§ä, §ä§à §á§Ö§â§Ö§Ù§Ñ§á§å§ã§Ü§Ñ§Ö§Þ §ä§Ñ§Û§Þ§Ö§â
        if ((CleanTimer.old_control_state == 0) && (CleanTimer.control_state ==1)) CleanTimer.timer_counter = 0;
        if (++CleanTimer.timer_counter  >= getReg8(CLEAN_TIMER)*600)
        {
            CleanTimer.tumer_on = 0;
            CleanTimer.timer_counter = 0;
            setReg8(LIGTH, 0 );
         }
         else
            *state = 4;
      }
    CleanTimer.old_control_state = CleanTimer.control_state;
    return;
}


static u32 double_channel1_counter = 0;
static u32 double_channel2_counter = 0;
void vCheckDoubleChannelAlarm( u8 *error_state )
{

    if (getReg8(CDV_BP_CH_COUNT) == 2)    //§¦§ã§Ý§Ú §Ó§í§Ò§â§Ñ§ß §à§Õ§Ú§ß §Ü§Ñ§ß§Ñ§Ý, §ä§à §ã§Ò§â§Ñ§ã§í§Ó§Ñ§Ö§Þ §æ§Ý§Ñ§Ô§Ú §à§ê§Ú§Ò§à§Ü, §Ñ§Ý§Ô§à§â§Ú§ä§Þ §ß§Ö §â§Ñ§Ò§à§ä§Ñ§ä§Ö
    {
        (*error_state) &=  ~( FIRST_CHANNEL_ERROR | SECOND_CHANNEL_ERROR );
    }
    else
    {
        if (((*error_state) & (FIRST_CHANNEL_ERROR | SECOND_CHANNEL_ERROR))== 0) //§¦§ã§Ý§Ú §ß§Ö§ä §à§ê§Ú§Ò§Ü§Ú
        {
            if ( getAIN(SENS1) < getReg16(CH1_SETTING)) //§°§ä§ã§Ö§Ø§Ú§Ó§Ñ§Ö§Þ §Õ§Ñ§Ý§Ó§Ö§ß§Ú§Ö §Ó §á§Ö§â§Ó§à§Þ §Ü§Ñ§ß§Ñ§Ý§Ö
                double_channel1_counter++;
            else
                double_channel1_counter = 0;
            if  (getAIN(SENS2) < getReg16(CH2_SETTING)) //§°§ä§ã§Ö§Ø§Ú§Ó§Ñ§Ö§Þ §Õ§Ñ§Ý§Ó§Ö§ß§Ú§Ö §Ó§à §Ó§ä§â§à§à§Þ §Ü§Ñ§ß§Ñ§Ý§Ö
                double_channel2_counter++;
            else
                double_channel2_counter = 0;
            if (double_channel1_counter!=0 && double_channel2_counter!=0) //§¦§ã§Ý§Ú §Õ§Ñ§Ó§Ý§Ö§ß§Ú§Ö §å§á§Ñ§Ý§à §Ó §à§Ò§à§Ú§ç §Ü§Ñ§ß§Ñ§Ý§Ñ §à§Õ§ß§à§Ó§â§Ö§Þ§Ö§ß§ß§à, §ä§à §ã§Ò§â§Ñ§ã§í§Ó§Ö§Þ §ã§é§Ö§ä§é§Ú§Ü§Ú
                                                                          //§à§ê§Ú§Ò§Ü§Ñ §ß§Ö §Ó§à§Ù§ß§Ú§Ü§Ñ§Ö§ä
            {
                double_channel1_counter = 0;
                double_channel2_counter = 0;
            }
            if (double_channel1_counter >= 60000 )  (*error_state)|=FIRST_CHANNEL_ERROR;   //§Ö§ã§Ý§Ú §Õ§Ñ§Ó§Ý§Ö§ß§Ú§Ö §Ü§Ñ§ß§Ñ§Ý 1 §Þ§Ö§ß§î§ß§ê§Ö §Ñ§Ó§Ñ§â. §å§ã§ä§Ó§Ü§Ú §Ò§à§Ý§î§ê§Ö 10 §Þ§Ú§ß.
            if (double_channel2_counter >= 60000 )  (*error_state)|=SECOND_CHANNEL_ERROR;  //§Ö§ã§Ý§Ú §Õ§Ñ§Ó§Ý§Ö§ß§Ú§Ö §Ü§Ñ§ß§Ñ§Ý 2 §Þ§Ö§ß§î§ß§ê§Ö §Ñ§Ó§Ñ§â. §å§ã§ä§Ó§Ü§Ú §Ò§à§Ý§î§ê§Ö 10 §Þ§Ú§ß.

        }
        else
        {
              if ((*error_state) & FIRST_CHANNEL_ERROR)  //§³§Ò§â§Ñ§ã§í§Ó§Ñ§Ö§Þ §à§ê§Ú§Ò§Ü§å §Ö§ã§Ý§Ú §Õ§Ñ§Ó§Ý§Ö§ß§Ú§Ö §Ó§ã§Ñ§ä§Ý§à §Ò§à§Ý§î§ê§Ö §Þ§Ú§ß§Þ§Ñ§Ý§î§ß§à§Û §å§ã§ä§Ñ§Ó§Ü§Ó§Ú.
              {
                  if (getAIN(SENS1) > getReg16(SETTING_MIN)) (*error_state) &= ~FIRST_CHANNEL_ERROR;
              }
              else
              {
                  if (getAIN(SENS2) > getReg16(SETTING_MIN)) (*error_state) &= ~SECOND_CHANNEL_ERROR;
              }
        }
    }
}
