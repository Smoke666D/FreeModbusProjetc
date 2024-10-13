/*
 * user_process_service.c
 *
 *  Created on: Oct 9, 2024
 *      Author: i.dymov
 */


#include "user_process_service.h"



/* ����ߧܧ�ڧ� ��ѧۧާ֧�� ���ڧ��ܧ�*/
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
         //����ݧ� ���ڧ�֧� �ߧ�ӧ�� ���ݧ�اڧ�֧ݧ�ߧ�� ����ߧ�, ��� ��֧�֧٧ѧ���ܧѧ֧� ��ѧۧާ֧�
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

    if (getReg8(CDV_BP_CH_COUNT) == 2)    //����ݧ� �ӧ�ҧ�ѧ� ��էڧ� �ܧѧߧѧ�, ��� ��ҧ�ѧ��ӧѧ֧� ��ݧѧԧ� ���ڧҧ��, �ѧݧԧ��ڧ�� �ߧ� ��ѧҧ��ѧ��
    {
        (*error_state) &=  ~( FIRST_CHANNEL_ERROR | SECOND_CHANNEL_ERROR );
    }
    else
    {
        if (((*error_state) & (FIRST_CHANNEL_ERROR | SECOND_CHANNEL_ERROR))== 0) //����ݧ� �ߧ֧� ���ڧҧܧ�
        {
            if ( getAIN(SENS1) < getReg16(CH1_SETTING)) //�����֧اڧӧѧ֧� �էѧݧӧ֧ߧڧ� �� ��֧�ӧ�� �ܧѧߧѧݧ�
                double_channel1_counter++;
            else
                double_channel1_counter = 0;
            if  (getAIN(SENS2) < getReg16(CH2_SETTING)) //�����֧اڧӧѧ֧� �էѧݧӧ֧ߧڧ� �ӧ� �ӧ����� �ܧѧߧѧݧ�
                double_channel2_counter++;
            else
                double_channel2_counter = 0;
            if (double_channel1_counter!=0 && double_channel2_counter!=0) //����ݧ� �էѧӧݧ֧ߧڧ� ���ѧݧ� �� ��ҧ�ڧ� �ܧѧߧѧݧ� ��էߧ�ӧ�֧ާ֧ߧߧ�, ��� ��ҧ�ѧ��ӧ֧� ���֧��ڧܧ�
                                                                          //���ڧҧܧ� �ߧ� �ӧ�٧ߧڧܧѧ֧�
            {
                double_channel1_counter = 0;
                double_channel2_counter = 0;
            }
            if (double_channel1_counter >= 60000 )  (*error_state)|=FIRST_CHANNEL_ERROR;   //�֧�ݧ� �էѧӧݧ֧ߧڧ� �ܧѧߧѧ� 1 �ާ֧ߧ�ߧ�� �ѧӧѧ�. ����ӧܧ� �ҧ�ݧ��� 10 �ާڧ�.
            if (double_channel2_counter >= 60000 )  (*error_state)|=SECOND_CHANNEL_ERROR;  //�֧�ݧ� �էѧӧݧ֧ߧڧ� �ܧѧߧѧ� 2 �ާ֧ߧ�ߧ�� �ѧӧѧ�. ����ӧܧ� �ҧ�ݧ��� 10 �ާڧ�.

        }
        else
        {
              if ((*error_state) & FIRST_CHANNEL_ERROR)  //���ҧ�ѧ��ӧѧ֧� ���ڧҧܧ� �֧�ݧ� �էѧӧݧ֧ߧڧ� �ӧ�ѧ�ݧ� �ҧ�ݧ��� �ާڧߧާѧݧ�ߧ�� ����ѧӧܧӧ�.
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
