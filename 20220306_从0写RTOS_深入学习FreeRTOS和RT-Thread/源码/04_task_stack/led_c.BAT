SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;D:\Program Files\Git\cmd;E:\cortexA7_windows_tools\gcc-arm-linux-gnueabihf-6.2.1\bin;C:\Program Files (x86)\GnuWin32\bin;C:\Users\weidongshan\AppData\Local\Microsoft\WindowsApps;D:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin;O:\arm-gcc\gcc-linaro-6.2.1-2016.11-i686-mingw32_arm-linux-gnueabihf\bin;
SET CPU_TYPE=STM32F103ZE
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=led_c
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\start._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\uart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\led_c.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\led_c.axf" --i32combined --output ".\Objects\led_c.hex"
fromelf  --bin  --output=led.bin  Objects\led_c.axf
fromelf  --text  -a -c  --output=led.dis  Objects\led_c.axf
