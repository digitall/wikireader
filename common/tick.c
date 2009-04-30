/*
 * tick a 16+12 = 28 bit timer
 * Copyright (c) 2009 Openmoko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "regs.h"
#include "tick.h"

void tick_initialise()
{
	// enable clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK1 |=
		//CPUAHB_HCKE |
		//LCDCAHB_HCKE |
		//GPIONSTP_HCKE |
		//SRAMC_HCKE |
		//EFSIOBR_HCKE |
		//MISC_HCKE |
		//IVRAMARB_CKE |
		//TM5_CKE |
		//TM4_CKE |
		//TM3_CKE |
		//TM2_CKE |
		TM1_CKE |
		TM0_CKE |
		//EGPIO_MISC_CK |
		//I2S_CKE |
		//DCSIO_CKE |
		//WDT_CKE |
		//GPIO_CKE |
		//SRAMSAPB_CKE |
		//SPI_CKE |
		//EFSIOSAPB_CKE |
		//CARD_CKE |
		//ADC_CKE |
		//ITC_CKE |
		//DMA_CKE |
		//RTCSAPB_CKE |
		0;
	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// Advanced Mode
	REG_T16_ADVMODE = T16ADV;

	// 16 bit Timer 0
	// Stop timer
	REG_T16_CTL0 =
		//INITOLx |
		//SELFMx |
		//SELCRBx |
		//OUTINVx |
		//CKSLx |
		//PTMx |
		//PRESETx |
		//PRUNx |
		0;

	// Set divider 48MHz / 1
	REG_T16_CLKCTL_0 =
		P16TONx |
		//P16TSx_MCLK_DIV_4096 |
		//P16TSx_MCLK_DIV_1024 |
		//P16TSx_MCLK_DIV_256 |
		//P16TSx_MCLK_DIV_64 |
		//P16TSx_MCLK_DIV_16 |
		//P16TSx_MCLK_DIV_4 |
		//P16TSx_MCLK_DIV_2 |
		P16TSx_MCLK_DIV_1 |
		0;

	// Set count
	REG_T16_CR0B = 4095;

	// Reset
	REG_T16_CTL0 |= PRESETx;


	// 16 bit Timer 1
	// Stop timer
	REG_T16_CTL1 =
		//INITOLx |
		//SELFMx |
		//SELCRBx |
		//OUTINVx |
		//CKSLx |
		//PTMx |
		//PRESETx |
		//PRUNx |
		0;

	// Set divider 48MHz / 1
	REG_T16_CLKCTL_1 =
		P16TONx |
		P16TSx_MCLK_DIV_4096 |
		//P16TSx_MCLK_DIV_1024 |
		//P16TSx_MCLK_DIV_256 |
		//P16TSx_MCLK_DIV_64 |
		//P16TSx_MCLK_DIV_16 |
		//P16TSx_MCLK_DIV_4 |
		//P16TSx_MCLK_DIV_2 |
		//P16TSx_MCLK_DIV_1 |
		0;

	// Set count
	REG_T16_CR1B = 65535;

	// Reset
	REG_T16_CTL1 |= PRESETx;

	tick_start();
}

void tick_stop()
{
	// Stop Counting
	REG_T16_CTL0 =
		//INITOLx |
		//SELFMx |
		//SELCRBx |
		//OUTINVx |
		//CKSLx |
		//PTMx |
		//PRESETx |
		//PRUNx |
		0;

	REG_T16_CTL1 =
		//INITOLx |
		//SELFMx |
		//SELCRBx |
		//OUTINVx |
		//CKSLx |
		//PTMx |
		//PRESETx |
		//PRUNx |
		0;

	// Reset
	REG_T16_CTL0 |= PRESETx;
	REG_T16_CTL1 |= PRESETx;
}

void tick_start()
{
	// Set PAUSE On
	REG_T16_CNT_PAUSE =
		//PAUSE5 |
		//PAUSE4 |
		//PAUSE3 |
		//PAUSE2 |
		PAUSE1 |
		PAUSE0 |
		0;
	// Run
	REG_T16_CTL0 = PRUNx;
	REG_T16_CTL1 = PRUNx;

	// Set PAUSE Off
	REG_T16_CNT_PAUSE =
		//PAUSE5 |
		//PAUSE4 |
		//PAUSE3 |
		//PAUSE2 |
		//PAUSE1 |
		//PAUSE0 |
		0;
}

unsigned long tick_get(void)
{
	register unsigned long count;
	count = REG_T16_TC1 * 4096 + REG_T16_TC0;
	return count;
}
