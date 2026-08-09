#include <lpc213x.h>

void adc_init(void)
{
    /* P0.28 = AD0.1, PINSEL1 bits[25:24] = 01 */
    PINSEL1 &= ~(3 << 24);   /* clear bits 25:24 */
    PINSEL1 |=  (1 << 24);   /* set 01 = AD0.1 */

    /* AD0CR: channel1, CLKDIV=4, PDN=1 */
    AD0CR = (1 << 1)          /* SEL = channel 1 */
          | (4 << 8)          /* CLKDIV = 4      */
          | (1 << 21);        /* PDN = on        */
}

unsigned int adc_read(void)
{
    unsigned int val;

    /* Start conversion */
    AD0CR |= (1 << 24);

    /* Wait for DONE bit */
    while(!(AD0GDR & (1U << 31)));

    /* Extract 10-bit result bits[15:6] */
    val = (AD0GDR >> 6) & 0x3FF;

    /* Stop conversion */
    AD0CR &= ~(1 << 24);

    return val;
}