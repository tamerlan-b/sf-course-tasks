#include "datetime.hpp"
// TODO: улучшить формат отображения даты и времени

std::fstream& operator >>(std::fstream& is, Date& d)
{
    is >> d.day;
    is >> d.month;
    is >> d.year;
    return is;
}
std::ostream& operator <<(std::ostream& os, const Date& d)
{
    os << d.day;
    os << ' ';
    os << (d.month < 10 ? "0" : "") << d.month;
    os << ' ';
    os << d.year;
    return os;
}


std::fstream& operator >>(std::fstream& is, Time& t)
{
    is >> t.hour;
    is >> t.min;
    is >> t.sec;
    return is;
}
std::ostream& operator <<(std::ostream& os, const Time& t)
{
    os << t.hour;
    os << ' ';
    os << (t.min < 10 ? "0" : "") << t.min;
    os << ' ';
    os << (t.sec < 10 ? "0" : "") << t.sec;
    return os;
}


std::fstream& operator >>(std::fstream& is, DateTime& dt)
{
    is >> dt.date;
    is >> dt.time;
    return is;
}
std::ostream& operator <<(std::ostream& os, const DateTime& dt)
{
    os << dt.date;
    os << ' ';
    os << dt.time;
    return os;
}