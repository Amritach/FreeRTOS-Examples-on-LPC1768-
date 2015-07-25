/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   main.c		(FreeRTOS)
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description:  Program to demonstrate the usage of queue to send and receive data between two tasks.
		Queue timeout for not receiving the data within specified amount of time

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
#include "queue.h"
#include "uart.h"			// Explore Embedded UART library

#define MaxQueueSize 3
#define MaxElementsPerQueue 20

static void MyTask1(void* pvParameters);
static void MyTask2(void* pvParameters);

xTaskHandle TaskHandle_1;
xTaskHandle TaskHandle_2;

xQueueHandle MyQueueHandleId;

#define LED_Task1    0x02u
#define LED_Task2   0x04u

#define LED_PORT LPC_GPIO2->FIOPIN

int main(void)
{
	SystemInit();			/* Initialize the controller */
	UART_Init(38400);		 /* Initialize the Uart module */	
	LPC_GPIO2->FIODIR = 0xffffffffu;

	MyQueueHandleId = xQueueCreate(MaxQueueSize,MaxElementsPerQueue);	/* Cretae a queue */

	if(MyQueueHandleId != 0)
	{
		UART_Printf("\n\rQueue Created");

		xTaskCreate( MyTask1, ( signed char * )"Task1", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_1 );
		xTaskCreate( MyTask2, ( signed char * )"Task2", configMINIMAL_STACK_SIZE, NULL, 2, &TaskHandle_2 );

		vTaskStartScheduler();	/* start the scheduler */
	}
	else
		UART_Printf("\n\rQueue not Created");

	while(1);

	return 0;
}


static void MyTask1(void* pvParameters)
{
	char RxBuffer[MaxElementsPerQueue];
	
	LED_PORT = LED_Task1;	  /* Led to indicate the execution of Task1*/
	UART_Printf("\n\rTask1, Reading the data from queue");
	
	if(pdTRUE == xQueueReceive(MyQueueHandleId,RxBuffer,100))
	{
		LED_PORT = LED_Task1;	  /* Led to indicate the execution of Task1*/
		UART_Printf("\n\rBack in task1, Received data is:%s",RxBuffer);
	}
	else
	{
		LED_PORT = LED_Task1;	  /* Led to indicate the execution of Task1*/
		UART_Printf("\n\rBack in task1, No Data received:");
	}

	vTaskDelete(TaskHandle_1);
}


static void MyTask2(void* pvParameters)
{
	char TxBuffer[MaxElementsPerQueue]={"Hello world"};
	
	LED_PORT = LED_Task2;	  /* Led to indicate the execution of Task2*/

	UART_Printf("\n\rTask2, Waiting for some time");
	vTaskDelay(200);
	
	LED_PORT = LED_Task2;	  /* Led to indicate the execution of Task2*/
	UART_Printf("\n\rTask2, Filling the data onto queue");
	
	if(pdTRUE == xQueueSend(MyQueueHandleId,TxBuffer,100))
	{
		LED_PORT = LED_Task2;	  /* Led to indicate the execution of Task2*/
		UART_Printf("\n\rSuccessfully sent the data");
	}
	else
	{
		LED_PORT = LED_Task2;	  /* Led to indicate the execution of Task2*/
		UART_Printf("\n\rSending Failed");
	}

	UART_Printf("\n\rExiting task2");
	vTaskDelete(TaskHandle_2);
}
