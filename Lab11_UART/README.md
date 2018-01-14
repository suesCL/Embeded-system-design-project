## Overview 
The software converts numbers into ASCII strings and display the string on the display connected to UART0. 

* Part a) 

You will write an I/O driver routine that outputs strings to the UART0 device. 


See the comments in the UART.h and UART.c for more detailed descriptions of how this UART_OutString function is to work.

* Part b) 

You will write an I/O driver routine that outputs an unsigned decimal number to the UART0 device. 
See the comments in the UART.h and UART.c for more detailed descriptions of how these UART_ConvertUDec and UART_OutUDec functions are to work.


* Part c) 

Assume the system stores the integers 0 to 9999, but the values mean 0.000 to 9.999 cm. For example, in the software a variable might contain 1234, but that value actually means 1.234 cm. You will write an I/O driver routine that outputs the value of the distance to the UART0 device. See the comments in the UART.h and UART.c for more detailed descriptions of how these UART_ConvertDistance and UART_OutDistance functions are to work.
