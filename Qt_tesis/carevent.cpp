#include "carevent.h"

const unsigned int CarEvent::MonthDays[12]={31,59,90,120,151,181,212,243,273,304,334,365};
const unsigned int CarEvent::MonthDays2[12]={31,28,31,30,31,30,31,31,30,31,30,31};

CarEvent::CarEvent(unsigned int _day, unsigned int _month, unsigned int _year,unsigned int _eventID)
{
day = QString::number(_day);
month = QString::number(_month);
year = QString::number(_year);
eventID = QString::number(_eventID);
}

CarEvent::CarEvent(){
    day=QString("");
    month=QString("");
    year=QString("");
}

unsigned int CarEvent::getDay(){
    return day.toInt();
}

unsigned int CarEvent::getMonth(){
    return month.toInt();
}

unsigned int CarEvent::getYear(){
    return year.toInt();
}

unsigned int CarEvent::getID(){
    return eventID.toInt();
}

void CarEvent::updateDate(unsigned int _day,unsigned int _month,unsigned int _year){
    day = QString::number(_day);
    month = QString::number(_month);
    year = QString::number(_year);
}

unsigned int CarEvent::daysFromNumber(unsigned int _day, unsigned int _month, unsigned int _year){
    if((_day==day.toInt()) & (_month==month.toInt()) & (_year==year.toInt()))
        return 0;
    unsigned int daysTo=0;
    if(_year<=year.toInt()){
        daysTo = (MonthDays[_month-1]+_day)-(MonthDays[month.toInt()-1]+day.toInt());
    }
    else{
        daysTo = (((_year-year.toInt())*365) - (MonthDays[month.toInt()-1]+day.toInt()))
                 +(MonthDays[_month-1]+_day);
    }
    return daysTo;
}

unsigned int CarEvent::daysToNumber(unsigned int _day, unsigned int _month, unsigned int _year){
    return (365*(1+((_year-year.toInt()))) - daysFromNumber(_day,_month,_year));
}

QString CarEvent::daysFromString(unsigned int _day,unsigned int _month,unsigned int _year){
unsigned int tempTimeDays = daysFromNumber(_day,_month,_year);
unsigned int years = tempTimeDays/365;
qDebug("years");
qDebug()<<years;
tempTimeDays = tempTimeDays - years*365;
movingMonth=_month-1;
unsigned int months=0;
while(tempTimeDays>MonthDays2[movingMonth]){
    tempTimeDays=tempTimeDays-MonthDays2[movingMonth];
    months++;
    if(movingMonth==11)
        movingMonth=0;
    else
        movingMonth++;
}
qDebug("months");
qDebug()<<months;
unsigned int days = tempTimeDays;
qDebug("days");
qDebug()<<days;
QString daysFromOuput;
daysFromOuput.sprintf("Pasaron %ua %um %ud",years,months,days);
return daysFromOuput;
}

QString CarEvent::daysToString(unsigned int _day,unsigned int _month,unsigned int _year){
unsigned int tempTimeDays = daysToNumber(_day,_month,_year);
unsigned int years = tempTimeDays/365;
qDebug("years");
qDebug()<<years;
tempTimeDays = tempTimeDays - years*365;
movingMonth=_month-1;
unsigned int months=0;
while(tempTimeDays>MonthDays2[movingMonth]){
    tempTimeDays=tempTimeDays-MonthDays2[movingMonth];
    months++;
    if(movingMonth==11)
        movingMonth=0;
    else
        movingMonth++;
}
qDebug("months");
qDebug()<<months;
unsigned int days = tempTimeDays;
qDebug("days");
qDebug()<<days;
QString daysToOuput;
daysToOuput.sprintf("Faltan %ua %um %ud",years,months,days);
return daysToOuput;
}

QDataStream &operator <<(QDataStream &stream, const CarEvent &myclass)
{
    stream<<myclass.day;
    stream<<myclass.month;
    stream<<myclass.year;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, CarEvent &myclass)
{
    stream>>myclass.day;
    stream>>myclass.month;
    stream>>myclass.year;
    return stream;
}
