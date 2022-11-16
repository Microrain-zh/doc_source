


int main1(void)
{
	while (1)
	{
		if (read_gpio_key())
		{
			write_gpio_led();
		}
	}
}



typedef struct LEDDevice {
	int group;
	int pin;
	void (*Init)(struct LEDDevice *pDev);
	void (*Control)(struct LEDDevice *pDev, int iStatus);
}LEDDevice, *PLEDDevice;

void ASK100_F103_LED_Init(struct LEDDevice *pDev)
{
	HAL_GPIO_Init(....); /* 配置为输出引脚 */
}

void ASK100_F103_LED_Control(struct LEDDevice *pDev, int iStatus)
{
	HAL_GPIO_WritePin(...., iStatus); /* 设置引脚电平 */
}

static LEDDevice g_tLEDDev = {
	PORTA, PIN5, ASK100_F103_LED_Init, ASK100_F103_LED_Control
};

int main(void )
{
	PLEDDevice pLEDDdev = &g_tLEDDev;
	pLEDDdev->Init(pLEDDdev);

	while (1)
	{
		if (/* 读取按键 */)
		{
			pLEDDdev->Control(pLEDDdev, 1);
		}
	}
}


typedef struct ButtonDevice {
	int group;
	int pin;
	void (*Init)(struct ButtonDevice *pDev);
	int (*GetStatus)(struct ButtonDevice *pDev);
}ButtonDevice, *PButtonDevice;


