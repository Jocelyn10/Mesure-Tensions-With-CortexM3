#include "lpc17xx_adc.h"
#include "lpc_types.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_pinsel.h"
#include "adc.h"

unsigned int adcValue;

/*
 * @brief    ADC init routine for CONTINUOUS mode
 * @param    None.
 * @return   None.
 */

void ADC_INIT_CONT(void)
{
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 1;				// Init ADC pin connect
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;				// AD0.2 on P0.25
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	// Configuration for ADC
	ADC_Init(LPC_ADC, 200000);						// ADC conversion rate = 200KHz
	ADC_ChannelCmd(LPC_ADC, 2, ENABLE);				// select: ADC channel 2
	ADC_BurstCmd(LPC_ADC, ENABLE);					// set CONTINUOUS mode
	ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);	// start CONTINUOUS mode
}



