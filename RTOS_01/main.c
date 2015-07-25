/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   main.c (ARM controllers FreeRTOS)
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Program to demonstrate the task switching between the three tasks to understand 
		working of scheduler.

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 


GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 

Errors and omissions should be reported to codelibraries@exploreembedded.com
 **************************************************************************************************/
	
/* Scheduler include files. */
#include "FreeRtOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "uart.h"		//Explore Embedded UART library

/* Local Tasks declaration */
static void MyTask1(void* pvParameters);
static void MyTask2(void* pvParameters);
static void MyTask3(void* pvParameters);
static void MyIdleTask(void* pvParameters);

#define LED_IdleTask 0x01u
#define LED_Task1    0x02u
#define LED_Task2    0x04u
#define LED_Task3    0x08u
#define LED_Task4    0x10u

#define LED_PORT LPC_GPIO2->FIOPIN

int main(void)
{  
	SystemInit();			/* Initialize the controller */
	UART_Init(38400);		/* Initialize the Uart module */	
	LPC_GPIO2->FIODIR = 0xffffffffu;

	/* Create the three tasks with priorities 1,2,3. Only tasks will be created. 
	 * Tasks will be excecuted once the scheduler is started.
	 * An idle task is also created, which will be run when there are no tasks in RUN state */ 
	xTaskCreate( MyTask1, ( signed char * )"Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( MyTask2, ( signed char * )"Task2", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
	xTaskCreate( MyTask3, ( signed char * )"Task3", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( MyIdleTask, ( signed char * )"IdleTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	UART_Printf("\n\rIn the main");
	
	vTaskStartScheduler();		/* Start the schedular */

	while(1);
}


/* Task1 with priority 1 */
static void MyTask1(void* pvParameters)
{
	while(1)
	{
		LED_PORT = 	LED_Task1;	  /* Led to indicate the execution of Task1*/
		UART_Printf("\n\rTask1");  
		vTaskDelay(150);
	}
}

/* Task1 with priority 2 */
static void MyTask2(void* pvParameters)
{
	while(1)
	{
		LED_PORT = 	LED_Task2;	  /* Led to indicate the execution of Task2*/
		UART_Printf("\n\rTask2");
		vTaskDelay(250);
	}
}

/* Task1 with priority 3 */
static void MyTask3(void* pvParameters)
{
	while(1)
	{
		LED_PORT = 	LED_Task3;	  /* Led to indicate the execution of Task3*/
		UART_Printf("\n\rTask3");
		vTaskDelay(600);
	}
}

/* Task1 with priority 4 */
static void MyIdleTask(void* pvParameters)
{	
	while(1)
	{
		LED_PORT = 	LED_IdleTask;		 /* Led to indicate the execution of Idle Task*/
		UART_Printf("\n\rIn idle state");
	}									 
}

