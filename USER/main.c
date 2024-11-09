#include "FreeRTOS.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "task.h"
#include "usart.h"
#include "led.h"

/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1任务句柄 */
static TaskHandle_t LED1_Task_Handle = NULL;
/* LED2任务句柄 */
static TaskHandle_t LED2_Task_Handle = NULL;

/**************************** 内核对象句柄 **************************/


/* 空闲任务栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;

/**
 * *******************************************
 * @brief 获取空闲任务的任务栈与任务控制块内存
 * @param *ppxIdleTaskStackBuffer 任务控制块内存
 * @param *ppxIdleTaskStackBuffer 任务栈内存
 * @param *pulIdleTaskStackSize	 任务堆栈大小
 * @author Sunph5
 * @version V1.0.0
 * @date 2024-10-07
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * **********************************************
 * @brief 获取定时器任务的任务栈和任务控制块内存
 * @param *ppxTimerTaskTCBBuffer 任务控制块内存
 * @param *ppxTimerTaskStackBuffer 任务栈内存
 * @param *pulTimerTaskStackSize 任务栈大小
 * @author Sunph5
 * @version V1.0.0
 * @date 2024-10-07
 */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

static void BSP_Init(void)
{
	/* 优先级的分组，分为4组，即4位都用来表示抢占式优先级，范围为0~15 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4

	/* LED初始化 */
	LED_GPIO_Config();

	/* 串口初始化 */
	USART_Config(115200);
}

static void LED1_Task(void *pvParameters)
{
	while(1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);
		printf("led1 running, state: %d\r\n", LED0);
	}
}

static void LED2_Task(void *pvParameters)
{
	while(1)
	{
		LED1 = ~LED1;
		vTaskDelay(1000);
		printf("led2 running, state: %d\r\n", LED1);
	}
}

static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;		/* 定义一个创建信息返回值，默认为pdPASS */

	taskENTER_CRITICAL();/* 进入临界区 */

	/* 创建LED1_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t)LED1_Task,
						  (const char *)"LED1_Task",
						  (uint32_t)64,
						  (void *)NULL,
						  (UBaseType_t)2,
						  (TaskHandle_t *)&LED1_Task_Handle);
	
	if(pdPASS == xReturn)	/* 创建成功 */
		printf("LED1_Task任务创建成功！\r\n");

	/* 创建LED1_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t)LED2_Task,
						  (const char *)"LED2_Task",
						  (uint32_t)64,
						  (void *)NULL,
						  (UBaseType_t)3,
						  (TaskHandle_t *)&LED2_Task_Handle);

	if(pdPASS == xReturn)	/* 创建成功 */
		printf("LED2_Task任务创建成功！\r\n");

	vTaskDelete(AppTaskCreate_Handle);/* 删除AppTaskCreate任务 */

	taskEXIT_CRITICAL();	/* 退出临界区 */
}

int main(void)
{
	BaseType_t xReturn = pdPASS;
	/* 开发板硬件初始化 */
	BSP_Init();
	printf("This is a new freertos\r\n");

	/* 创建动态AppTaskCreate任务 */
	xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,
						  (const char *)"AppTaskCreate",
						  (uint32_t)128,
						  (void *)NULL,
						  (UBaseType_t)3,
						  (TaskHandle_t *)&AppTaskCreate_Handle);
	if(xReturn == pdPASS) /* 创建成功 */
	{
		vTaskStartScheduler();/* 启动任务，开启调度 */
	}
	else
		return -1;

	while (1)
		;
}
