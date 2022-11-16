# Framebuffer驱动程序框架



## 1.  字符设备驱动程序回顾







## 2. LCD硬件操作原理

碰到LCD点不亮，解决思路：cat 大文件 > /dev/fb0，正常会导致花屏

* 确定背光引脚没问题：输出1、0，不适应PWM
* 确定引脚极性：LCD芯片手册对比DTS
* 确定时序：LCD芯片手册对比DTS
* 检查驱动



## 3. Framebuffer驱动框架



### 3.1 怎么写Framebuffer驱动程序：

* 分配fb_info

  ```c
  struct fb_info *fbinfo;
  fbinfo = framebuffer_alloc(0, &pdev->dev);
  ```

  

* 设置fb_info

  ```c
  fbinfo->fix  // 分配现存、记录地址
  fbinfo->var  // x/y分辨率,  bpp, 红绿蓝的bitfiled    
  ```

  

* 注册fb_info

  ```c
  ret = register_framebuffer(fbinfo);
  ```

* 硬件操作：初始化LCD控制器



### 3.2 理解框架

