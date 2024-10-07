#include "FreeRTOS.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "task.h"
#include "usart.h"
#include "led.h"

/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle;

/**************************** 内核对象句柄 **************************/
/* 信号量、消息队列、事件标志组、软件定时器都属于内核的对象，要想使用这些内核对象，
   必须先创建，创建成功之后会返回响应的句柄。这实际上就是一个指针，后续我们就可以
   通过句柄操作这些内核对象 */


/* 空闲任务栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];
/* AppTaskCreate任务栈 */
static StackType_t AppTaskCreate_Stack[128];
/* LED任务栈 */
static StackType_t LED_Task_Stack[128];

/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;
/* AppTaskCreate 任务控制块 */
static StaticTask_t AppTaskCreate_TCB;
/* LED_Task 任务控制块 */
static StaticTask_t LED_Task_TCB;

// #define LED1_TASK_PRIO 3			// 任务优先级
// #define LED1_STK_SIZE 50			// 任务堆栈大小
// TaskHandle_t LED1Task_Handler;		// 任务句柄
// void led1_task(void *pvParameters); // 任务函数

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

static void LED_Task(void)
{
	while(1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);
		printf("led running, state: %d\r\n", LED0);
	}
}

static void AppTaskCreate(void)
{
	taskENTER_CRITICAL();/* 进入临界区 */

	/* 创建LED_Task任务 */
	LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,
										(const char *)"LED_Task",
										(uint32_t)128,
										(void *)NULL,
										(UBaseType_t)4,
										(StackType_t *)LED_Task_Stack,
										(StaticTask_t *)&LED_Task_TCB);

	if(NULL != LED_Task_Handle)	/* 创建成功 */
		printf("LED_Task任务创建成功！\r\n");
	else
		printf("LED_Task创建任务失败！\r\n");

	vTaskDelete(AppTaskCreate_Handle);/* 删除AppTaskCreate任务 */

	taskEXIT_CRITICAL();	/* 退出临界区 */
}

int main(void)
{
	/* 开发板硬件初始化 */
	BSP_Init();
	printf("This is a new freertos\r\n");
	/* 创建AppTaskCreate任务 */
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,
											 (const char *)"AppTaskCreate",
											 (uint32_t)128,
											 (void *)NULL,
											 (UBaseType_t)3,
											 (StackType_t *)AppTaskCreate_Stack,
											 (StaticTask_t *)&AppTaskCreate_TCB);
	if(NULL != AppTaskCreate_Handle) /* 创建成功 */
	{
		vTaskStartScheduler();/* 启动任务，开启调度 */
	}

	while (1)
		;
}
