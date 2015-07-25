/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   main.c		(FreeRTOS)
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description:  Program to demonstrate the semaphore usage and priority inversion


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
#include "semphr.h"
#include "uart.h"			// Explore Embedded UART library

static void My_LPT(void* pvParameters);
static void My_MPT(void* pvParameters);
static void My_HPT(void* pvParameters);

xTaskHandle LPT_Handle;
xTaskHandle MPT_Handle;
xTaskHandle HPT_Handle;

xSemaphoreHandle Sem_A = NULL;

#define LED_My_LPT    0x02u
#define LED_My_MPT    0x04u
#define LED_My_HPT    0x08u

#define LED_PORT LPC_GPIO2->FIOPIN

int main(void)
{
	SystemInit();			/* Initialize the controller */
	UART_Init(38400);		 /* Initialize the Uart module */	
	LPC_GPIO2->FIODIR = 0xffffffffu;

	vSemaphoreCreateBinary(Sem_A);		/* Create a binary semaphore */

	if(Sem_A != NULL)
	{
		UART_Printf("\n\r\n\nSemaphore successfully created, Creating low priority task");
		xTaskCreate( My_LPT, ( signed char * )"LowTask", configMINIMAL_STACK_SIZE, NULL, 1, &LPT_Handle );
		vTaskStartScheduler();		/* Start the scheduler */
	}
	else
		UART_Printf("\n\rFailed to create Semaphore");

	while(1);

	return 0;
}


static void My_LPT(void* pvParameters)
{
	unsigned char LowPrio;

	LED_PORT = LED_My_LPT;	  /* Led to indicate the execution of My_LPT*/

	LowPrio = uxTaskPriorityGet(LPT_Handle);
	UART_Printf("\n\rLPT:%d,Acquiring semaphore",LowPrio);

	xSemaphoreTake(Sem_A,portMAX_DELAY);

	UART_Printf("\n\rLPT: Creating HPT");
	xTaskCreate( My_HPT, ( signed char * )"HighTask", configMINIMAL_STACK_SIZE, NULL, 3, &HPT_Handle );

	LED_PORT = LED_My_LPT;	  /* Led to indicate the execution of My_LPT*/
	LowPrio = uxTaskPriorityGet(LPT_Handle);
	UART_Printf("\n\rLPT:%d Creating MPT",LowPrio);
	xTaskCreate( My_MPT, ( signed char * )"MidTask", configMINIMAL_STACK_SIZE, NULL, 2, &MPT_Handle );

	LED_PORT = LED_My_LPT;	  /* Led to indicate the execution of My_LPT*/
	LowPrio = uxTaskPriorityGet(LPT_Handle);
	UART_Printf("\n\rLPT:%d waiting for some time",LowPrio);
	vTaskDelay(200);

	LED_PORT = LED_My_LPT;	  /* Led to indicate the execution of My_LPT*/
	LowPrio = uxTaskPriorityGet(LPT_Handle);
	UART_Printf("\n\rLPT:%d Releasing Semaphore",LowPrio);
	xSemaphoreGive(Sem_A);

	LED_PORT = LED_My_LPT;	  /* Led to indicate the execution of My_LPT*/
	LowPrio = uxTaskPriorityGet(LPT_Handle);
	UART_Printf("\n\rFinally Exiting LPT:%d",LowPrio);
	vTaskDelete(LPT_Handle);
}


static void My_MPT(void* pvParameters)
{
	uint8_t MidPrio;

	LED_PORT = LED_My_MPT;	  /* Led to indicate the execution of My_MPT*/

	MidPrio = uxTaskPriorityGet(MPT_Handle);

	UART_Printf("\n\rIn MPT:%d, waiting for some time",MidPrio);		
	vTaskDelay(200);

	LED_PORT = LED_My_MPT;	  /* Led to indicate the execution of My_MPT*/
	UART_Printf("\n\rBack In MPT:%d",MidPrio);
	UART_Printf("\n\rDeleting MPT:%d",MidPrio);

	vTaskDelete(MPT_Handle);
}


static void My_HPT(void* pvParameters)
{
	uint8_t HighPrio;

	LED_PORT = LED_My_HPT;	  /* Led to indicate the execution of My_HPT*/
	HighPrio = uxTaskPriorityGet(HPT_Handle);
	UART_Printf("\n\rIn HPT:%d, trying to Acquire the semaphore",HighPrio);

	HighPrio = uxTaskPriorityGet(HPT_Handle);
	xSemaphoreTake(Sem_A,portMAX_DELAY);
	LED_PORT = LED_My_HPT;	  /* Led to indicate the execution of My_HPT*/
	UART_Printf("\n\rIn HPT:%d, Acquired the semaphore",HighPrio);

	HighPrio = uxTaskPriorityGet(HPT_Handle);
	UART_Printf("\n\rIn HPT:%d, releasing the semaphore",HighPrio);
	xSemaphoreGive(Sem_A);

	LED_PORT = LED_My_HPT;	  /* Led to indicate the execution of My_HPT*/
	UART_Printf("\n\rExiting the HPT");
	vTaskDelete(HPT_Handle);
}

