#include <display_device.h>
#include <font_system.h>
#include "show_text.h"
#include "encoding/encoding.h"

/**********************************************************************
 * 函数名称： GetPixelColorFromBitMap
 * 功能描述： 在位图里, 获取某个像素的数据
 * 输入参数： ptFontBitMap-位图
 * 输入参数： iX,iY-坐标(LCD坐标系)
 * 输出参数： 无
 * 返 回 值： 像素的颜色值
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/12	     V1.0	  韦东山	      创建
 ***********************************************************************/
static unsigned int GetPixelColorFromBitMap(PFontBitMap ptFontBitMap, int iX, int iY)
{
	/* 仅针对8*16的列行式点阵 */
	int x, y;
	unsigned char *byte = ptFontBitMap->pucBuffer;

	x = iX - ptFontBitMap->iLeftUpX;
	y = iY - ptFontBitMap->iLeftUpY;

	if (y < 8)
	{
		byte += x;
	}
	else
	{
		byte += 8 + x;
	}

	if (y >= 8)
		y -= 8;

	return *byte & (1<<y);

}


/**********************************************************************
 * 函数名称： DrawBitMapOnFrameBuffer
 * 功能描述： 在LCD的显存里绘制位图
 * 输入参数： ptDisplayDev-显示设备
 * 输入参数： ptFontBitMap-位图
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/12	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DrawBitMapOnFrameBuffer(PDisplayDevice ptDisplayDev, PFontBitMap ptFontBitMap)
{
	int x;
	int y;
	unsigned int dwColor;

	for (y = ptFontBitMap->iLeftUpY; (y < ptFontBitMap->iLeftUpY + ptFontBitMap->iRows) && (y < ptDisplayDev->iYres); y++)
	{
		for (x = ptFontBitMap->iLeftUpX; (x < ptFontBitMap->iLeftUpX + ptFontBitMap->iWidth) && (x < ptDisplayDev->iXres); x++)
		{	
			/* 从位图中取出每个像素的值 */
			dwColor = GetPixelColorFromBitMap(ptFontBitMap, x, y);

			/* SetPixel */
			ptDisplayDev->SetPixel(ptDisplayDev, x, y, dwColor);
		}
	}
}


/**********************************************************************
 * 函数名称： ShowTextInDisplayDevice
 * 功能描述： 在LCD上绘制字符串
 * 输入参数： ptDisplayDev-显示设备
 * 输入参数： iX,iY-坐标(LCD坐标系)
 * 输入参数： Str-ASCII字符串(目前不支持中文)
 * 输出参数： 无
 * 返 回 值： 0-成功, (-1)-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/12	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ShowTextInDisplayDevice(PDisplayDevice ptDisplayDev, int iX, int iY, char *Str)
{
	int i = 0;
	unsigned int dwCode;
	int len;

	FontBitMap tFontBitMap;

	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;

	while (Str[i])
	{	
		/* 根据指定的编码集/编码格式来处理str
		 * 逐个取出字符的编码值
		 */	
		//dwCode = Str[i];
		len = GetCodeForStr(&Str[i], &dwCode);
		i += len; /* 下一个字符 */

		/* 取出字符的点阵 */
		tFontBitMap.pucBuffer = NULL;
		if (GetFontBitMap(dwCode, &tFontBitMap) == 0)
		{
			/* 在LCD的显存上绘制出来 */
			DrawBitMapOnFrameBuffer(ptDisplayDev, &tFontBitMap);
		}

		tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
		tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
		
	}

	ptDisplayDev->Flush(ptDisplayDev);
	return 0;
	
}

