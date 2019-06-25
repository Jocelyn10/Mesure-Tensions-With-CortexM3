/************************************************************************/
/*    Title:    code for I2C LCD-modul					     			*/
/*    Author:   ACMC sw application team                                */
/*    Date:     03/12                                                   */
/*    Software: GNUARM                   								*/
/*    Hardware: LPC17xx                                 				*/
/*    Note:     recommend lpc17xx_i2c.h/lpc17xx_i2c.c			      	*/
/*                        lpc17xx_pinsel.h/lpc17xx_pinsel.c             */
/*              functions:                      	                  	*/
/*              LCD_INIT       	->  initialize I2C-LCD    			    */
/*              LCD_gotoXY     	->  set LCD cursor  line and character  */
/*              LCD_SENDSTRING 	->  sends a string to LCD          		*/
/*				LCD_SENDCHAR	->	sends a character to LCD			*/
/*				LcdSendInt		->	sends a integer to LCD				*/
/************************************************************************/


#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"
#include "lcd.h"
#include "delay.h"

const char adr_lcd = 0x74 >> 1; // Adr. LCD

/********************************************************************//**
 * @brief		init sequence for lcd
 * @param[in]	none
 * @return 		none
 *********************************************************************/
void LCD_INIT(void) {
	I2C_M_SETUP_Type data;

	uint8_t init1[] = {
			0x00, 	// NOP
			0x2e, 	// function set (4-Lines, ...)
			0x0c, 	// display on and cursor off
			0x06, 	// entry mode
			0x01 	// clear display
			};
	uint8_t init2[] = {
			0x00, 	// set command mode!!! RS=0
			0x80, 	// co=1, rs=0, r/w =0, 0,0,0,0,0
			0x40 	// CG RAM address 0100 0000
			};

	// ° character
	uint8_t init3[] = { 0x40, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00 };

	/**/
	// configure pins
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;

	// P0.0, P0.1 as GPIO
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	// P0.19, P0.20 as I2C1
	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 19;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 20;
	PINSEL_ConfigPin(&PinCfg);

	// I2C init , I2C on
	I2C_Init(LPC_I2C1, 100000);
	I2C_Cmd(LPC_I2C1, ENABLE);


	// LCD init
	data.sl_addr7bit = adr_lcd;
	data.tx_data = init1;
	data.tx_length = sizeof(init1);
	data.rx_data = NULL;
	data.rx_length = 0;
	data.retransmissions_max = 1;
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);

	data.tx_data = init2;
	data.tx_length = sizeof(init2);
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);

	data.tx_data = init3;
	data.tx_length = sizeof(init3);
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);
}

/********************************************************************//**
 * @brief		set LCD cursor to column and character
 * @param[in]	line, character
 * @return 		none
 *********************************************************************/
void LCD_gotoXY(char line, char character) {
	char lcd_line = 0x00, sum = 0x00;
	I2C_M_SETUP_Type data;

	switch (line) 			// switch line
	{
	case 1:
		lcd_line = 0x00; 	// start adress line 1=0x00
		break;

	case 2:
		lcd_line = 0x20; 	// start adress line 2=0x20
		break;

	case 3:
		lcd_line = 0x40; 	// start adress line 3=0x40
		break;

	default:
		lcd_line = 0x00; 	// for all other line 1
		break;
	}

	sum = (lcd_line + character - 1); // calculate cursor address

	uint8_t gotoxy[] = {
			0x02, 			// Cursor home
			(sum | 0x80) };	// send cursor address

	data.sl_addr7bit = adr_lcd;
	data.tx_data = gotoxy;
	data.tx_length = sizeof(gotoxy);
	data.rx_data = NULL;
	data.rx_length = 0;
	data.retransmissions_max = 1;
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);
}

/********************************************************************//**
 * @brief		send a string to i2c lcd
 * @param[in]	pointer to char
 * @return 		none
 *********************************************************************/
void LCD_SENDSTRING(char *ptr_char) {
	I2C_M_SETUP_Type data;
	uint8_t i = 1;
	uint8_t buffer[16];
	buffer[0] = 0x40; // operating sequence for LCD

	// copy characters to send buffer
	while (*ptr_char != 0x00 && i < sizeof(buffer)) {
		buffer[i] = 0x80 | *ptr_char++;
		i++;
	}

	data.sl_addr7bit = adr_lcd;
	data.tx_data = buffer;
	data.tx_length = i;
	data.rx_data = NULL;
	data.rx_length = 0;
	data.retransmissions_max = 1;
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);
}

/********************************************************************//**
 * @brief		send a character to i2c lcd
 * @param[in]	character
 * @return 		none
 *********************************************************************/
void LCD_SENDCHAR(char character) {
	I2C_M_SETUP_Type data;
	uint8_t buffer[2];
	buffer[0] = 0x40; // operating sequence for LCD
	buffer[1] = 0x80 | character;

	data.sl_addr7bit = adr_lcd;
	data.tx_data = buffer;
	data.tx_length = 2;
	data.rx_data = NULL;
	data.rx_length = 0;
	data.retransmissions_max = 1;
	I2C_MasterTransferData(LPC_I2C1, &data, I2C_TRANSFER_POLLING);
}

/********************************************************************//**
 * @brief		convert and send a integer to i2c lcd
 * @param[in]	iBcd
 * @return 		none
 *********************************************************************/
void LcdSendInt(unsigned int iBcd)
{
	char cBcd[6];						// string field

	cBcd[5]=0x00;						// zero terminate
	cBcd[4]=iBcd%10 + '0';				// convert one's digit
	cBcd[3]=(iBcd/10)%10 + '0';			// convert ten's digit
	cBcd[2]=(iBcd/100)%10 + '0';		// convert hundred's digit
	cBcd[1]=(iBcd/1000)%10 + '0';		// convert thousand's digit
	cBcd[0]=(iBcd/10000)%10 + '0';		// convert ten thousand's digit

	LCD_SENDSTRING(cBcd);				// send string
}
