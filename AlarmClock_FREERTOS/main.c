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

extern void vApplicationIdleHook( void );
void vButtonTask(void *pvParameters);
void vClockct(void *pvParameters);
void vUserInt(void *pvParameters);
void vAlarm(void *pvpParameters);
void vLED(void *pvParameters);

TaskHandle_t UserInt;
TaskHandle_t Clockct;
TaskHandle_t Alarmct;
TaskHandle_t LEDct;
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
int8_t Alarm = 1;
	
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
	xTaskCreate(vClockct, (const char *) "Clockct", configMINIMAL_STACK_SIZE, NULL, 2, &Clockct);
	xTaskCreate(vUserInt, (const char *) "UserInt", configMINIMAL_STACK_SIZE, NULL, 2, &UserInt);
	xTaskCreate(vAlarm, (const char *) "Alarmtsk", configMINIMAL_STACK_SIZE, NULL, 2, &Alarmct);
	xTaskCreate(vLED, (const char *) "LEDtsk", configMINIMAL_STACK_SIZE, NULL, 2, &LEDct);
	vTaskSuspend(Alarmct);
	vTaskSuspend(LEDct);
	xButtonEvent = xEventGroupCreate();

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"FreeRTOS 10.0.1");
	vDisplayWriteStringAtPos(1,0,"EDUBoard 1.0");
	vDisplayWriteStringAtPos(2,0,"Template");
	vDisplayWriteStringAtPos(3,0,"ResetReason: %d", reason);
	vTaskStartScheduler();
	return 0;
}
void vUserInt(void *pvParamters)
{
	
	(void) pvParamters;
	uint8_t UIMODE = 0;
	bool hour = false;
	bool minute = false;
	bool second = false;
	
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
				l_button1 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,11);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 22:
				l_button2 = true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,22);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 33:
				l_button3 = true;				
				eventbitbutton = xEventGroupClearBits(xButtonEvent,33);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			case 44:
				UIMODE = 8;
				l_button4 =	true;
				eventbitbutton = xEventGroupClearBits(xButtonEvent,44);
				eventbitbutton = xEventGroupGetBits(xButtonEvent);
				break;
			}
		if (UIMODE == 7)
		{

			vDisplayClear();
			vDisplayWriteStringAtPos(0,0,"Alarm Settings");
			vDisplayWriteStringAtPos(1,1,"Alarm: %s", A_Time);
			vDisplayWriteStringAtPos(3,0,"S1:B S2:h S3:m S4:s");
			if (hour == false && minute == false && second == false)
			{
				if (s_button1 == true)
				{
					UIMODE = 0;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					hour = true;
					s_button2 = false;
				}
				if (s_button3 == true)
				{
					minute = true;
					s_button3 = false;
				}
				if (s_button4 == true)
				{
					second = true;
					s_button4 = false;
				}
			}
			if (hour == true)
			{				
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");
				
				if (s_button1 == true)
				{
					hour = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					a_hours--;
				}
				if (l_button2 == true)
				{
					a_hours = a_hours-10;
				}
				if (s_button3 == true)
				{
					a_hours++;
				}
				if (l_button3 == true)
				{
					a_hours = a_hours+10;
				}
				if (a_hours >= 24)
				{
					a_hours = 0;
				}
			}
			if (minute == true)
			{				
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");
				
				if (s_button1 == true)
				{
					minute = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					a_minutes--;
				}
				if (l_button2 == true)
				{
					a_minutes = a_minutes-10;
				}
				if (s_button3 == true)
				{
					a_minutes++;
				}
				if (l_button3 == true)
				{
					a_minutes = a_minutes+10;
				}
				if (a_minutes >= 60)
				{
					a_minutes = 0;
				}
			}
			if (second == true)
			{
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");				
				
				if (s_button1 == true)
				{
					second = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					a_seconds--;
				}
				if (l_button2 == true)
				{
					a_seconds = a_seconds-10;
				}
				if (s_button3 == true)
				{
					a_seconds++;
				}
				if (l_button3 == true)
				{
					a_seconds = a_seconds+10;
				}
				if (a_seconds >= 60)
				{
					a_seconds = 0;
				}
				
			}
		}
		if (UIMODE == 8)
		{
			vTaskSuspend(Clockct);
			sprintf(&Time[0], "%.2i:%.2i:%.2i", hours, minutes, seconds);
			vDisplayClear();
			vDisplayWriteStringAtPos(0,0,"Time Settings");
			vDisplayWriteStringAtPos(1,1,"Time: %s", Time);
			vDisplayWriteStringAtPos(3,0,"S1:B S2:h S3:m S4:s");
			if (hour == false && minute == false && second == false)
			{
				if (s_button1 == true)
				{
					UIMODE = 0;
					s_button1 = false;
					vTaskResume(Clockct);
				}
				if (s_button2 == true)
				{
					hour = true;
					s_button2 = false;
				}
				if (s_button3 == true)
				{
					minute = true;
					s_button3 = false;
				}
				if (s_button4 == true)
				{
					second = true;
					s_button4 = false;
				}
			}
			if (hour == true)
			{
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");
				
				if (s_button1 == true)
				{
					hour = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					hours--;
				}
				if (l_button2 == true)
				{
					hours = hours-10;
				}
				if (s_button3 == true)
				{
					hours++;
				}
				if (l_button3 == true)
				{
					hours = hours+10;
				}
				if (hours >= 24)
				{
					hours = 0;
				}
			}
			if (minute == true)
			{
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");
				
				if (s_button1 == true)
				{
					minute = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					minutes--;
				}
				if (l_button2 == true)
				{
					minutes = minutes-10;
				}
				if (s_button3 == true)
				{
					minutes++;
				}
				if (l_button3 == true)
				{
					minutes = minutes+10;
				}
				if (minutes >= 60)
				{
					minutes = 0;
				}
			}
			if (second == true)
			{
				vDisplayWriteStringAtPos(2,0, "S2:-1 S2_Long:-10");
				vDisplayWriteStringAtPos(3,0,"S3:+1 S3_Long:+10   ");
				
				if (s_button1 == true)
				{
					second = false;
					s_button1 = false;
				}
				if (s_button2 == true)
				{
					seconds--;
				}
				if (l_button2 == true)
				{
					seconds = seconds-10;
				}
				if (s_button3 == true)
				{
					seconds++;
				}
				if (l_button3 == true)
				{
					seconds = seconds+10;
				}
				if (seconds >= 60)
				{
					seconds = 0;
				}
			}	
		}
		if(UIMODE == 0){
			if (s_button2 == true)
			{
				Alarm = Alarm*-1;
			}
			if (l_button3 == true)
			{
				UIMODE = 7;
			}
			if (l_button4 == true)
			{	
				UIMODE = 8;
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
		s_button1 = false;
		s_button2 = false;
		s_button3 = false;
		s_button4 = false;
		l_button1 = false;
		l_button2 = false;
		l_button3 = false;
		l_button4 = false;
		vTaskDelay(200/portTICK_RATE_MS);

		
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
		if (hours == a_hours && minutes == a_minutes && seconds == a_seconds && Alarm == 1)
		{
			vTaskSuspend(UserInt);
			vTaskResume(Alarmct);
			vTaskResume(LEDct);
		}
		sprintf(&Time[0], "%.2i:%.2i:%.2i", hours, minutes, seconds);
		sprintf(&A_Time[0], "%.2i:%.2i:%.2i", a_hours, a_minutes, a_seconds);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void vAlarm(void *pvpParameters){
	
	(void) pvpParameters;

	for (;;)
	{	
		vDisplayClear();
		vDisplayWriteStringAtPos(0,0,"ALARM!");
		vDisplayWriteStringAtPos(1,0,"Press S1 for OFF");
		vDisplayWriteStringAtPos(2,0,"Press S2 for Sleep");
		
		switch (eventbitbutton)
		{
			case 1:
			vTaskSuspend(LEDct);
			PORTE.OUT = 0x0F;
			PORTF.OUT = 0x0F;
			eventbitbutton = xEventGroupClearBits(xButtonEvent,1);
			eventbitbutton = xEventGroupGetBits(xButtonEvent);
			vTaskResume(UserInt);
			vTaskSuspend(Alarmct);
			break;
			case 2:
			eventbitbutton = xEventGroupClearBits(xButtonEvent,2);
			eventbitbutton = xEventGroupGetBits(xButtonEvent);
			a_minutes = a_minutes+5;
			if (a_minutes >=61)
			{
				a_minutes = a_minutes-60;
				a_hours++;
			}
			vTaskResume(UserInt);
			vTaskSuspend(LEDct);
			PORTE.OUT = 0x0F;
			PORTF.OUT = 0x0F;

			vTaskSuspend(Alarmct);
			break;
		}
		vTaskDelay(200/ portTICK_RATE_MS);
	}
	
	
}

void vLED(void *pvParameters){
		
	(void ) pvParameters;
		
	PORTF.DIRSET = 0x0F;
	PORTE.DIRSET = 0x0F;	
	for(;;){
		
		PORTF.OUTTGL = 0x0F;
		PORTE.OUTTGL = 0x0F;
		
		vTaskDelay(100 / portTICK_RATE_MS);
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
		if (getButtonPress(BUTTON2) == LONG_PRESSED)
		{
			eventbitbutton = xEventGroupSetBits(xButtonEvent,22);
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
