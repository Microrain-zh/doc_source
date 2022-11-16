# 使用Framebuffer改造SPI_OLED驱动



## 1.  怎么编写Framebuffer驱动

* 分配fb_info
* 设置fb_info
  * fb_var
  * fb_fix
* 注册fb_info
* 硬件操作



## 2. 现场编程

配置内核，把下列配置项去掉：

![image-20220125212414098](pic/01_disable_framebuffer_console.png)