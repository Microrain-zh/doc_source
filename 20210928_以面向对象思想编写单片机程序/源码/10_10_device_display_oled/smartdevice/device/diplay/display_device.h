
#ifndef __DISPLAY_DEVICE_H
#define __DISPLAY_DEVICE_H

typedef struct DispayDevice {
	char *name;
	void *FBBase; /* CPU能直接读写的显存 */
	int iXres;    /* X方向分辨率 */
	int iYres;    /* Y方向分辨率 */
	int iBpp;     /* 每个像素使用多少个像素 */
	int (*Init)(struct DispayDevice *ptDev);   /* 硬件初始化 */
	void (*Flush)(struct DispayDevice *ptDev); /* 把FBBase的数据刷到LCD的显存里 */

	/* 设置FBBase中的数据, 把(iX,iY)的像素设置为颜色dwColor 
	 * dwColor的格式:0x00RRGGBB
	 */
	int (*SetPixel)(struct DispayDevice *ptDev, int iX, int iY, unsigned int dwColor); 
}DispayDevice, *PDispayDevice;

PLEDDevice GetDispayDevice(int which);


#endif /* __DISPLAY_DEVICE_H */

