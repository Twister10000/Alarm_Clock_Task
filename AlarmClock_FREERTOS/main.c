/*
 * AlarmClock_FREERTOS.c
 *
 * Created: 29.09.2023
 * Author : Twister10000
 */ 

//#include <avr/io.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"
#include "ButtonHandler.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

extern void vApplicationIdleHook( void );
void vButtonTask(void *pvParameters);
void vClockct(void *pvParameters);
void vUserInt(void *pvParameters);

TaskHandle_t UserInt;
TaskHandle_t Clockct;
EventGroupHandle_t xButtonEvent;
EventBits_t eventbitbutton;

char Time[16];	
uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
char A_Time[16];
uint8_t a_seconds = 0;
uint8_t a_minutes = 0;
uint8_t a_hours = 0;

bool s_button1 = false;
bool s_button2 = false;
bool s_button3 = false;
bool s_button4 = false;
bool l_button1 = false;
bool l_button2 = false;
bool l_button3 = false;
bool l_button4 = false;

void vApplicationIdleHook( void )
{	
	
}

int main(void)
{

    resetReason_t reason = getResetReason();

	vInitClock();
	vInitDisplay();
	
	xTaskCreate(vButtonTask, (const char *) "btTask", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(vClockct, (const char *) "Clockct", configMINIMAL_STACK_SIZE, NULL, 2, Clockct);
	xTaskCreate(vUserInt, (const char *) "UserInt", configMINIMAL_STACK_SIZE, NULL, 2, UserInt);
	xButtonEvent = xEventGroupCreate();

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"FreeRTOS 10.0.1");
	vDisplayWriteStringAtPos(1,0,"EDUBoard 1.0");
	vDisplayWriteStringAtPos(2,0,"Template");
	vDisplayWriteStringAtPos(3,0,"ResetReason: %d", reason);
	vTaskStartScheduler();
	return 0;
}
void vUserInt(void *pvParamters){
	
	(void) pvParamters;
	uint8_t UIMODE = 0;
	int8_t Alarm = 1;
	
	for (;;)
	{		
			
		switch(eventbitbutton){
				
			case 1:
				
				s_button1 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,1);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 2:

				s_button2 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,2);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 3:
				s_button3 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,3);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 4:
				s_button4 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,4);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 11:
				eventbitbutton = xEventGroupClearBits(xButtonEvent,11);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 44:
				UIMODE = 8;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,44);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			}
		if (UIMODE == 8){
			
			vDisplayClear();
			vDisplayWriteStringAtPos(0,0,"Time Settings");
			vDisplayWriteStringAtPos(1,1,"Time: %s", Time);
			vDisplayWriteStringAtPos(3,0,"S1:B S2:h S3:m S4:s");
			//vTaskSuspend(Clockct);
			if (s_button1 == true)
			{
				UIMODE == 0;
			}
			if (s_button2 == true)
			{
				hours++;	
			}
			if (s_button3 == true)
			{
				minutes++;
			}
			if (s_button4 == true)
			{
				seconds++;
			}
			//vTaskResume(Clockct);
		}
		if(UIMODE == 0){
			if (s_button2 == true)
			{
				Alarm = Alarm*-1;
			}
			switch(Alarm){
				case 1:
					vDisplayClear();
					vDisplayWriteStringAtPos(0,0,"Alarm-Clock 1.0");
					vDisplayWriteStringAtPos(1,2,"Time: %s",Time);
					vDisplayWriteStringAtPos(2,1,"Alarm: %s", A_Time);
					vDisplayWriteStringAtPos(3,0,"Alarm: Aktiv");
					break;
				case -1:					
					vDisplayClear();
					vDisplayWriteStringAtPos(0,0,"Alarm-Clock 1.0");
					vDisplayWriteStringAtPos(1,2,"Time: %s",Time);
					vDisplayWriteStringAtPos(2,1,"Alarm: %s", A_Time);
					vDisplayWriteStringAtPos(3,0,"Alarm: Deaktiviert");
					break;
			}
		}
		vTaskDelay(200/portTICK_RATE_MS);
		s_button1 = false;
		s_button2 = false;
		s_button3 = false;
		s_button4 = false;
	}
}

void vClockct(void *pvParameters){
	
	(void) pvParameters;
	
	for (;;)
	{
		seconds++;
		
		if (seconds == 60)
		{
			minutes++;
			seconds = 0;
		}
		if (minutes == 60){
			hours++;
			minutes = 0;
		}
		if (hours == 24)
		{
			hours = 0;
		}
		
		sprintf(&Time[0], "%.2i:%.2i:%.2i", hours, minutes, seconds);
		sprintf(&A_Time[0], "%.2i:%.2i:%.2i", a_hours, a_minutes, a_seconds);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void vButtonTask(void *pvParameters){
	
	(void) pvParameters;
	initButtons();
	
	for (;;)
	{
		updateButtons();
			
		if(getButtonPress(BUTTON1) == SHORT_PRESSED){
			
			eventbitbutton = xEventGroupSetBits(xButtonEvent, 1);
		}
		if (getButtonPress(BUTTON1) == LONG_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,11);
		}
		if (getButtonPress(BUTTON2) == SHORT_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,2);
		}
		if (getButtonPress(BUTTON3) == SHORT_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,3);
		}
		if (getButtonPress(BUTTON3) == LONG_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,33);
		}
		if (getButtonPress(BUTTON4) == SHORT_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,4);
		}
		if (getButtonPress(BUTTON4) == LONG_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,44);	
		}
		vTaskDelay((1000/BUTTON_UPDATE_FREQUENCY_HZ)/portTICK_RATE_MS);
	}
}
