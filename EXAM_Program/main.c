#include <stdio.h>
#include <conio.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h" 
#include "queue.h"
 
//Enabling Task delay using macro.
#define INCLUDE_vTaskDelay 1
/*
 * Prototypes for the standard FreeRTOS application hook (callback) functions
 * implemented within this file.  See http://www.freertos.org/a00016.html .
 */
void vApplicationMallocFailedHook(void);

/* An enumerated type used to
identify the source of the data. */

/* A subset of the enumerated types used in the the Bluetooth stack to identify events. */
typedef enum
{
	eScanDevicesEvent = 0,           /* Scanning for new devices. */
	eHelloRxEvent,                         /* A Device sends a hello packet. */
	ePairAcceptEvent,                     /* FreeRTOS_accept() called to accept a new bluetooth pair. */
} eBLE_Event_t;

/* Structure type that will
be passed on the queue. */
typedef struct BLE_TASK_COMMANDS
{
	/* An enumerated type that identifies the event. See the eBLE_Event_tdefinition above. */
	eBLE_Event_t  eBLE_EventType;
	/* A generic pointer that can hold a value, or point to a buffer. */
	void* pvData;
} BLE_StackEvent_t;


/* Notes if the trace is running or not. */
static BaseType_t xTraceRunning = pdFALSE;
 
void vSenderTask(void* pvParameters);
void vReceiverTask(void* pvParameters);

QueueHandle_t xQueue;

int main(void) {

	xQueue = xQueueCreate(10, sizeof(BLE_StackEvent_t));
	/* Two variables of type Data_t that will
	be passed on the queue. */
	static const BLE_StackEvent_t xStructsToSend[2] =
	{
	{ eScanDevicesEvent, 110 }, /* Used by Sender1. */
	{ eScanDevicesEvent, 120 } /* Used by Sender2. */
	};

	if (xQueue != NULL) {
		xTaskCreate(vSenderTask, " Bus protocol Sender 1", configMINIMAL_STACK_SIZE, &xStructsToSend[0], 1, NULL);

		xTaskCreate(vSenderTask, " Bus protocol Sender 2", configMINIMAL_STACK_SIZE, &xStructsToSend[1], 1, NULL);
		xTaskCreate(vReceiverTask, "CPU Reciever", configMINIMAL_STACK_SIZE, NULL, 1, NULL);


		vTaskStartScheduler();
		for (;;);

	}

}



void vSenderTask(void* pvParameters) {
	// Task Handle
	TaskHandle_t myTsk;


	BLE_StackEvent_t *Datatosend;
	//printf("Parameter value : %d \n", pvParameters);
	BaseType_t xStatus;
	const TickType_t xtickstowait = pdMS_TO_TICKS(200);
	Datatosend = (BLE_StackEvent_t *)pvParameters;

	xStatus = xQueueSend(xQueue, &Datatosend, xtickstowait);

	if (xStatus != pdPASS)
	{
		printf("Error occured while sending the Data");
	}
	else {
		myTsk = xTaskGetCurrentTaskHandle();
		printf("==================\nMy %s task  IS RUNNING \n", pcTaskGetName(myTsk));
		printf("Data successfully Sent(eBLE_EventType) : %d \n",Datatosend->eBLE_EventType);
		printf("Data successfully Sent( pvData) : %d \n", Datatosend->pvData);

	}

}


void vReceiverTask(void* pvParameters) {
	// Task Handle
	TaskHandle_t myTsk;

	BLE_StackEvent_t *vRecievedData;
	BaseType_t xStatus;
	int16_t QueueElement_count;
	const TickType_t xtickstowait = pdMS_TO_TICKS(200);

	while (1)
	{
		if (uxQueueMessagesWaiting(xQueue) != 0) {
			printf("Space left in Queue\n");

			//QueueElement_count = uxQueueMessagesWaiting(xQueue);
			//printf("QueueElement_count BEFORE Recieved Data : %d \n", QueueElement_count);

			xStatus = xQueueReceive(xQueue, &vRecievedData, xtickstowait);

			//QueueElement_count = uxQueueMessagesWaiting(xQueue);
			//printf("QueueElement_count AFTER Recieved Data : %d \n", QueueElement_count);

			if (xStatus == pdPASS)
			{
				myTsk = xTaskGetCurrentTaskHandle();
				printf("==================\nMy %s task  IS RUNNING \n", pcTaskGetName(myTsk));
				printf("Rexieved Data (eBLE_EventType): %d \n", vRecievedData->eBLE_EventType);
				printf("Rexieved Data ( pvData): %d \n", vRecievedData->pvData);
			}
			else {
				printf("ERROR: In Reading the DATA \n");
			}
		}
	}


}

/*
void vSendRxDataToTheCPUTask(ScanDeviceBufferDescriptor_t* pxRxedData)
{
	BLE_StackEvent_t xEventStruct;
	/* Complete the IPStackEvent_t structure. The received data is stored in pxRxedData. 
	xEventStruct.eBLE_EventType = eScanDevicesEvent;
	xEventStruct.pvData = (void*)pxRxedData;
	/* Send the IPStackEvent_t structure to the TCP/IP task. 
	xSendEventStructToIPTask(&xEventStruct);


	BaseType_t xStatus;
	const TickType_t xtickstowait = pdMS_TO_TICKS(200);
	Datatosend = (Data_t*)pvParameters;

	xStatus = xQueueSend(xQueue, &xEventStruct, xtickstowait);

	if (xStatus != pdPASS)
	{
		printf("Error occured while sending the Data");
	}
	else {
		printf("Data successfully Sent ucValue : %d \n", xEventStruct->eBLE_EventType);
		printf("Data successfully Sent Source : %d \n", xEventStruct->pvData);

	}

}
*/
void vAssertCalled(unsigned long ulLine, const char* const pcFileName)
{
	static BaseType_t xPrinted = pdFALSE;
	volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

	/* Called if an assertion passed to configASSERT() fails.  See
	http://www.freertos.org/a00110.html#configASSERT for more information. */

	/* Parameters are not used. */
	(void)ulLine;
	(void)pcFileName;

	printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());

	taskENTER_CRITICAL();
	{
		/* Stop the trace recording. */
		if (xPrinted == pdFALSE)
		{
			xPrinted = pdTRUE;
			if (xTraceRunning == pdTRUE)
			{
				// vTraceStop();
				// prvSaveTraceFile();
			}
		}

		/* Cause debugger break point if being debugged. */
		__debugbreak();

		/* You can step out of this function to debug the assertion by using
		the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
		value. */
		while (ulSetToNonZeroInDebuggerToContinue == 0)
		{
			__asm { NOP };
			__asm { NOP };
		}
	}
	taskEXIT_CRITICAL();
}


void vApplicationMallocFailedHook(void)
{
	vAssertCalled(__LINE__, __FILE__);
}
