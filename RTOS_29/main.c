#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"





#ifndef F_CPU
#define F_CPU 16000000UL  //  Use unsigned long
#endif

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "queue.h"

#define MaxQueueSize 3
#define MaxElementsPerQueue 20

static void MyTask1(void* pvParameters);
static void MyTask2(void* pvParameters);

xTaskHandle TaskHandle_1;
xTaskHandle TaskHandle_2;

xQueueHandle MyQueueHandleId;

int main(void){

	UART_Init(38400);

    MyQueueHandleId = xQueueCreate(MaxQueueSize,MaxElementsPerQueue);

	if(MyQueueHandleId != 0)
	{
	  UART_Printf("\n\rQueue Created");

	  xTaskCreate( MyTask1, ( signed char * )"Task1", configMINIMAL_STACK_SIZE, NULL, 3, &TaskHandle_1 );
	  xTaskCreate( MyTask2, ( signed char * )"Task3", configMINIMAL_STACK_SIZE, NULL, 2, &TaskHandle_2 );

	  vTaskStartScheduler();
	}
    else
     UART_Printf("\n\rQueue Created");

	while(1);

	return 0;
}


static void MyTask1(void* pvParameters){

    char RxBuffer[MaxElementsPerQueue];
	UART_Printf("\n\rTask1, Reading the data from queue");

	xQueueReceive(MyQueueHandleId,RxBuffer,100);

	UART_Printf("\n\rBack in task1, Received data is:");
	tx_string(RxBuffer);


	vTaskDelete(TaskHandle_1);

}


static void MyTask2(void* pvParameters){

    char TxBuffer[MaxElementsPerQueue]={"Hello world"};

	UART_Printf("\n\rTask2, Filling the data onto queue");

	xQueueSend(MyQueueHandleId,TxBuffer,100);

	UART_Printf("\n\rExiting task2");

	vTaskDelete(TaskHandle_2);
}
