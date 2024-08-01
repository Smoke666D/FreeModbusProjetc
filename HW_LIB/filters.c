/*
 * filters.c
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#include "filters.h"



#define AA 200


 uint16_t vRCFilter( uint16_t input,uint16_t * old_output)
{

    volatile uint32_t new = input;
    volatile uint32_t old = *old_output;
    volatile uint16_t  output =  ( AA * old + (256-AA)*new )>>8;
    *old_output = output;
    return output;
}

void vInitABFilter( ab_filter_data_t * filter, float k)
{
  filter->old_data = 0;
  filter->k= (uint8_t)( 256 * k );

}

#define A 240
ab_data RunABFilter( ab_data data, ab_filter_data_t * filter)
{
    filter->old_data   =  ( filter->k * filter->old_data + (256-filter->k) *data )>>8;
    return (filter->old_data);
}

void vInitRunAverga( aver_filter_data_t * filter,  float k )
{
  filter->old_data = 0;
 // filter->adaprive = adaptive;
  filter->k_up = k;
  //filter->k_down = k1;
}

aver_data RunAvrageFilter( aver_data data, aver_filter_data_t * filter )
{
    float koof =filter->k_up;//filter->k_up;
    /*if ( ( filter->adaprive) && ( data!=0 ) )
    {
        if ( ( (data - filter->old_data) / data ) > 0.3 )
        {
            koof = filter->k_down;
        }
    }*/


    float temp = ((int32_t)data - (int32_t)filter->old_data) * koof;
    filter->old_data = (aver_data)( (float)filter->old_data + temp);

    return (filter->old_data);
}

void vInitMedianFilter(median_filter_data_t * filter)
{
    filter->index = 0;
    for (uint8_t i = 0; i < MEDIAN_COUNT; i++)
    {
        filter->data[i] = 0;
    }
}

median_data MedianFilter( median_data new_val,median_filter_data_t * filter)
{
    filter->data[filter->index] = new_val;
    if (++filter->index >= MEDIAN_COUNT ) filter->index = 0;
    median_data buf[MEDIAN_COUNT ];
    for (uint8_t k= 0; k< MEDIAN_COUNT;k++ )
    {
        buf[k] = filter->data[k];
    }
    for (uint8_t i = 0; i<=((MEDIAN_COUNT/2)+1);i++)
    {
        for (uint8_t j=0; j< MEDIAN_COUNT-i-1; j++ )
        {
            if (buf[j]>buf[j+1])
            {
                uint32_t buff = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = buff;
            }
        }
    }
     median_data res = 0;
    if ((MEDIAN_COUNT & 2) == 0)
    {
        res = buf[MEDIAN_COUNT/2];
    }
    else
    {
        res = ((buf[MEDIAN_COUNT/2] + buf[MEDIAN_COUNT/2 +1]))/2;
    }
    return res;
}

