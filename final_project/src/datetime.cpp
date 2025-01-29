#include "datetime.hpp"
#include <algorithm>
#include <sstream>
// TODO: улучшить формат отображения даты и времени

std::istream& operator>>(std::istream& is, Date& d)
{
    is >> d.day;
    is >> d.month;
    is >> d.year;
    return is;
}
std::ostream& operator<<(std::ostream& os, const Date& d)
{
    os << d.day;
    os << ' ';
    os << (d.month < 10 ? "0" : "") << d.month;
    os << ' ';
    os << d.year;
    return os;
}

std::istream& operator>>(std::istream& is, Time& t)
{
    is >> t.hour;
    is >> t.min;
    is >> t.sec;
    return is;
}
std::ostream& operator<<(std::ostream& os, const Time& t)
{
    os << t.hour;
    os << ' ';
    os << (t.min < 10 ? "0" : "") << t.min;
    os << ' ';
    os << (t.sec < 10 ? "0" : "") << t.sec;
    return os;
}

std::istream& operator>>(std::istream& is, DateTime& dt)
{
    is >> dt.date;
    is >> dt.time;
    return is;
}
std::ostream& operator<<(std::ostream& os, const DateTime& dt)
{
    os << dt.date;
    os << ' ';
    os << dt.time;
    return os;
}

std::string Date::to_string() const
{
    std::string d_str = "";
    d_str += std::to_string(year) + "-";
    if (month < 10)
    {
        d_str += "0";
    }
    d_str += std::to_string(month) + "-";

    if (day < 10)
    {
        d_str += "0";
    }
    d_str += std::to_string(day);

    return d_str;
}

std::string Time::to_string() const
{
    std::string t_str = "";
    if (hour < 10)
    {
        t_str += "0";
    }
    t_str += std::to_string(hour) + ":";

    if (min < 10)
    {
        t_str += "0";
    }
    t_str += std::to_string(min) + ":";

    if (sec < 10)
    {
        t_str += "0";
    }
    t_str += std::to_string(sec);

    return t_str;
}

DateTime DateTime::parse(std::string dt_str)
{
    // Format: YYYY-MM-DD HH:MM:S.S (2025-01-28 21:18:26.458635)

    std::replace(dt_str.begin(), dt_str.end(), '-', ' ');
    std::replace(dt_str.begin(), dt_str.end(), ':', ' ');

    std::stringstream sstr;
    sstr << dt_str;

    DateTime dt;
    sstr >> dt.date.year;
    sstr >> dt.date.month;
    sstr >> dt.date.day;
    sstr >> dt.time.hour;
    sstr >> dt.time.min;
    double secs;
    sstr >> secs;
    dt.time.sec = static_cast<int>(secs);
    return dt;
}

std::string DateTime::to_string() const { return date.to_string() + " " + time.to_string(); }