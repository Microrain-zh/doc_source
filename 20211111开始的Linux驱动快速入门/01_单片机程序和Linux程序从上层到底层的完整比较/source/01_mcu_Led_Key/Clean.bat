@echo off
chcp 65001 
cls

@echo.
@echo Keil工程清理脚本 
@echo 请将此文件放在要清理项目的根目录下 
@echo www.100ask.net
@echo.        
@echo off

echo 是否清理 Y. 确认           
echo          N. 退出
Echo.  
Set /p var=请选择:
If /i %var%==N (Exit) 

@echo.
echo 清理中……

:: Project/
::del>nul 2>nul *.uvoptx   /s /q 
del>nul 2>nul *.uvguix.* /s /q  
del>nul 2>nul *.scvd     /s /q 

:: Project/DebugConfig
del>nul 2>nul *.dbgconf /s /q  

:: Project/Listings
del>nul 2>nul *.map /s /q  
del>nul 2>nul *.lst /s /q  

:: Project/Objects
del>nul 2>nul *.axf /s /q 
del>nul 2>nul *.htm /s /q 
del>nul 2>nul *.crf /s /q 
del>nul 2>nul *.dep /s /q 
del>nul 2>nul *.lnp /s /q 
del>nul 2>nul *.sct /s /q 
del>nul 2>nul *.hex /s /q 
del>nul 2>nul *.d   /s /q 
del>nul 2>nul *.o   /s /q 

:: Others
del>nul 2>nul *.iex   /s /q 
del>nul 2>nul *.bak   /s /q 

@echo.
Echo 清理完成！
@echo. 
timeout /T 3 /NOBREAK

exit
