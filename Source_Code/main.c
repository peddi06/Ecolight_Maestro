#include <lpc21xx.h>
#include "lcd.h"
#include "rtc.h"
#include "keypad.h"

/* from interrupt.c */
extern volatile int menu_flag;
void interrupt_init(void);

/* RTC variables */
extern int rtc_hour;
extern int rtc_min;
extern int rtc_sec;
extern int rtc_date;
extern int rtc_month;
extern int rtc_year;

/* ADC functions */
extern void adc_init(void);
extern unsigned int adc_read(void);

/* delay */
extern void delay_ms(unsigned int ms);

/* Checks Night time*/
int is_night_time(void)
{
    /* Night = 18:00 to 05:59 */
    /* Day   = 06:00 to 17:59 */
    if (rtc_hour >= 18)
        return 1;   /* evening/night */
    if (rtc_hour < 6)
        return 1;   /* early morning */
    return 0;       /* daytime */
}

/*  STARTUP SCREEN */
void startup_screen(void)
{
    lcd_cmd(0x01);

    lcd_cmd(0x80);
    lcd_string("Ecolight");

    lcd_cmd(0xC0);
    lcd_string("Maestro");

    delay_ms(500);

    lcd_cmd(0x01);
}

/*  RTC MENU (shown on interrupt)*/
void rtc_menu(void)
{
    lcd_cmd(0x01);

    lcd_cmd(0x80);
    lcd_string("1:EDIT RTC");

    lcd_cmd(0xC0);
    lcd_string("2:EXIT");
}

/*  RTC EDIT MODE */
void rtc_edit_mode(void)
{
    char key;
    unsigned int value;

    /* Keep looping in edit menu until user presses '8' (EXIT) */
    while (1)
    {
        lcd_cmd(0x01);

        lcd_cmd(0x80);
				lcd_string("1:HR 2:MN 3:SC 4:DY");
				lcd_cmd(0xC0);
				lcd_string("5:DT 6:MN 7:YR #:EX");

        /* Wait for a valid key */
        do
        {
            key = get_key();
        } while (key == 0);

        /* ---------- EDIT HOUR ---------- */
        if (key == '1')
        {
            lcd_cmd(0x01);
            lcd_cmd(0x80);
            lcd_string("ENTER HOUR(0-23):");
            lcd_cmd(0xC0);

            value = get_number();

            if (value <= 23)
            {
                rtc_hour = value;
                lcd_cmd(0x01);
                lcd_string("HOUR UPDATED!");
            }
            else
            {
                lcd_cmd(0x01);
                lcd_string("INVALID! (0-23)");
            }
            delay_ms(1000);
        }

        /* ---------- EDIT MINUTE ---------- */
        else if (key == '2')
        {
            lcd_cmd(0x01);
            lcd_cmd(0x80);
            lcd_string("ENTER MIN(0-59):");
            lcd_cmd(0xC0);

            value = get_number();

            if (value <= 59)
            {
                rtc_min = value;
                lcd_cmd(0x01);
                lcd_string("MIN UPDATED!");
            }
            else
            {
                lcd_cmd(0x01);
                lcd_string("INVALID! (0-59)");
            }
            delay_ms(1000);
        }

        /* ---------- EDIT SECOND ---------- */
        else if (key == '3')
        {
            lcd_cmd(0x01);
            lcd_cmd(0x80);
            lcd_string("ENTER SEC(0-59):");
            lcd_cmd(0xC0);

            value = get_number();

            if (value <= 59)
            {
                rtc_sec = value;
                lcd_cmd(0x01);
                lcd_string("SEC UPDATED!");
            }
            else
            {
                lcd_cmd(0x01);
                lcd_string("INVALID! (0-59)");
            }
            delay_ms(1000);
        }

       /* ---------- EDIT DAY ---------- */
else if (key == '4')
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("ENTER DAY:");
    lcd_cmd(0xC0);
    lcd_string("1-7(1=SUN)");
    delay_ms(300);
    
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("ENTER DAY(1-7):");
    lcd_cmd(0xC0);
    
    value = get_number();
    
    /* Adjust: user enters 1-7, store as 0-6 */
    if (value >= 1 && value <= 7)
    {
        rtc_day = value - 1;
        lcd_cmd(0x01);
        lcd_string("DAY UPDATED!");
    }
    else
    {
        lcd_cmd(0x01);
        lcd_string("INVALID! (1-7)");
    }
    delay_ms(1000);
}

/* ---------- EDIT DATE ---------- */
else if (key == '5')
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("ENTER DATE(1-31):");
    lcd_cmd(0xC0);
    value = get_number();
    if (value >= 1 && value <= 31)
    { rtc_date = value; lcd_cmd(0x01); lcd_string("DATE UPDATED!"); }
    else
    { lcd_cmd(0x01); lcd_string("INVALID! 1-31"); }
    delay_ms(1000);
}

/* ---------- EDIT MONTH ---------- */
else if (key == '6')
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("ENTER MON(1-12):");
    lcd_cmd(0xC0);
    value = get_number();
    if (value >= 1 && value <= 12)
    { rtc_month = value; lcd_cmd(0x01); lcd_string("MONTH UPDATED!"); }
    else
    { lcd_cmd(0x01); lcd_string("INVALID! 1-12"); }
    delay_ms(1000);
}

/* ---------- EDIT YEAR ---------- */
else if (key == '7')
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("ENTER YEAR(0-99):");
    lcd_cmd(0xC0);
    value = get_number();
    if (value <= 99)
    { rtc_year = value; lcd_cmd(0x01); lcd_string("YEAR UPDATED!"); }
    else
    { lcd_cmd(0x01); lcd_string("INVALID!"); }
    delay_ms(1000);
}

/* ---------- EXIT ---------- */
else if (key == '#')
{
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_string("RTC UPDATED!");
    lcd_cmd(0xC0);
    lcd_string("Returning...");
    delay_ms(1000);
    break;
}
    }
}

/*  MAIN */
int main(void)
{
    unsigned int light;
    /*
     * P0.10, P0.11, P0.12, P0.13 = LED outputs (bits 10-13)
     * Mask = 0x00003C00
     */
    IODIR0 |= 0x00003C00;

    /* All LEDs OFF at start */
    IOSET0 = 0x00003C00;

    lcd_init();
    rtc_init();
    keypad_init();
    adc_init();
    interrupt_init();

    menu_flag = 0;
    startup_screen();

    while (1)
    {
        /* -------- INTERRUPT: RTC MENU -------- */
        if (menu_flag == 1)
        {
            char key;

            rtc_menu();

            /* Wait for valid key press */
            do
            {
                key = get_key();
            } while (key == 0);

            if (key == '1')
            {
                lcd_cmd(0x01);
                lcd_string("EDIT MODE");
                delay_ms(500);
                rtc_edit_mode();
            }
            else if (key == '2')
            {
                lcd_cmd(0x01);
                lcd_string("RESUMING...");
                delay_ms(500);
            }

            menu_flag = 0;
        }

        /* -------- NORMAL OPERATION -------- */
        else
        {
            // Update software RTC (called every ~1 second via delay below) 
            rtc_update();

            // Display time on line 1 
            display_time();

            // Display date on line 2 
            display_date();

            // ---- LED control only during night hours ---- 
            if (is_night_time())
            {
                light = adc_read();

                if (light < 200)
                {
                    // Dark: turn LEDs ON 
                    IOCLR0 = 0x00003C00;
						}
                else
                {
                    //Bright: turn LEDs OFF 
                    IOSET0 = 0x00003C00;
                }
							}
            else
            {
                // Daytime: always keep LEDs OFF 
                IOSET0 = 0x00003C00;
            }

        // ~1 second loop delay (rtc_update increments seconds each loop) 
        delay_ms(500);
    }
}
				}

