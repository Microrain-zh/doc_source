
                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
				EXPORT  __Vectors
				IMPORT HardFault_Handler
				IMPORT UsageFault_Handler
				IMPORT SVC_Handler
				IMPORT SysTick_Handler
					
__Vectors       DCD     0                  
                DCD     Reset_Handler              ; Reset Handler
                DCD     0                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     0          ; MPU Fault Handler
                DCD     0           ; Bus Fault Handler
                DCD     UsageFault_Handler_asm         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     0           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     0             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler
				AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
				EXPORT  Reset_Handler             [WEAK]
                IMPORT  mymain

				IMPORT SystemInit
				IMPORT uart_init
				IMPORT UsageFaultInit
				IMPORT SysTickInit
				IMPORT LedInit

				LDR SP, =(0x20000000+0x10000)
				
				BL SystemInit

				BL uart_init
				
				BL UsageFaultInit
				
				LDR R0, =0
				LDR R1, =0x11111111
				LDR R2, =0x22222222
				LDR R3, =0x33333333
				LDR R12, =0x44444444
				LDR LR, =0x55555555
				
				DCD 0xffffffff
					
				SVC #1
				
				BL SysTickInit
				
				BL LedInit

				;BL mymain
				LDR R0, =mymain
				BLX R0

                ENDP

UsageFault_Handler_asm PROC
				MOV R0, SP
				B UsageFault_Handler
				ENDP

                 END


