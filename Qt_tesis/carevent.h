#ifndef CAREVENT_H
#define CAREVENT_H

#include <QString>
#include <QDebug>
#define TM_EVENT 0
#define SOAT_EVENT 1
#define OIL_EVENT 2
#define GAS_EVENT 3

class CarEvent
{
public:

    CarEvent(unsigned int _day,unsigned int _month,unsigned int _year,unsigned int _eventId);
    CarEvent();

    ~CarEvent(){

    };

    unsigned int getDay();
    unsigned int getMonth();
    unsigned int getYear();
    unsigned int getID();
    void updateDate(unsigned int _day,unsigned int _month,unsigned int _year);
    unsigned int daysToNumber(unsigned int _day,unsigned int _month,unsigned int _year);
    unsigned int daysFromNumber(unsigned int _day,unsigned int _month,unsigned int _year);
    QString daysToString(unsigned int _day,unsigned int _month,unsigned int _year);
    QString daysFromString(unsigned int _day,unsigned int _month,unsigned int _year);


    //Overloading of datastream
friend QDataStream &operator >>(QDataStream &stream,CarEvent &myclass);
friend QDataStream &operator <<(QDataStream &stream, const CarEvent &myclass);

private:
    QString day;
    QString month;
    QString year;
    QString eventID;
    static const unsigned int MonthDays[12];
    static const unsigned int MonthDays2[12];
};

#endif // CAREVENT_H
