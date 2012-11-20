#include "carevent.h"

CarEvent::CarEvent(QString _day, QString _month, QString _year, QString _eventID,int _test)
{
day=_day;
month=_month;
year=_year;
eventID = _eventID;
test=QString::number(_test);
}

CarEvent::CarEvent(){
    day=QString("");
    month=QString("");
    year=QString("");
    eventID=QString("");
    test=QString::number(0);
}

QDataStream &operator <<(QDataStream &stream, const CarEvent &myclass)
{
    stream<<myclass.day;
    stream<<myclass.month;
    stream<<myclass.year;
    stream<<myclass.eventID;
    stream<<myclass.test;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, CarEvent &myclass)
{
    stream>>myclass.day;
    stream>>myclass.month;
    stream>>myclass.year;
    stream>>myclass.eventID;
    stream>>myclass.test;
    return stream;
}
