#if 0
void SystemInit_bak_ok(void)
{
	extern int Image$$ER_IROM1$$Base;
	extern int Image$$ER_IROM1$$Length;
	extern int Load$$ER_IROM1$$Base;
	extern int Image$$RW_IRAM1$$Base;
	extern int Image$$RW_IRAM1$$Length;
	extern int Load$$RW_IRAM1$$Base;
	extern int Image$$RW_IRAM1$$ZI$$Base;
	extern int Image$$RW_IRAM1$$ZI$$Length;	
	
	/* text relocate */
	memcpy(&Image$$ER_IROM1$$Base, &Load$$ER_IROM1$$Base, &Image$$ER_IROM1$$Length);
	
	/* data relocate */
	memcpy(&Image$$RW_IRAM1$$Base, &Load$$RW_IRAM1$$Base, &Image$$RW_IRAM1$$Length);
	
	/* bss clear */
	memset(&Image$$RW_IRAM1$$ZI$$Base, 0, &Image$$RW_IRAM1$$ZI$$Length);
}

void SystemInit(void)
{
	extern int Image$$ER_IROM1$$Base[];
	extern int Image$$ER_IROM1$$Length[];
	extern int Load$$ER_IROM1$$Base[];
	extern int Image$$RW_IRAM1$$Base[];
	extern int Image$$RW_IRAM1$$Length[];
	extern int Load$$RW_IRAM1$$Base[];
	extern int Image$$RW_IRAM1$$ZI$$Base[];
	extern int Image$$RW_IRAM1$$ZI$$Length[];	
	
	/* text relocate */
	if (Image$$ER_IROM1$$Base != Load$$ER_IROM1$$Base)
		memcpy(Image$$ER_IROM1$$Base, LoaImage$$ER_IROM1$$Base, Load$$ER_IROM1$$Based$$ER_IROM1$$Base, Image$$ER_IROM1$$Length);
	
	/* data relocate */
	if (Image$$RW_IRAM1$$Base != Load$$RW_IRAM1$$Base)
		memcpy(Image$$RW_IRAM1$$Base, Load$$RW_IRAM1$$Base, Image$$RW_IRAM1$$Length);
	
	/* bss clear */
	memset(Image$$RW_IRAM1$$ZI$$Base, 0, Image$$RW_IRAM1$$ZI$$Length);
}

#endif

void SystemInit(void)
{
	extern int * Image$$ER_IROM1$$Base;
	extern int * Image$$ER_IROM1$$Length;
	extern int * Load$$ER_IROM1$$Base;
	extern int * Image$$RW_IRAM1$$Base;
	extern int * Image$$RW_IRAM1$$Length;
	extern int * Load$$RW_IRAM1$$Base;
	extern int * Image$$RW_IRAM1$$ZI$$Base;
	extern int * Image$$RW_IRAM1$$ZI$$Length;	
	
	/* text relocate */
	if (&Image$$ER_IROM1$$Base != &Load$$ER_IROM1$$Base)
		memcpy(&Image$$ER_IROM1$$Base, &Load$$ER_IROM1$$Base, &Image$$ER_IROM1$$Length);
	
	/* data relocate */
	if (&Image$$RW_IRAM1$$Base != &Load$$RW_IRAM1$$Base)
		memcpy(&Image$$RW_IRAM1$$Base, &Load$$RW_IRAM1$$Base, &Image$$RW_IRAM1$$Length);
	
	/* bss clear */
	memset(&Image$$RW_IRAM1$$ZI$$Base, 0, &Image$$RW_IRAM1$$ZI$$Length);
}
