	
void LedInit(void)
{	
	unsigned int *pReg;
	
	/* ʹ��GPIOB */
	pReg = (unsigned int *)(0x40021000 + 0x18);
	*pReg |= (1<<3);
	
	/* ����GPIOB0Ϊ������� */
	pReg = (unsigned int *)(0x40010C00 + 0x00);
	*pReg |= (1<<0);

	
}

void LedCtrl(int on)
{
	unsigned int *pReg;
	pReg = (unsigned int *)(0x40010C00 + 0x0C);
	
	if (on)
	{
		/* ����GPIOB0���0 */
		*pReg &= ~(1<<0);
	}
	else
	{
		/* ����GPIOB0���1 */
		*pReg |= (1<<0);
	}	
}
