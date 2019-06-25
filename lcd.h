/************************************************************************/
/*    Title:    header for I2C LCD-modul				     			*/
/*    Author:   ACMC sw application team                                */
/*    Date:     03/12                                                   */
/*    Software: GNUARM                   								*/
/*    Hardware: LPC17xx                                 				*/
/*    Note:     required  lpc17xx_i2c.h/lpc17xx_i2c.c			      	*/
/*                        lpc17xx_pinsel.h/lpc17xx_pinsel.c             */
/*              functions:                      	                  	*/
/*              LCD_INIT       	->  initialize I2C-LCD    			    */
/*              LCD_gotoXY     	->  set LCD cursor  line and character  */
/*              LCD_SENDSTRING 	->  sends a string to LCD          		*/
/*				LCD_SENDCHAR	->	sends a character to LCD			*/
/*				LcdSendInt		->	sends a integer to LCD				*/
/************************************************************************/
#ifndef LCD_H_
#define LCD_H_

extern const char adr_lcd; // i2c address LCD

/********************************************************************//**
 * @brief		init sequence for lcd
 * @param[in]	none
 * @return 		none
 *********************************************************************/
void LCD_INIT(void);

/********************************************************************//**
 * @brief		set LCD cursor to column and character
 * @param[in]	line, character
 * @return 		none
 *********************************************************************/
void LCD_gotoXY(char line, char character);

/********************************************************************//**
 * @brief		send a string to i2c lcd
 * @param[in]	pointer to char
 * @return 		none
 *********************************************************************/
void LCD_SENDSTRING(char *ptr_char);

/********************************************************************//**
 * @brief		send a character to i2c lcd
 * @param[in]	character
 * @return 		none
 *********************************************************************/
void LCD_SENDCHAR(char character);

/********************************************************************//**
 * @brief		convert and send a integer to i2c lcd
 * @param[in]	iBcd
 * @return 		none
 *********************************************************************/
void LcdSendInt(unsigned int iBcd);

#endif
