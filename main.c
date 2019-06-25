#include "delay.h"
#include "lcd.h"
#include "coocox.h"
#include "CoOS.h"
#include "lpc17xx_adc.h"

#include "adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "delay.h"

unsigned int valeurLue[7];
int Vr, DV=0, u, V;
unsigned char cBcd1, cBcd10, cBcd100, cBcd1000;
unsigned char cUcd1, cUcd10, cUcd100, cUcd1000;

unsigned int task = 0;
unsigned long btn = 1 << 10;

OS_STK task1_stk[128];
OS_STK task2_stk[128];
OS_STK task_btn[128];
OS_STK sync_stk[128];

OS_FlagID flag;
OS_FlagID flagBtn;

void task1(void* pdata);
void task2(void* pdata);

void taskBouton(void* pdata);
void synchroTask(void* pdata);

void createFlagAndTask();
void sendToLcd(int lcdLine, char message[]);
void eraseLCDLine(int lcdLine);

/* Leds */
uint32_t LED1 = (1<<1);
uint32_t LED2 = (1<<2);

int main(void) {

	GPIO_SetDir(0, LED1, 1);
	GPIO_SetDir(0, LED2, 1);

	GPIO_SetDir(0, btn, 0);
	LCD_INIT();
	ADC_INIT_CONT();

	CoInitOS();

	createFlagAndTask();

	CoStartOS();
	while (1);
}

void createFlagAndTask() {
	flag = CoCreateFlag(TRUE, 0);
	flagBtn = CoCreateFlag(TRUE, 0);
	CoCreateTask(synchroTask,0,3,&sync_stk[128-1],128);
	CoCreateTask(task1,0,2,&task1_stk[128-1],128);
	CoCreateTask(taskBouton,0,1,&task_btn[128-1],128);
}

void synchroTask(void* pdata) {

	for (;;) {

		int cnt = 0;

		for (;;) {

			if ((cnt % 2) == 0) {
				CoSetFlag(flag);
			} else if ((cnt % 2) == 1) {
				CoSetFlag(flagBtn);
			}
			cnt++;
		}
	}
}

void taskBouton(void* pdata) {

	unsigned long initValue;

	for (;;) {

		CoWaitForSingleFlag(flagBtn, 0);
		initValue = GPIO_ReadValue(0);

		//Pour lancer ou arrêter le programme
		if (!(initValue & btn)) {

			if (task == 0) {
				task = 1;


					if (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2,
							ADC_DATA_DONE)) {
						adcValue = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
						Vr = adcValue;

						GPIO_ClearValue(0,LED1);
						GPIO_ClearValue(0,LED2);
					}
				delay_ms(500);

				sendToLcd(1, "Program");
				eraseLCDLine(2);
				sendToLcd(3, "\"Demarrage\"");

				delay_ms(1000);

			}

			else if(task != 0){

				sendToLcd(1, "Program");
				eraseLCDLine(2);
				sendToLcd(3, "\"Arret\"");
				task = 0;

				GPIO_ClearValue(0,LED1);
				GPIO_ClearValue(0,LED2);

				CoCreateTask(task1,0,1,&task1_stk[128-1],128);
				delay_ms(500);
			}
		}
	}
}

void task1(void* pdata) {

	for (;;) {

		CoWaitForSingleFlag(flag, 0);

		if (task == 1) {
			sendToLcd(1, "Tache 0");
			sendToLcd(2, "Vr lue");
			sendToLcd(3, "avec succes");
			delay_ms(2000);

			eraseLCDLine(1);
			sendToLcd(2, "Vr=");
			LcdSendInt(Vr);
			eraseLCDLine(3);
			delay_ms(2000);

			sendToLcd(1, "Debut");
			sendToLcd(2, "Tache 1");
			eraseLCDLine(3);
			delay_ms(2000);

			eraseLCDLine(1);
			eraseLCDLine(2);
			eraseLCDLine(3);

				if (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, ADC_DATA_DONE)) {

				adcValue = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
				V = adcValue; // V = adcValue
				DV = V - Vr;

				if (DV >= 0) {

					LCD_INIT();

					LCD_gotoXY(1, 0);
					LCD_SENDSTRING("V =");
					LCD_gotoXY(2, 0);
					LCD_SENDSTRING("Vr=");
					LCD_gotoXY(3, 0);
					LCD_SENDSTRING("DV=");

					LCD_gotoXY(1, 4);
					LcdSendInt(V);
					LCD_gotoXY(2, 4);
					LcdSendInt(Vr);
					LCD_gotoXY(3, 4);
					LcdSendInt(DV);

					GPIO_SetValue(0, LED1);
					GPIO_ClearValue(0, LED2);

					delay_ms(4000);
				}

				else {

					LCD_INIT();

					DV = (-1)*DV;

					LCD_gotoXY(1, 0);
					LCD_SENDSTRING("V =");
					LCD_gotoXY(2, 0);
					LCD_SENDSTRING("Vr=");
					LCD_gotoXY(3, 0);
					LCD_SENDSTRING("DV=-");

					cBcd1 = DV % 10 + '0';
					cBcd10 = (DV / 10) % 10 + '0';
					cBcd100 = (DV / 100) % 10 + '0';
					cBcd1000 = (DV / 1000) % 10 + '0';

					LCD_gotoXY(1, 4);
					LcdSendInt(V);
					LCD_gotoXY(2, 4);
					LcdSendInt(Vr);

					LCD_gotoXY(3, 5);
					LCD_SENDCHAR(cBcd1000);
					LCD_gotoXY(3, 6);
					LCD_SENDCHAR(cBcd100);
					LCD_gotoXY(3, 7);
					LCD_SENDCHAR(cBcd10);
					LCD_gotoXY(3, 8);
					LCD_SENDCHAR(cBcd1);

					GPIO_SetValue(0, LED1);
					GPIO_ClearValue(0, LED2);

					delay_ms(4000);
				}
			}

				sendToLcd(1, "Fin");
				sendToLcd(2, "Tache 1");
				eraseLCDLine(3);
				delay_ms(2000);

				task = 2;
				CoCreateTask(task2,0,1,&task2_stk[128-1],128);
				CoExitTask();

		}
	}
}

void task2(void* pdata) {

	for (;;) {

		CoWaitForSingleFlag(flag, 0);

		if (task == 2) {
			sendToLcd(1, "Fin");
			sendToLcd(2, "Tache 1");
			eraseLCDLine(3);
			delay_ms(2000);

			sendToLcd(1, "Debut");
			sendToLcd(2, "Tache 2");
			eraseLCDLine(3);
			delay_ms(2000);

			eraseLCDLine(1);
			eraseLCDLine(2);

			int k = 6; // Pour NSA, k=6
			u = -k*DV;

				if (u >= 0) {

					if (DV >= 0) {

						LCD_INIT();

						LCD_gotoXY(1, 0);
						LCD_SENDSTRING("V =");
						LCD_gotoXY(2, 0);
						LCD_SENDSTRING("DV=");
						LCD_gotoXY(3, 0);
						LCD_SENDSTRING("u =");

						LCD_gotoXY(1, 4);
						LcdSendInt(V);
						LCD_gotoXY(2, 4);
						LcdSendInt(DV);
						LCD_gotoXY(3, 4);
						LcdSendInt(u);

						GPIO_SetValue(0, LED1);
						GPIO_ClearValue(0, LED2);

						delay_ms(4000);

					} else {

						LCD_INIT();

						LCD_gotoXY(1, 0);
						LCD_SENDSTRING("V =");
						LCD_gotoXY(2, 0);
						LCD_SENDSTRING("DV=");
						LCD_gotoXY(3, 0);
						LCD_SENDSTRING("u =");

						LCD_gotoXY(1, 4);
						LcdSendInt(V);

						LCD_gotoXY(2, 4);
						LCD_SENDCHAR(cBcd1000);
						LCD_gotoXY(2, 5);
						LCD_SENDCHAR(cBcd100);
						LCD_gotoXY(2, 6);
						LCD_SENDCHAR(cBcd10);
						LCD_gotoXY(2, 7);
						LCD_SENDCHAR(cBcd1);


						LCD_gotoXY(3, 4);
						LcdSendInt(u);

						GPIO_SetValue(0, LED1);
						GPIO_ClearValue(0, LED2);

						delay_ms(4000);
					}
				}

				else {

					u = (-1) * u;

					cUcd1 = u % 10 + '0';
					cUcd10 = (u / 10) % 10 + '0';
					cUcd100 = (u / 100) % 10 + '0';
					cUcd1000 = (u / 1000) % 10 + '0';

					if (DV >= 0) {

						LCD_INIT();

						LCD_gotoXY(1, 0);
						LCD_SENDSTRING("V =");
						LCD_gotoXY(2, 0);
						LCD_SENDSTRING("DV=");
						LCD_gotoXY(3, 0);
						LCD_SENDSTRING("u=-");

						LCD_gotoXY(1, 4);
						LcdSendInt(V);
						LCD_gotoXY(2, 4);
						LcdSendInt(DV);

						LCD_gotoXY(3, 4);
						LCD_SENDCHAR(cUcd1000);
						LCD_gotoXY(3, 5);
						LCD_SENDCHAR(cUcd100);
						LCD_gotoXY(3, 6);
						LCD_SENDCHAR(cUcd10);
						LCD_gotoXY(3, 7);
						LCD_SENDCHAR(cBcd1);

						GPIO_SetValue(0, LED1);
						GPIO_ClearValue(0, LED2);

						delay_ms(4000);

					} else {

						LCD_INIT();

						LCD_gotoXY(1, 0);
						LCD_SENDSTRING("V =");
						LCD_gotoXY(2, 0);
						LCD_SENDSTRING("DV=-");
						LCD_gotoXY(3, 0);
						LCD_SENDSTRING("u=-");

						LCD_gotoXY(1, 4);
						LcdSendInt(V);

						LCD_gotoXY(2, 5);
						LCD_SENDCHAR(cBcd1000);
						LCD_gotoXY(2, 6);
						LCD_SENDCHAR(cBcd100);
						LCD_gotoXY(2, 7);
						LCD_SENDCHAR(cBcd10);
						LCD_gotoXY(2, 8);
						LCD_SENDCHAR(cBcd1);

						LCD_gotoXY(3, 4);
						LCD_SENDCHAR(cUcd1000);
						LCD_gotoXY(3, 5);
						LCD_SENDCHAR(cUcd100);
						LCD_gotoXY(3, 6);
						LCD_SENDCHAR(cUcd10);
						LCD_gotoXY(3, 7);
						LCD_SENDCHAR(cBcd1);

						GPIO_SetValue(0, LED1);
						GPIO_ClearValue(0, LED2);

						delay_ms(4000);
					}
				}

			}

			sendToLcd(1, "   Fin   ");
			sendToLcd(2, " Tache 2 ");
			eraseLCDLine(3);
			delay_ms(1000);

			GPIO_ClearValue(0, LED1);
			GPIO_ClearValue(0, LED2);

			task = 1;
			CoCreateTask(task1,0,1,&task1_stk[128-1],128);
			CoExitTask();
		}

}

void sendToLcd(int lcdLine, char message[]) {

	eraseLCDLine(lcdLine);
	LCD_gotoXY(lcdLine, 0);
	LCD_SENDSTRING(message);
}

void eraseLCDLine(int lcdLine) {

	LCD_gotoXY(lcdLine, 0);
	LCD_SENDSTRING("            ");
}
