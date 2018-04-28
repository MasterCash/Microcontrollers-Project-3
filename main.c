//include <reg51.h>
#include <reg932.h>
#include "uart.h"


void main()
{
	P1M1 = 0x00;
	EA = 1;
	//to remove warnings**
	uart_init();
	uart_transmit('a');
	uart_get();
	//********************
	
	//stuff herex
	
}