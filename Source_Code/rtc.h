#ifndef RTC_H
#define RTC_H

extern int rtc_hour;
extern int rtc_min;
extern int rtc_sec;
extern int rtc_date;
extern int rtc_month;
extern int rtc_year;
extern int rtc_day;

void rtc_init(void);
void rtc_update(void);
void display_time(void);
void display_date(void);

#endif
