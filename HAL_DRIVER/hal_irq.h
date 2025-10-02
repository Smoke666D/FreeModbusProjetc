/*
 * hal_irq.h
 *
 *  Created on: Jul 6, 2024
 *      Author: i.dymov
 */

#ifndef HAL_HAL_IRQ_H_
#define HAL_HAL_IRQ_H_


#include "main.h"

#if CORE == WCH32V2 || CORE == WCH32V3
	void HAL_InitAPB1(uint32_t apbmask);
	void HAL_InitAPB2(uint32_t apbmask);
	#if CORE == WCH32V2
		#include "ch32v20x.h"
		void PFIC_IRQ_ENABLE_PG1(IRQn_Type irg, u8 prior, u8 subprior);
	#else
		#include "ch32v30x.h"
		#include "ch32v30x_misc.h"
		void PFIC_IRQ_ENABLE_PG2(IRQn_Type irg, u8 prior, u8 subprior);
	#endif
#endif

#endif /* HAL_HAL_IRQ_H_ */
