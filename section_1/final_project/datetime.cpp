#include "datetime.hpp"

std::ostream& operator << (std::ostream& os, const Date& d)
{
    os <<  d.day << '.' << (d.month < 10 ? "0" : "") << d.month << '.' << d.year;
    return os;
}
std::ostream& operator << (std::ostream& os, const Time& t)
{
    os << t.hour << ':' << (t.min < 10 ? "0" : "") << t.min << ':' << (t.sec < 10 ? "0" : "") << t.sec;
    return os;
}

std::ostream& operator << (std::ostream& os, const DateTime& dt)
{
    os << dt.time << ' ' << dt.date;
    return os;
}