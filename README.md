# FreeRTOS_QueueAndTasks

Practical Exam question (30 minutes):

You have a Bluetooth stack definition of the following,

/* A subset of the enumerated types used in the the Bluetooth stack to identify events. */
typedef enum
{
eScanDevicesEvent = 0,           /* Scanning for new devices. */
eHelloRxEvent,                         /* A Device sends a hello packet. */
ePairAcceptEvent,                     /* FreeRTOS_accept() called to accept a new bluetooth pair. */ 
} eBLE_Event_t;

typedef struct BLE_TASK_COMMANDS
{
/* An enumerated type that identifies the event. See the eBLE_Event_tdefinition above. */
eBLE_Event_t  eBLE_EventType;
/* A generic pointer that can hold a value, or point to a buffer. */
void* pvData;
} BLE_StackEvent_t;

You have to send error codes 110, 120, 130 as eScanDevicesEvent  
How would you send this from two different Bus protocol senders over a Queue Set to the CPU ?
Write your application using FreeRTOS APIs and simulate the messages over your console.

Complete your coding and raise your hand once your are done. You have 30 minutes for this part.
