#pragma once

#include <ctime>
#include <iostream>
#include <fstream>

struct Date
{
    int year;
    int month;
    int day;

    Date() = default;
    Date(std::tm* tm): year(tm->tm_year + 1900), month(tm->tm_mon + 1), day(tm->tm_mday)
    {}

    friend std::fstream& operator >>(std::fstream& is, Date& d);
    friend std::ostream& operator <<(std::ostream& os, const Date& d);
};

struct Time
{
    int hour;
    int min;
    int sec;
    Time() = default;
    Time(std::tm* tm): hour(tm->tm_hour), min(tm->tm_min), sec(tm->tm_sec)
    {}

    friend std::fstream& operator >>(std::fstream& is, Time& t);
    friend std::ostream& operator <<(std::ostream& os, const Time& t);
};

struct DateTime
{
    Date date;
    Time time;

    DateTime() = default;
    DateTime(std::tm* tm): date(tm), time(tm)
    {}

    DateTime(std::time_t t): DateTime(std::localtime(&t))
    {}

    friend std::fstream& operator >>(std::fstream& is, DateTime& dt);
    friend std::ostream& operator <<(std::ostream& os, const DateTime& dt);
};