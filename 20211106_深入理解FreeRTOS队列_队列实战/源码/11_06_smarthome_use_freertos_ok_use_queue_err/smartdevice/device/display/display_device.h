
#ifndef __DISPLAY_DEVICE_H
#define __DISPLAY_DEVICE_H

typedef struct DisplayDevice {
	char *name;
	void *FBBase; /* CPU能直接读写的显存 */
	int iXres;    /* X方向分辨率 */
	int iYres;    /* Y方向分辨率 */
	int iBpp;     /* 每个像素使用多少个像素 */
	int iSize;
	int (*Init)(struct DisplayDevice *ptDev);   /* 硬件初始化 */
	void (*Flush)(struct DisplayDevice *ptDev); /* 把FBBase的数据刷到LCD的显存里 */

	/* 设置FBBase中的数据, 把(iX,iY)的像素设置为颜色dwColor 
	 * dwColor的格式:0x00RRGGBB
	 */
	int (*SetPixel)(struct DisplayDevice *ptDev, int iX, int iY, unsigned int dwColor); 
	struct DisplayDevice *pNext;
}DisplayDevice, *PDisplayDevice;


/**********************************************************************
 * 函数名称： DisplayDeviceRegister
 * 功能描述： 注册一个显示设备
 * 输入参数： ptDisplayDevice-显示设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void DisplayDeviceRegister(PDisplayDevice ptDisplayDevice);

/**********************************************************************
 * 函数名称： __GetDisplayDevice
 * 功能描述： 获得显示设备
 * 输入参数： name-显示设备的名称
 * 输出参数： 无
 * 返 回 值： 成功-PDisplayDevice, 失败-NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PDisplayDevice __GetDisplayDevice(char *name);


#endif /* __DISPLAY_DEVICE_H */

