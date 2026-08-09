#include <lpc21xx.h>

volatile int menu_flag = 0;

/* ISR */
void __irq EINT0_ISR(void)
{
    menu_flag = 1;

    EXTINT = 0x01;      // Clear EINT0 interrupt flag
    VICVectAddr = 0;    // End of interrupt
}

/* INTERRUPT INIT */
void interrupt_init(void)
{
    PINSEL1 |= 0x01;    // P0.16 = EINT0

    EXTMODE = 0x01;     // Edge triggered
    EXTPOLAR = 0x00;    // Falling edge triggered

    EXTINT = 0x01;      // Clear pending interrupt

    VICIntSelect = 0x00;

    VICVectAddr0 = (unsigned long)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14;

    VICIntEnable = (1 << 14);
}
