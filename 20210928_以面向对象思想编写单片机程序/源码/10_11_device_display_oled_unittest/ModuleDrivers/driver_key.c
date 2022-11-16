#include <input_system.h>
#include <kal_time.h>
#include <gpio_key.h>
#include <input_buffer.h>
#include "driver_key.h"

static volatile uint8_t key1_val = KEY_RELEASED;
static volatile uint8_t key2_val = KEY_RELEASED;

void KEY_GPIO_ReInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	KEY1_GPIO_CLK_EN();
	KEY2_GPIO_CLK_EN();

	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	GPIO_InitStruct.Pin = KEY1_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = KEY2_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(KEY1_PIN);
	HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	InputEvent event;

	/* 消除抖动 */
	
	if(KEY1_PIN == GPIO_Pin)
	{
		/* 构造InputEvent */
		event.time  = KAL_GetTime();  /* kal */
		event.eType = INPUT_EVENT_TYPE_KEY;
		event.iKey  = K1_CODE;

		/* 读取GPIO电平, GPIO为0表示按下, 此时压力是1,松开后压力是0 */
		event.iPressure = !K1_STATUS;  

		/* 放入环形缓冲区 */
		PutInputEvent(&event);
	}
	else if(KEY2_PIN == GPIO_Pin)
	{
		/* 构造InputEvent */
		event.time	= KAL_GetTime();  /* kal */
		event.eType = INPUT_EVENT_TYPE_KEY;
		event.iKey	= K2_CODE;

		/* 读取GPIO电平, GPIO为0表示按下, 此时压力是1,松开后压力是0 */
		event.iPressure = !K2_STATUS;

		/* 放入环形缓冲区 */
		PutInputEvent(&event);
	}
}

uint8_t KEY1_Value(void)
{
	return key1_val;
}

uint8_t KEY2_Value(void)
{
	return key2_val;
}
