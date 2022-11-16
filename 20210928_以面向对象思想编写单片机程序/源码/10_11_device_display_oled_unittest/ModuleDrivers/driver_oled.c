/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_oled.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2021.8.18      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_oled.h"
#include "driver_i2c.h"

extern const uint8_t ascii_font[128][16];

/*
 *  函数名：OLED_WriteCmd
 *  功能描述：I2C发送命令给OLED：开始信号->设备地址->控制字节->命令数据->停止信号
 *  输入参数：cmd-——>发送给OLED的命令数据
 *  输出参数：无
 *  返回值：无
*/
void OLED_WriteCmd(uint8_t cmd)
{
    I2C_Start();
    I2C_WriteByte(OLED_WRITE_ADDR);
    // I2C_WriteByte(OLED_ADDR | OLED_WRITE);
    I2C_WriteByte(0x00);
    I2C_WriteByte(cmd);
    I2C_Stop();
}

/*
 *  函数名：OLED_WriteData
 *  功能描述：I2C发送命令给OLED：开始信号->设备地址->控制字节->GDRAM数据->停止信号
 *  输入参数：data-——>发送给OLED的写入GDRAM的数据
 *  输出参数：无
 *  返回值：无
*/
void OLED_WriteData(uint8_t data)
{
    I2C_Start();
    I2C_WriteByte(OLED_WRITE_ADDR);
    // I2C_WriteByte(OLED_ADDR | OLED_WRITE);
    I2C_WriteByte(0x40);
    I2C_WriteByte(data);
    I2C_Stop();
}

/*
 *  函数名：OLED_WriteNBytes
 *  功能描述：连续发送N个写入显存的数据：开始信号->设备地址->控制字节->N个GDRAM数据->停止信号
 *  输入参数：buf——>指向保存了要发送数据的首地址；length-->发送数据的个数
 *  输出参数：无
 *  返回值：无
*/
void OLED_WriteNBytes(uint8_t *buf, uint16_t length)
{
    uint16_t i = 0;
    if(buf == NULL) return;
    I2C_Start();
    I2C_WriteByte(OLED_WRITE_ADDR);
    I2C_WriteByte(0x40);
    for(i=0; i<length; i++)
    {
        I2C_WriteByte(buf[i]);
    }
    I2C_Stop();
}

/************** 1. 基础命令功能函数 **************/
/*
 *  函数名：OLED_SetContrastValue
 *  功能描述：设置OLED的对比度：发送命令0x81--->发送对比度值
 *  输入参数：value --> 对比度值，范围0~255
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetContrastValue(uint8_t value)
{
    OLED_WriteCmd(0x81);
    OLED_WriteCmd(value);
}

/************** 2. 滚动命令功能函数 **************/
/*
 *  函数名：OLED_H_Scroll
 *  功能描述：让指定页的像素朝着设置的方向按设置的频率水平滚动
 *  输入参数：dir--->滚动方向：朝左或者朝右
 *            start--->起始页
 *            fr_time--->滚动间隔时间，每隔这么多帧水平偏移一列滚动
 *            end--->结束页
 *  输出参数：无
 *  返回值：无
*/
void OLED_H_Scroll(H_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end)
{
    if((dir != H_RIGHT) && (dir != H_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xFF);
}

/*
 *  函数名：OLED_HV_Scroll
 *  功能描述：让指定页的像素朝着设置的方向按设置的频率水平滚动且会将像素的行地址进行偏移offset这么多个单位
 *  输入参数：dir--->滚动方向：朝左或者朝右
 *            start--->起始页
 *            fr_time--->滚动间隔时间，每隔这么多帧水平偏移一列滚动
 *            end--->结束页
 *            offset--->行偏移单位
 *  输出参数：无
 *  返回值：无
*/
void OLED_HV_Scroll(HV_SCROLL_DIR dir, uint8_t start, uint8_t fr_time, uint8_t end, uint8_t offset)
{
    if((dir != HV_RIGHT) && (dir != HV_LEFT))     return;
    if((start>0x07) || (fr_time>0x07) || (end>0x07) || (offset>0x3F))    return;
    OLED_WriteCmd(dir);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(start);
    OLED_WriteCmd(fr_time);
    OLED_WriteCmd(end);
    OLED_WriteCmd(offset);
}

/*
 *  函数名：OLED_SetVScrollArea
 *  功能描述：设置OLED像素垂直滚动的区域
 *  输入参数：area-->顶行区域
 *            row_num--->滚动的行区域
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetVScrollArea(uint8_t area, uint8_t row_num)
{
    if((area>0x3F) || (row_num>0x7F))       return;
    OLED_WriteCmd(0xA3);
    OLED_WriteCmd(area);
    OLED_WriteCmd(row_num);
}

/************** 3. 地址设置功能函数 **************/
static MEM_MODE mem_mode = PAGE_ADDR_MODE;  // 静态局部变量，保存OLED的地址模式的

/*
 *  函数名：OLED_SetColAddr_PAGE
 *  功能描述：设置OLED在页地址模式下的显示起始行地址
 *  输入参数：addr-->起始行地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetColAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 0x7F)   return;
    OLED_WriteCmd(0x00 + (addr & 0x0F));
    OLED_WriteCmd(0x10 + (addr>>4));
}

/*
 *  函数名：OLED_SetMemAddrMode
 *  功能描述：设置OLED的地址模式
 *  输入参数：mode-->地址模式：页地址模式、水平地址模式、垂直地址模式
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetMemAddrMode(MEM_MODE mode)
{
    if((mode != H_ADDR_MODE) && (mode != V_ADDR_MODE) && (mode != PAGE_ADDR_MODE))      return;
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(mode);
    mem_mode = mode;
}

/*
 *  函数名：OLED_SetColAddr_HV
 *  功能描述：设置OLED在水平地址模式或垂直地址模式下像素显示的起始行地址和结束行地址
 *  输入参数：start-->起始行地址
 *            end --->结束行地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetColAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 127) || (end > 127))    return;
    OLED_WriteCmd(0x21);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  函数名：OLED_SetPageAddr_HV
 *  功能描述：设置OLED在水平地址模式或垂直地址模式下像素显示的起始页地址和结束页地址
 *  输入参数：start-->起始页地址
 *            end --->结束页地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetPageAddr_HV(uint8_t start, uint8_t end)
{
    if(mem_mode == PAGE_ADDR_MODE)      return;
    if((start > 7) || (end > 7))        return; 
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(start);
    OLED_WriteCmd(end);
}

/*
 *  函数名：OLED_SetPageAddr_PAGE
 *  功能描述：设置OLED在页地址模式下的显示起始页地址
 *  输入参数：addr-->起始页地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetPageAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 7)   return;
    OLED_WriteCmd(0xB0 + addr);
}

/************** 4. 硬件配置功能函数 **************/
/*
 *  函数名：OLED_SetDispStartLine
 *  功能描述：设置OLED从第line行开始显示，即将默认的0行偏移至line那一行
 *  输入参数：line-->显示的起始行
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetDispStartLine(uint8_t line)
{
    if(line > 63)       return;
    OLED_WriteCmd(0x40 + line);
}

/*
 *  函数名：OLED_SetMuxRatio
 *  功能描述：设置OLED复用率
 *  输入参数：ratio-->复用率值
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetMuxRatio(uint8_t ratio)
{
    if((ratio < 15) || (ratio > 63))      return;
    OLED_WriteCmd(0xA8);
    OLED_WriteCmd(ratio);
}

/*
 *  函数名：OLED_SetDispOffset
 *  功能描述：设置OLED的COM引脚偏移值
 *  输入参数：offset-->COM偏移值
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetDispOffset(uint8_t offset)
{
    if(offset > 63)     return;
    OLED_WriteCmd(0xD3);
    OLED_WriteCmd(offset);
}

/*
 *  函数名：OLED_SetComConfig
 *  功能描述：设置OLED的COM引脚属性
 *  输入参数：mode-->COM引脚模式是连续的还是可选择的
 *            state-->COM引脚翻转与否
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetComConfig(COM_PINS_MODE mode, COM_REMAP_STATE state)
{
    if((mode != COM_PIN_SEQ) && (mode != COM_PIN_ALT))      return;
    if((state != COM_NOREMAP) && (state != COM_REMAP))      return;
    
    OLED_WriteCmd(0xDA);
    OLED_WriteCmd(0x02 + (mode << 4) + (state << 5));
}

/************** 5. 时间设置功能函数 **************/
/*
 *  函数名：OLED_SetDCLK_Freq
 *  功能描述：设置OLED的扫描周期和晶振频率
 *  输入参数：div-->时钟分频系数
 *            freq-->晶振频率
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetDCLK_Freq(uint8_t div, uint8_t freq)
{
    if((div>0x0F) || (freq>0x0F))       return;
    OLED_WriteCmd(0xD5);
    OLED_WriteCmd(div + (freq<<4));
}

/*
 *  函数名：OLED_SetPreChargePeriod
 *  功能描述：设置OLED的预充电周期
 *  输入参数：phase1-->预充电阶段1时间
 *            phase2-->预充电阶段2时间
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetPreChargePeriod(uint8_t phase1, uint8_t phase2)
{
    if((phase1>0x0F) || (phase2>0x0F))       return;
    OLED_WriteCmd(0xD9);
    OLED_WriteCmd(phase1 + (phase2<<4));    
}

/*
 *  函数名：OLED_SetVcomhLevel
 *  功能描述：设置OLED的电压等级
 *  输入参数：level-->电压等级
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetVcomhLevel(VCOMH_LEVEL level)
{
    if((level != LEVEL_0) && (level != LEVEL_1) && (level != LEVEL_2))      return;
    OLED_WriteCmd(0xDB);
    OLED_WriteCmd(level << 4);
}

/************** 6. 电荷碰撞功能函数 **************/
/*
 *  函数名：OLED_SetChargePump
 *  功能描述：打开或关闭OLED的电荷泵
 *  输入参数：state-->电荷泵打开或关闭
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetChargePump(CHARGE_PUMP_STATE state)
{
    if((state != PUMP_DISABLE) && (state != PUMP_ENABLE))   return;
    OLED_WriteCmd(0x8D);
    OLED_WriteCmd((state<<2) + 0x10);
}

/************** 7. OLED初始化函数 **************/
void OLED_Init(void)
{   
    OLED_SetMemAddrMode(PAGE_ADDR_MODE);    // 0. 设置地址模式
    OLED_SetMuxRatio(0x3F);                 // 1. 设置多路复用率
    OLED_SetDispOffset(0x00);               // 2. 设置显示的偏移值
    OLED_SetDispStartLine(0x00);            // 3. 设置起始行
    OLED_SEG_REMAP();                       // 4. 行翻转
    OLED_SCAN_REMAP();                      // 5. 正常扫描
    OLED_SetComConfig(COM_PIN_SEQ, COM_NOREMAP);          // 6. COM 引脚设置
    OLED_SetContrastValue(0x7F);            // 7. 设置对比度
    ENTIRE_DISP_OFF();                      // 8. 全屏点亮/熄灭
    DISP_NORMAL();                          // 9. 显示模式
    OLED_SetDCLK_Freq(0x00, 0x08);          // 10. 设置分频系数和频率增值
    OLED_SetChargePump(PUMP_ENABLE);        // 11. 使能电荷碰撞
    
    OLED_SetComConfig(COM_PIN_ALT, COM_NOREMAP);
    
    DISP_ON();
}

/************** 8. 基本驱动功能函数 **************/
/*
 *  函数名：OLED_SetPosition
 *  功能描述：设置像素显示的起始页和起始列地址
 *  输入参数：page-->页地址模式下的起始页地址
 *            col-->页地址模式下的起始行地址
 *  输出参数：无
 *  返回值：无
*/
void OLED_SetPosition(uint8_t page, uint8_t col)
{
    OLED_SetPageAddr_PAGE(page);
    OLED_SetColAddr_PAGE(col);
}

/*
 *  函数名：OLED_Clear
 *  功能描述：清屏函数
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void OLED_Clear(void)
{
    uint8_t i = 0;
    uint8_t buf[128] = {0};
    
    for(i=0; i<8; i++)
    {
        OLED_SetPosition(i, 0);
        OLED_WriteNBytes(&buf[0], 128);
    }
}

/*
 *  函数名：OLED_Copy
 *  功能描述：把缓冲区的数据写到LCD的显存去
 *  输入参数：buf-缓冲区,
 *  输出参数：无
 *  返回值：无
 *  注意: 如果buf就是LCD实际使用的显存, 则此函数可以写为空函数
 */
void OLED_Copy(uint8_t *buf)
{
    uint8_t i = 0;
    
    for(i=0; i<8; i++)
    {
        OLED_SetPosition(i, 0);
        OLED_WriteNBytes(&buf[i*128], 128);
    }
}


/*
 *  函数名：OLED_PutChar
 *  功能描述：显示一个字符
 *  输入参数：page --> 起始页地址
 *            col --> 起始列地址
 *            c -->   显示的字符
 *  输出参数：无
 *  返回值：无
*/
void OLED_PutChar(uint8_t page, uint8_t col, char c)
{
    OLED_SetPosition(page, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][0], 8);
    
    OLED_SetPosition(page + 1, col);
    OLED_WriteNBytes((uint8_t*)&ascii_font[c][8], 8);
}

/*
 *  函数名：OLED_PrintString
 *  功能描述：显示一个字符串
 *  输入参数：page --> 起始页地址
 *            col --> 起始列地址
 *            str -->   显示的字符串
 *  输出参数：无
 *  返回值：无
*/
void OLED_PrintString(uint8_t page, uint8_t col, char *str)
{
    while(*str != 0)
    {
        OLED_PutChar(page, col, *str);
        col += 8;
        if(col > 127)
        {
            page += 2;
        }
        
        if(page > 7)
        {
            page = 0;
        }
        
        str++;
    }
}
