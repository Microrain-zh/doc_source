#include <string.h>
#include "font_manager.h"

static PFontLib g_ptFontLibs;

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
void FontLibRegister(PFontLib ptFontLib)
{
	ptFontLib->pNext = g_ptFontLibs;
	g_ptFontLibs = ptFontLib;
}

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
PFontLib __GetFontLib(char *name)
{
	PFontLib pTmp = g_ptFontLibs;
	while (pTmp)
	{
		if (strcmp(pTmp->name, name) == 0)
			return pTmp;
		else
			pTmp = pTmp->pNext;
	}
	return NULL;
}


