#include "sys.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
 
/************************************************
 ALIENTEK战舰STM32开发板 FreeRTOS 实验2-1
 跑马灯实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define START_TASK_PRIO		1 		//任务优先级
#define START_STK_SIZE		128 	//任务堆栈大小
TaskHandle_t StartTask_Handler;		//任务句柄
void start_task(void *pvParameters);//任务函数

#define LED0_TASK_PRIO		2		//任务优先级
#define LED0_STK_SIZE		50 		//任务堆栈大小
TaskHandle_t LED0Task_Handler;		//任务句柄
void led0_task(void *pvParameters);		//任务函数

#define LED1_TASK_PRIO		3 		//任务优先级
#define LED1_STK_SIZE		50 		//任务堆栈大小
TaskHandle_t LED1Task_Handler;		//任务句柄
void led1_task(void *pvParameters);		//任务函数


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	uart_init(115200);	//初始化串口
	LED_Init();		  	//初始化LED

	//创建开始任务
	xTaskCreate((TaskFunction_t)start_task,
				(const char *)"start task",
				(uint16_t)START_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)START_TASK_PRIO,
				(TaskHandle_t *)&StartTask_Handler);
	vTaskStartScheduler();
 }


//开始任务函数
void start_task(void*pvParameters)
{
	taskENTER_CRITICAL();//进入临界段
	//创建LED0任务
	xTaskCreate((TaskFunction_t)led0_task,				//任务函数
				(const char *)"led0 task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);
	//创建LED1任务
	xTaskCreate((TaskFunction_t)led1_task,				//任务函数
				(const char *)"led1 task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);
	vTaskDelete(StartTask_Handler);	//删除开始任务
	taskEXIT_CRITICAL();			//退出临界区
}

//LED0任务函数
void led0_task(void *pvParameters)
{
	while (1)
	{
		LED0 = ~LED0;
		vTaskDelay(500);
	}
}

//LED1任务函数
void led1_task(void* pvParameters)
{
	while (1)
	{
		LED1 = 0;
		vTaskDelay(200);
		LED1 = 1;
		vTaskDelay(800);
	}
}
