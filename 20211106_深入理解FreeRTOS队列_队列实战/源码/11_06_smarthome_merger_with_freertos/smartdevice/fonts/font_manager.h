#ifndef __FONT_MANAGER_H
#define __FONT_MANAGER_H

typedef struct FontBitMap {
	int iLeftUpX;     /* 位图左上角X坐标 */
	int iLeftUpY;     /* 位图左上角Y坐标 */
	int iWidth;       /* 字体宽度 */
	int iRows;        /* 字体高度 */
	int iCurOriginX;  /* 原点X坐标 */
	int iCurOriginY;  /* 原点Y坐标 */
	int iNextOriginX; /* 下一个字符X坐标 */ 
	int iNextOriginY; /* 下一个字符Y坐标 */ 
	unsigned char *pucBuffer; /* 字符点阵 */
}FontBitMap, *PFontBitMap;

typedef struct FontLib {
	char *name;
	int (*FontInit)(struct FontLib *pFontLib);  /* 初始化字库 */
	void (*GetFontSize)(int *piWidth, int *piHeigh);                   /* 获取字体大小 */
	int (*SetFontSize)(int iFontSize);          /* 设置字体大小 */
	int (*GetFontBitMap)(unsigned int dwCode, PFontBitMap ptFontBitMap);  /* 获取某个字符的字库 */
	struct FontLib *pNext;
}FontLib, *PFontLib;

/**********************************************************************
 * 函数名称： FontLibRegister
 * 功能描述： 注册一个字库
 * 输入参数： ptFontLib-字库
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/09	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FontLibRegister(PFontLib ptFontLib);

/**********************************************************************
 * 函数名称： __GetFontLib
 * 功能描述： 获得字库
 * 输入参数： name-字库的名称
 * 输出参数： 无
 * 返 回 值： 成功-PFontLib, 失败-NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/09	     V1.0	  韦东山	      创建
 ***********************************************************************/
PFontLib __GetFontLib(char *name);


#endif /* __FONT_MANAGER_H */


