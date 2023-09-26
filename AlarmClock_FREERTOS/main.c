/*
 * AlarmClock_FREERTOS.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : chaos
 */ 

//#include <avr/io.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

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

TaskHandle_t UserInt;

	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;

void vApplicationIdleHook( void )
{	
	
}

int main(void)
{

    resetReason_t reason = getResetReason();

	vInitClock();
	vInitDisplay();
	
	xTaskCreate(vButtonTask, (const char *) "btTask", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(vButtonTask, (const char *) "Clockct", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vUserInt, (const char *) "UserInt", configMINIMAL_STACK_SIZE, NULL, 2, UserInt);

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
	
	
	for (;;)
	{
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
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

void vButtonTask(void *pvParameters){
	
	(void) pvParameters;
	initButtons();
	
	for (;;)
	{
		
		if(getButtonPress(BUTTON1) == SHORT_PRESSED){
			
			
		}
		if (getButtonPress(BUTTON2) == SHORT_PRESSED)
		{
			
		}
		if (getButtonPress(BUTTON3) == SHORT_PRESSED)
		{
			
		}
		if (getButtonPress(BUTTON4) == SHORT_PRESSED)
		{
			
		}
		vTaskDelay((1000/BUTTON_UPDATE_FREQUENCY_HZ)/portTICK_RATE_MS);
	}
}
