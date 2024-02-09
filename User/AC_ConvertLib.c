/*
 * AC_ConvertLib.c
 *
 *  Created on: Feb 8, 2024
 *      Author: i.dymov
 */


#include "AC_ConvertLib.h"


 uint16_t vRCFilter( uint16_t input,uint16_t * old_output)

{

    volatile uint32_t new = input;
    volatile uint32_t old = *old_output;


    volatile uint16_t  output =  ( A * old + (256-A)*new )>>8;
    //*old_input = input;
    *old_output = output;
    return output;
}



 void  vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd)
 {
   switch (AMP/100)
          {
            case 0:
              *del = 0;
              *fd =  70;
              break;
            case 1:
              *del = 2;
              *fd =  40;
              break;
            case 2:
              *del = 4;
              *fd =  25;
              break;
            case 3:
              *del = 5;
              *fd =  20;
              break;
            case 4:
              *del = 7;
              *fd =  15;
              break;
            case 5:
              *del = 8;
              *fd =  10;
              break;
            case 6:
              *del = 11;
              *fd =  8;
              break;
            case 7:
              *del = 12;
              *fd =  7;
              break;
            case 8:
              *del = 14;
              *fd =  6;
              break;
            case 9:
              *del = 15;
              *fd =  5;
              break;
            case 10:
              *del = 16;
              *fd =  4;
              break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            default:
              *del = 20;
              *fd =  4;
              break;
          }
 }


 void vADCFindFreq(int16_t * data, uint16_t * count, uint8_t off, int16_t AMP)
 {
   uint8_t AMP_DELTA = 15,
                  FD = 15,
                  F1 = 0,
                  F2 = 0,
                  CNT = 0,
                  index = 0;

   uint16_t tt=0;
   uint16_t PER[MAX_ZERO_POINT];

   vADCSetFreqDetectParam(AMP,&AMP_DELTA,&FD);

   for (uint16_t i=FD<<1;i<(AC_CONVERION_NUMBER-FD)<<1;i=i+2)
   {
     //§¦§ã§Ý§Ú §Ù§ß§Ñ§é§Ö§ß§Ú§Ö §á§à§á§Ñ§Õ§Ö§ä §Ó §Ü§à§â§â§Ú§Õ§à§â §à§Ü§Ý§à §ß§å§Ý§ñ
     if ((data[i] > -AMP_DELTA) && (data[i] < AMP_DELTA))
     {
         //§ä§à §á§â§Ó§Ö§â§ñ§Ö§Þ §ä§Ö§Ü§å§ë§å§ð §æ§Ñ§Ù§å
         if ((data[i] > data[i-FD*4]) || (data[i] < data[i+FD*4]))
             F2  = 1U;
         else
             F2 = 0U;
         if (CNT > 0)
         {
           if (((F2==1) && (F1 == 1)) || ((F2==0) &&(F1==0)))
           {
             index = 5;
             break;
           }
         }
         F1 = F2;
         PER[index++] =i/2;
         i = i+ FD*4;
         CNT++;
         if (index> MAX_ZERO_POINT) break;
     }
   }
   tt =0;
   if (index>1)
   {
     for (uint8_t i =1;i<index;i++)
     {
       tt =tt+ + PER[i]-PER[i-1];
     }
     tt = (tt/(index-1))*2;
   }
   *count = tt;

   return;
 }


float  xADCRMS(int16_t * source, uint16_t size, uint8_t cc )
 {
   uint64_t sum =0;
   for (uint16_t i=0;i<size*cc;i=i+cc)
   {
     sum =sum+ source[i]*source[i];
   }
   sum =sum/size;
   return  sqrt(sum);

 }


//§¶§å§ß§Ö§è§Ú§ñ §á§à §Ó§í§é§Ú§ä§Ñ§ß§Ú§ð §ß§Ö§Û§ä§â§Ñ§Ý§Ú §Ú§Ù §æ§Ñ§Ù§í
 void vDecNetural(int16_t * data)
{
 for (int16_t i = 0;i<AC_CONVERION_NUMBER*ADC_CHANNEL;i=i+ADC_CHANNEL)
 {
   data[i]   = data[i]   - data[i+1];
 }
  return;
}


/*
 * §³§Ö§â§Ó§Ú§ã§ß§Ñ§ñ §æ§å§ß§Ü§è§Ú§ñ §Õ§Ý§ñ §Ó§í§é§Ú§ã§Ý§Ö§ß§Ú§ñ §Þ§Ñ§Ü§ã§Ú§Þ§Ñ§Ý§î§ß§à§Ô§à §Ù§ß§Ñ§é§Ö§ß§Ú§ñ
 *  source  -§Ñ§Õ§â§Ö§ã §Ò§å§æ§Ö§â§Ñ DMA
 *  off     -§ß§à§Þ§Ö§â §Ü§Ñ§ß§Ñ§Ý§Ñ, §ã§Ü§à§ä§à§â§í§Þ §â§Ñ§Ò§à§ä§Ñ§Ö§ä §æ§å§ß§Ü§è§Ú§ñ
 *  size    -§â§Ñ§Ù§Þ§Ö§â §Ò§å§æ§Ö§â§Ñ §Õ§Ý§ñ §à§Ò§â§Ñ§Ò§à§ä§Ü§Ú
 */
 int16_t  xADCMax( int16_t * source,  uint16_t * delay )
{

  int16_t max =0;
  uint16_t del =0,i;

  for (i=0;i<AC_CONVERION_NUMBER*ADC_CHANNEL;i=i+ADC_CHANNEL)
  {
      if ( source[i ] >  max)
      {
        max = source[i];
        del = i;
      }
  }
  *delay = (del/ADC_CHANNEL);
  return ( max );

}
