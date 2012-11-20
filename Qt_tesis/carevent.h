#ifndef CAREVENT_H
#define CAREVENT_H

#include <QString>

class CarEvent
{
public:
    QString day;
    QString month;
    QString year;
    QString eventID;
    QString test;

    CarEvent(QString _day,QString _month,QString _year,
                  QString _eventID, int _test);
    CarEvent();

    ~CarEvent(){

    };
    //Overloading of datastream
friend QDataStream &operator >>(QDataStream &stream,CarEvent &myclass);
friend QDataStream &operator <<(QDataStream &stream, const CarEvent &myclass);

private:


};

#endif // CAREVENT_H
