/*
 * keypad.c - KEYPAD-PHONE, Proteus LPC2138
 *
 * EXACT WIRING:
 *   Row A -> P0.0 (pin 19) INPUT + 10k pullup to 3.3V
 *   Row B -> P0.1 (pin 21) INPUT + 10k pullup to 3.3V
 *   Row C -> P0.2 (pin 22) INPUT + 10k pullup to 3.3V
 *   Row D -> P0.3 (pin 26) INPUT + 10k pullup to 3.3V
 *   Col 1 -> P0.4 (pin 27) OUTPUT
 *   Col 2 -> P0.5 (pin 29) OUTPUT
 *   Col 3 -> P0.6 (pin 30) OUTPUT
 *
 *         Col1  Col2  Col3
 * RowA: [  1  ][  2  ][  3  ]
 * RowB: [  4  ][  5  ][  6  ]
 * RowC: [  7  ][  8  ][  9  ]
 * RowD: [  *  ][  0  ][  #  ]
 */

#include <lpc21xx.h>
#include "keypad.h"
#include "lcd.h"

/* ROW pins = INPUT (pulled HIGH by 10k, go LOW on keypress) */
#define ROW_A  (1<<0)   /* P0.0 - pin 19 */
#define ROW_B  (1<<1)   /* P0.1 - pin 21 */
#define ROW_C  (1<<2)   /* P0.2 - pin 22 */
#define ROW_D  (1<<3)   /* P0.3 - pin 26 */

/* COL pins = OUTPUT (we drive LOW one by one) */
#define COL1   (1<<4)   /* P0.4 - pin 27 */
#define COL2   (1<<5)   /* P0.5 - pin 29 */
#define COL3   (1<<6)   /* P0.6 - pin 30 */

extern void delay_ms(unsigned int);

/* keypad_map[row][col] */
char keypad_map[4][3] =
{
    {'1', '2', '3'},   /* Row A */
    {'4', '5', '6'},   /* Row B */
    {'7', '8', '9'},   /* Row C */
    {'*', '0', '#'}    /* Row D */
};

void keypad_init(void)
{
    /* Force P0.0-P0.6 as GPIO */
    PINSEL0 &= ~(0x00003FFF);

    /* COL pins = OUTPUT */
    IODIR0 |= (COL1 | COL2 | COL3);

    /* ROW pins = INPUT */
    IODIR0 &= ~(ROW_A | ROW_B | ROW_C | ROW_D);

    /* All cols HIGH = idle */
    IOSET0 = (COL1 | COL2 | COL3);
}

char get_key(void)
{
    unsigned int row_read;
    unsigned int cols[3] = {COL1, COL2, COL3};
    int r, c;

    for (c = 0; c < 3; c++)
    {
        /* All cols HIGH first */
        IOSET0 = COL1 | COL2 | COL3;

        /* Drive current col LOW */
        IOCLR0 = cols[c];

        /* Settling time */
        delay_ms(5);

        /* Read all rows */
        row_read = IOPIN0 & (ROW_A | ROW_B | ROW_C | ROW_D);

        /* Check which row went LOW */
        r = -1;
        if      (!(row_read & ROW_A)) r = 0;
        else if (!(row_read & ROW_B)) r = 1;
        else if (!(row_read & ROW_C)) r = 2;
        else if (!(row_read & ROW_D)) r = 3;

        if (r >= 0)
        {
            /* Wait for key release with timeout */
            unsigned int timeout = 1000;
            while (timeout--)
            {
                delay_ms(1);
                row_read = IOPIN0 & (ROW_A | ROW_B | ROW_C | ROW_D);
                if ((row_read & ROW_A) && (row_read & ROW_B) &&
                    (row_read & ROW_C) && (row_read & ROW_D))
                    break;
            }
            delay_ms(20);

            /* Restore cols to idle */
            IOSET0 = COL1 | COL2 | COL3;

            return keypad_map[r][c];
        }
    }

    /* No key - restore cols */
    IOSET0 = COL1 | COL2 | COL3;
    return 0;
}

int get_number(void)
{
    char d1, d2;
    int num;

    /* Wait for first valid digit */
    do { d1 = get_key(); } while (d1 == 0 || d1 == '*' || d1 == '#');
    lcd_data(d1);
    
    delay_ms(300);  /* small delay so LCD settles */

    /* Wait for second valid digit */
    do { d2 = get_key(); } while (d2 == 0 || d2 == '*' || d2 == '#');
    lcd_data(d2);

    delay_ms(300);

    num = ((d1 - '0') * 10) + (d2 - '0');
    return num;
}
