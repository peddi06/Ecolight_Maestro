#include "rtc.h"
#include "lcd.h"

/* Days of week string table */
static const char *day_names[7] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

int rtc_hour  = 18;   /* Start at 18:00 to demonstrate night mode */
int rtc_min   = 0;
int rtc_sec   = 0;
int rtc_date  = 20;
int rtc_month = 5;
int rtc_year  = 26;
int rtc_day   = 2;    /* 0=SUN, 1=MON ... 6=SAT  (2 = Tuesday) */

/*  RTC INIT */
void rtc_init(void)
{
    rtc_hour  = 11;   /* daytime - LEDs OFF on startup */
    rtc_min   = 00;
    rtc_sec   = 11;
    rtc_date  = 20;
    rtc_month = 5;
    rtc_year  = 26;
    rtc_day   = 2;
}

/*  RTC UPDATE (call once per second from main loop) */
void rtc_update(void)
{
    rtc_sec++;

    if (rtc_sec >= 60)
    {
        rtc_sec = 0;
        rtc_min++;
    }

    if (rtc_min >= 60)
    {
        rtc_min = 0;
        rtc_hour++;
    }

    if (rtc_hour >= 24)
    {
        rtc_hour = 0;
        rtc_date++;
        rtc_day++;

        if (rtc_day >= 7)
            rtc_day = 0;
    }

    /* Simple date rollover (31-day months; extend if needed) */
    if (rtc_date > 31)
    {
        rtc_date = 1;
        rtc_month++;
    }

    if (rtc_month > 12)
    {
        rtc_month = 1;
        rtc_year++;

        if (rtc_year > 99)
            rtc_year = 0;
    }
}

/*  DISPLAY TIME on LCD line 1: HH:MM:SS  DAY */
void display_time(void)
{
    char buf[16];
    
    lcd_cmd(0x80);
    
    /* HH:MM:SS */
    lcd_data((rtc_hour / 10) + '0');
    lcd_data((rtc_hour % 10) + '0');
    lcd_data(':');
    lcd_data((rtc_min / 10) + '0');
    lcd_data((rtc_min % 10) + '0');
    lcd_data(':');
    lcd_data((rtc_sec / 10) + '0');
    lcd_data((rtc_sec % 10) + '0');
    lcd_data(' ');
    
    /* Day name */
    lcd_string((char *)day_names[rtc_day]);
    
    /* FILL remaining chars with spaces to clear old data */
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
}

void display_date(void)
{
    lcd_cmd(0xC0);
    
    /* DD/MM/YY */
    lcd_data((rtc_date  / 10) + '0');
    lcd_data((rtc_date  % 10) + '0');
    lcd_data('/');
    lcd_data((rtc_month / 10) + '0');
    lcd_data((rtc_month % 10) + '0');
    lcd_data('/');
    lcd_data((rtc_year  / 10) + '0');
    lcd_data((rtc_year  % 10) + '0');
    
    /* FILL remaining with spaces to clear old garbage */
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
    lcd_data(' ');
}
