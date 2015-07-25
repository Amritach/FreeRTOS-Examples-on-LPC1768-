
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
	  xTaskCreate( MyTask2, ( signed char * )"Task2", configMINIMAL_STACK_SIZE, NULL, 4, &TaskHandle_2 );

	  vTaskStartScheduler();
	}
    else
     UART_Printf("\n\rQueue Created");

	while(1);

	return 0;
}

char TxBuffer[5][MaxElementsPerQueue]={"Hello world", "Good Morning", "welcome", "Good Bye", "Free Rtos"};
char RxBuffer[5][MaxElementsPerQueue];

static void MyTask1(void* pvParameters){

	unsigned char i;

	UART_Printf("\n\rTask1, Reading the data from queue");

	for(i=0;i<5;i++)
    {
	  if(pdTRUE == xQueueReceive(MyQueueHandleId,RxBuffer[i],100))
	   {
	    UART_Printf("\n\rBack in task1, Received data is:");
	    UART_TxString(RxBuffer[i]);
	   }
	  else
       {
        UART_Printf("\n\rBack in task1, No Data received:");
       }
    }   
    
     
	vTaskDelete(TaskHandle_1);
}


static void MyTask2(void* pvParameters){

	unsigned char i;

	UART_Printf("\n\rTask2, Filling the data onto queue");


	for(i=0;i<3;i++)
    {
	   if(pdTRUE == xQueueSend(MyQueueHandleId,TxBuffer[i],100))
	     {
	       UART_Printf("\n\rSuccessfully sent the data");
	     }
	    else
         {
    	   UART_Printf("\n\rSending Failed");
         }
    }      
    
    	for(i=3;i<5;i++)
    	{
        	if(pdTRUE == xQueueSendToFront(MyQueueHandleId,TxBuffer[i],100))
        	{
            	UART_Printf("\n\rSuccessfully sent the data");
        	}
        	else
        	{
            	UART_Printf("\n\rSending Failed");
        	}
    	}   

	  UART_Printf("\n\rExiting task2");
	   vTaskDelete(TaskHandle_2);
}
