#pragma once

#include <ctime>
#include "3rd-party/nlohmann/json.hpp"
#include <iostream>

struct Date
{
    int year;
    int month;
    int day;

    Date(){}
    Date(std::tm* tm): year(tm->tm_year + 1900), month(tm->tm_mon + 1), day(tm->tm_mday)
    {}
    friend std::ostream& operator << (std::ostream& os, const Date& d);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Date, year, month, day)
};

struct Time
{
    int hour;
    int min;
    int sec;
    Time(){}
    Time(std::tm* tm): hour(tm->tm_hour), min(tm->tm_min), sec(tm->tm_sec)
    {}

    friend std::ostream& operator << (std::ostream& os, const Time& t);

    // operator std::tm*() const
    // {
    // }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Time, hour, min, sec)
};

struct DateTime
{
    Date date;
    Time time;

    DateTime(){}
    DateTime(std::tm* tm): date(tm), time(tm)
    {}

    DateTime(std::time_t t): DateTime(std::localtime(&t))
    {
    }

    friend std::ostream& operator << (std::ostream& os, const DateTime& dt);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DateTime, date, time)
};
