#include "MainWindow.h"
#include <QWSServer>

//#include "serial_helper.h"
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <QtCore>
#include <termios.h>
#include <QtGui>
#include <string.h>
#include <fstream>
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include "QKeyPushButton.h"
#include "widgetKeyBoard.h"
#include "examplemyfocus.h"
#include "customtm.h"
#include <QMessageBox>
#include <QDebug>


using namespace std;

extern "C"
{
#include <serial_helper.h>
}

// The Constructor calls the subfunctions for creation of the sample application
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

 //  if(QWSServer::isCursorVisible() == true) {QWSServer::setCursorVisible(false);}
    DateLabel=new QLabel();
    outOfBounds=true;
    ttyfd = serial_open("/dev/ttySP1", 115200, CLOCAL | CREAD | CS8, 1);
    timer1= new QTimer(this);
    timer2= new QTimer(this);
    timer3= new QTimer(this);
     //typedef struct  tm ComInfo;
    comObjectPtr= new ComInfo;
    comObjectSetDate =new ComInfo;
    qDebug("size: %d",sizeof(*comObjectSetDate));

    FuelImage= new QLabel();
    animTime=0;
    FuelValue=0;
    TempValue=0;
    flag=false;
    slide=0;
    count=0;
    count1=0;
    vel=0;
    i=0;
    b=0;




    strcpy(CommandAte0,"ATE0\r");
    //strcpy(Commandspeed,"ATDP\r");
    //strcpy(CommandRpm, "AT I\r");
    strcpy(Commandspeed,"010D1\r");
    strcpy(CommandRpm, "010C2\r");
    strcpy(CommandFuel,"012f1\r");
    strcpy(CommandTemp,"01051\r");
    strcpy(CommandGetTime,"111111111111111\r");
    strcpy(CommandSetTime,"222222222222222\r");

     for(int l=0;l<64;l++){
         qRpmAnswer[l]=0;
         RpmAnswer[l]=0;
         qSpeedAnswer[l]=0;
         SpeedAnswer[l]=0;
     }


//    write (ttyfd, CommandAte0, 5);
//    qDebug("write echo");
//    read  (ttyfd, Ate0Answer, sizeof(Ate0Answer));
//    qDebug("read echo");

    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);
    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(metaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));

    Phonon::createPath(mediaObject, audioOutput);

    setupActions();
   // setupMenus();
    setupUi();
    addFiles();
    timeLcd->setText("00:00");

    createGuiControlComponents();
    createSubSlidingWidgets();
    createSlidingStackedWidget();
    createMainLayout();
    createConnections();
    tcflush(ttyfd,TCIOFLUSH);

    //timer1->start(200);
    //timer2->start(20001);
    timer3->start(1000*60);
    qDebug("timer Start");

    GetDate();

}

MainWindow::~MainWindow() {

}
////////////////////////CONTROL COMPONENTS////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createGuiControlComponents() {
    int KeyboardIndx;
    KeyboardIndx=3;
    animTime=0;

    ButtonMusic = new QPushButton();
    ButtonMusic->setFixedSize( 95,35 );
    //ButtonMusic->setStyleSheet("color: white;background-color: black;border-style: outset;border-width: 2px;border-radius: 5px;border-color: beige; font: bold 14px;max-width: 10em;padding: 6px;");

    ButtonKeyboard = new QPushButton();
    ButtonKeyboard->setFixedSize(95,35);
    //ButtonKeyboard->setStyleSheet("color: white; background-color: black;border-style: outset;border-width: 2px;border-radius: 5px;border-color: beige; font: bold 14px;max-width: 10em;padding: 6px;");

    ButtonRun = new QPushButton();
    ButtonRun->setFixedSize(95,35);
    //ButtonRun->setStyleSheet("color: white; background-color: black;border-style: outset;border-width: 2px;border-radius: 5px;border-color: beige; font: bold 14px;max-width: 10em;padding: 6px;");

    ButtonCalendar = new QPushButton();
    ButtonCalendar->setFixedSize(95,35);



    PixmapMusicOff= QPixmap(":/images/MusicOff.png");
    PixmapMusicOn=QPixmap(":/images/MusicOn.png");
    ButtonMusicIconOff= QIcon(PixmapMusicOff);
    ButtonMusicIconOn= QIcon(PixmapMusicOn);

    ButtonMusic->setIcon(ButtonMusicIconOff);
    ButtonMusic->setIconSize(PixmapMusicOff.rect().size());

    PixmapRunOff= QPixmap(":/images/RunOff.png");
    PixmapRunOn=QPixmap(":/images/RunOn.png");
    ButtonRunIconOff= QIcon(PixmapRunOff);
    ButtonRunIconOn= QIcon(PixmapRunOn);

    ButtonRun->setIcon(ButtonRunIconOn);
    ButtonRun->setIconSize(PixmapRunOn.rect().size());

    PixmapKeyboardOff= QPixmap(":/images/ObdOff.png");
    PixmapKeyboardOn=QPixmap(":/images/ObdOn.png");
    ButtonKeyboardIconOff= QIcon(PixmapKeyboardOff);
    ButtonKeyboardIconOn= QIcon(PixmapKeyboardOn);

    ButtonKeyboard->setIcon(ButtonKeyboardIconOff);
    ButtonKeyboard->setIconSize(PixmapKeyboardOff.rect().size());

    PixmapCalendarOff= QPixmap(":/images/CalOff.png");
    PixmapCalendarOn=QPixmap(":/images/CalOn.png");
    ButtonCalendarIconOff= QIcon(PixmapCalendarOff);
    ButtonCalendarIconOn= QIcon(PixmapCalendarOn);

    ButtonCalendar->setIcon(ButtonCalendarIconOff);
    ButtonCalendar->setIconSize(PixmapCalendarOff.rect().size());

    connect(ButtonMusic,SIGNAL(pressed()),this,SLOT(SetIndexMusic()));
    connect(ButtonKeyboard,SIGNAL(pressed()),this,SLOT(SetIndexKeyBoard()));
    connect(ButtonRun,SIGNAL(pressed()),this,SLOT(SetIndexRun()));
    connect(ButtonCalendar,SIGNAL(pressed()),this,SLOT(SetIndexCalendar()));
}

////////////////////////MAIN LAYOUT////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createMainLayout() {
    centralWidget=new QWidget(this);
    mainLayout=new QHBoxLayout();
    centralWidget->setLayout(mainLayout);
    RightPaneLayout=new QVBoxLayout();
    MainCenterPanelLayout=new QVBoxLayout();
    RightPanelWidget=new QWidget();
    MainCenterPanelWidget=new QWidget();

    RightPanelWidget->setLayout(RightPaneLayout);
    MainCenterPanelWidget->setLayout(MainCenterPanelLayout);

    QPixmap fuel("fuel.png");
    FuelImage->setPixmap(fuel);

    FuelBar = new QProgressBar(this);
    FuelBar->setTextDirection(QProgressBar::TopToBottom);
    FuelBar->setOrientation(Qt::Vertical);
    FuelBar->setRange(0, 100);
    FuelBar->setTextVisible(false);
    FuelBar->setFixedSize(20,102);
    FuelLevelChanged(40);

    TempBar = new QProgressBar(this);
    TempBar->setTextDirection(QProgressBar::TopToBottom);
    TempBar->setOrientation(Qt::Vertical);
    TempBar->setRange(15, 140);
    TempBar->setTextVisible(false);
    TempBar->setFixedSize(20,102);
    TempLevelChanged(15);

    HourLabel = new QLabel();
    QString HourLabelStyle = "QLabel {"
            "background : rgba(255,255,255,0);"
                  "color: white;"
            "font: bold 14px \"Times New Roman\";"
                  "}";
     HourLabel->setStyleSheet(HourLabelStyle);
    DateLabel->setStyleSheet(HourLabelStyle);
    HourLabel->setFixedHeight(14);
     DateLabel->setFixedHeight(14);
    HourLabel->setText("00:00");
    DateLabel->setText("00 / 00 / 00");
     RightPaneLayout->setSpacing(1);
    QVBoxLayout *timeLayout = new QVBoxLayout;

    timeLayout->addWidget(HourLabel,0,Qt::AlignAbsolute);
    timeLayout->addWidget(DateLabel,0,Qt::AlignAbsolute);
   timeLayout->setSpacing(0);
    PrevNextLayout= new QHBoxLayout;
    PrevNextLayout->setSpacing(0);
    PrevNextLayout->setMargin(0);


    MainCenterPanelLayout->setSpacing(0);
    MainCenterPanelLayout->setMargin(0);

    PrevNextLayout->addWidget(ButtonKeyboard);
    PrevNextLayout->addWidget(ButtonCalendar);
    PrevNextLayout->addWidget(ButtonRun);
    PrevNextLayout->addWidget(ButtonMusic);


    MainCenterPanelLayout->addWidget(slidingStacked);
    MainCenterPanelLayout->addLayout(PrevNextLayout);

    QVBoxLayout *tempLayout =new QVBoxLayout;

    QLabel *tempLabel =new QLabel;

    tempLabel->setPixmap(QPixmap(":/images/temp.png"));
    QLabel *fuelLabel =new QLabel;

    fuelLabel->setPixmap(QPixmap(":/images/fuel.png"));


    tempLayout->addWidget(TempBar,0,Qt::AlignAbsolute);
    tempLayout->addWidget(tempLabel);
    QVBoxLayout *fuelLayout =new QVBoxLayout;
    fuelLayout->addWidget(FuelBar,0,Qt::AlignAbsolute);
    fuelLayout->addWidget(fuelLabel);
    QHBoxLayout *BarsLayout=new QHBoxLayout;
    BarsLayout->addSpacing(10);
    BarsLayout->addLayout(tempLayout);
    BarsLayout->addLayout(fuelLayout);
    BarsLayout->addSpacing(10);


    mediaLayout= new QGridLayout;
    mediaLayout->setSpacing(0);
    mediaLayout->addWidget(playButton,0,0);
    mediaLayout->addWidget(pauseButton,0,1);
    mediaLayout->addWidget(backwardButton,1,0);
    mediaLayout->addWidget(forwardButton,1,1);
    // tempwidget->addAction(playButton);
    //tempwidget->addAction(pauseButton);
  //tempwidget->setIconSize(QSize(40,40));
//tempwidget->setContentsMargins(0, 0, 0, 0);
 //tempwidget->setStyleSheet("QToolBar{margin:0px; spacing: 0px;}");
    //tempwidget->setFixedWidth(100);
    //tempwidget->setMovable(false);
    QLabel *checkEngine =new QLabel;

    checkEngine->setPixmap(QPixmap(":/images/checkOff.png"));

    RightPanelWidget->setMinimumWidth(100);
    RightPanelWidget->setMaximumWidth(100);
    RightPaneLayout->addLayout(timeLayout);
    RightPaneLayout->addLayout(BarsLayout);
   // RightPaneLayout->addSpacing(30);
    RightPaneLayout->addWidget(checkEngine);
    RightPaneLayout->addLayout(mediaLayout);



    mainLayout->addWidget(MainCenterPanelWidget);

    mainLayout->addWidget(RightPanelWidget);

    RightPanelWidget->setStyleSheet("background-color: black");
   // RightPanelWidget->setObjectName(QObject::tr(rightPanelBackground));
    //RightPanelWidget->setStyleSheet(" background-image: url(:/images/background.png)  ");




    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    MainCenterPanelLayout->setSpacing(0);
    MainCenterPanelLayout->setMargin(0);
    RightPaneLayout->setMargin(0);


    this->setCentralWidget(centralWidget);
}

////////////////////////CREATE SUBSLIDING WIDGETS////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createSubSlidingWidgets() {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SLIDEWIDGET1/////TECLADO////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ObdTitleLabel = new QLabel();
    ObdTitleLabel->setText("INSERT YOUR OBD II CODE");
    ObdTitleLabel->setStyleSheet("color: white;font: bold 14px;");

    QPixmap PixmapSend(":/images/Send.png");
    QIcon SendIcon(PixmapSend);
    send =new QPushButton();
    send->setFixedSize(40,25);
    send->setIcon(SendIcon);
    send->setIconSize(PixmapSend.rect().size());
    connect(send, SIGNAL(clicked()),this, SLOT(SendCommandSlot()));
    send->setStyleSheet("border-radius: 5px;");

    QPixmap PixmapClear(":/images/Clear.png");
    QIcon ClearIcon(PixmapClear);
    clear =new QPushButton();
    clear->setFixedSize(40,25);
    clear->setIcon(ClearIcon);
    clear->setIconSize(PixmapSend.rect().size());
    connect(clear, SIGNAL(clicked()),this, SLOT(ClearCommandSlot()));
    clear->setStyleSheet("border-radius: 5px;");

    code =new QLabel();
    code->setStyleSheet("QLabel { font: bold 14px; color : white; }");

    mykeyboard = new widgetKeyBoard(false);
    mykeyboard->setZoomFacility(true);
    mykeyboard->enableSwitchingEcho(false); // enable possibility to change echo through keyboard
    mykeyboard->createKeyboard(); // only create keyboard
    mykeyboard->show(this);

    LineEdit1=new exampleMyFocus(mykeyboard);
    LineEdit1->setFixedSize(260, 25);
    QObject::connect(LineEdit1,SIGNAL(returnPressed()),this,SLOT(SendCommandSlot()));

    QHBoxLayout *horizontaltmp=new QHBoxLayout();
    horizontaltmp->addWidget(LineEdit1);
    horizontaltmp->addWidget(send);
    horizontaltmp->addWidget(clear);
    horizontaltmp->setSpacing(0);

    slideWidget1layout=new QVBoxLayout();
    slideWidget1=new QWidget();
    slideWidget1->setLayout(slideWidget1layout);
    slideWidget1->setStyleSheet("background-color: rgb(69,76,76);");
    slideWidget1->setAutoFillBackground(true);

    slideWidget1layout->addWidget(ObdTitleLabel,0,Qt::AlignCenter);
    slideWidget1layout->insertLayout(1,horizontaltmp);
    slideWidget1layout->addWidget(code,0,Qt::AlignAbsolute);
    slideWidget1layout->addWidget(mykeyboard,0,Qt::AlignAbsolute);


    slideWidget1layout->setSpacing(0);
    slideWidget1layout->setMargin(0);

    //slideWidget1->setStyleSheet("background-image: /home/carto/Escritorio/button1.png");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SLIDEWIDGET2////RUN/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SpeedTitle=new QLabel();
    SpeedTitle->setText("km/h");
    SpeedTitle->setStyleSheet("color: white;font: 24px");

    RpmTitle=new QLabel();
    RpmTitle->setText("RPM");
    RpmTitle->setStyleSheet("color: white;font: 24px");

    lcd_vel= new QLCDNumber();
    lcd_vel->setFixedWidth(300);
    lcd_vel->setStyleSheet("color: white;");

    lcd_Rpm= new QLCDNumber();
    lcd_Rpm->setFixedWidth(300);
    lcd_Rpm->setStyleSheet("color: white;");

    QGridLayout *slideWidget2layout=new QGridLayout();
    slideWidget2=new QWidget();
    slideWidget2->setLayout(slideWidget2layout);
    slideWidget2->setStyleSheet("background-color: black;");

    slideWidget2layout->addWidget(SpeedTitle,0,1,Qt::AlignBottom);
    slideWidget2layout->addWidget(RpmTitle,1,1,Qt::AlignBottom);
    slideWidget2layout->addWidget(lcd_vel,0,0);
    slideWidget2layout->addWidget(lcd_Rpm,1,0);
    slideWidget2layout->setMargin(0);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SLIDEWIDGET3//////PHONON///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout *slideWidget3layout=new QVBoxLayout();
    slideWidget3=new QWidget();
    slideWidget3->setLayout(slideWidget3layout);
    slideWidget3layout->addWidget(widgetPhonon);
    slideWidget3layout->setMargin(0);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SLIDEWIDGET4//////CALENDAR///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QPushButton *configTime =new QPushButton;
    configTime->setFixedSize(370,30);
    connect(configTime, SIGNAL(clicked()),this, SLOT(SetIndexConfigTime()));
    QPushButton *fileTestBtn = new QPushButton;
    fileTestBtn->setText("File Test");
    QPushButton *fileTestBtn2 = new QPushButton;
    fileTestBtn2->setText("File Open");
    carEventFilePath = QString("/home/felipe/Desktop/CarEventTestFile");
    CarEvent event1= CarEvent(20,12,2012,20);
    carEventList1.append(event1);
    connect(fileTestBtn,SIGNAL(clicked()),this,SLOT(fileSaveTest()));
    connect(fileTestBtn2,SIGNAL(clicked()),this,SLOT(fileOpenTest()));
    QVBoxLayout *slideWidget4layout=new QVBoxLayout();
    slideWidget4=new QWidget();
    slideWidget4->setLayout(slideWidget4layout);
    slideWidget4->setStyleSheet("background-color: rgb(69,76,76);");

    slideWidget4layout->addWidget(configTime,0,Qt::AlignAbsolute);
    slideWidget4layout->addWidget(fileTestBtn,0,Qt::AlignAbsolute);
    slideWidget4layout->addWidget(fileTestBtn2,0,Qt::AlignAbsolute);
    slideWidget4layout->setMargin(0);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SLIDEWIDGET5//////CONFIG TIME///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QString comboStyle=("QSpinBox {border-style: outset;border-width: 2px;border-color:rgb(49,56,56); border-radius: 10px; color: rgb(69,76,76); background-color: white;font:40px; selection-background-color: white;selection-color:rgb(69,76,76);spinbox-click-autorepeat-rate: 10000000 }"
                        "QSpinBox::up-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid grey; width: 0px; height: 0px; }"
                        "QSpinBox::up-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid black; width: 0px; height: 0px; }"
                        "QSpinBox::up-button { width: 40px; height: 30px;  background: rgba(100,100,100,0); }"
                        "QSpinBox::up-button:hover { width: 40px; height: 30px;  background: rgba(100,100,100,0) ;}"
                        "QSpinBox::down-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid grey; width: 0px; height: 0px; }"
                        "QSpinBox::down-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid black; width: 0px; height: 0px; }"
                        "QSpinBox::down-button { width: 40px; height: 30px; background: rgba(100,100,100,0);}"
                        "QSpinBox::down-button:hover { width: 40px; height: 30px; background: rgba(100,100,100,0);}");
    QString comboStyle1=("QDateEdit {border-style: outset;border-width: 2px;border-color:rgb(49,56,56); border-radius: 10px; color: rgb(69,76,76); background-color: white;font:40px; selection-background-color: white;selection-color:rgb(69,76,76);spinbox-click-autorepeat-rate: 10000000 }"
                        "QDateEdit::up-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid grey; width: 0px; height: 0px; }"
                        "QDateEdit::up-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid black; width: 0px; height: 0px; }"
                        "QDateEdit::up-button { width: 40px; height: 30px;  background: rgba(100,100,100,0); }"
                        "QDateEdit::up-button:hover { width: 40px; height: 30px;  background: rgba(100,100,100,0) ;}"
                        "QDateEdit::down-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid grey; width: 0px; height: 0px; }"
                        "QDateEdit::down-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid black; width: 0px; height: 0px; }"
                        "QDateEdit::down-button { width: 40px; height: 30px; background: rgba(100,100,100,0);}"
                        "QDateEdit::down-button:hover { width: 40px; height: 30px; background: rgba(100,100,100,0);}");

    QString comboStyle2=("QTimeEdit {border-style: outset;border-width: 2px;border-color:rgb(49,56,56); border-radius: 10px; color: rgb(69,76,76); background-color: white;font:40px; selection-background-color: white;selection-color:rgb(69,76,76);spinbox-click-autorepeat-rate: 10000000 }"
                        "QTimeEdit::up-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid grey; width: 0px; height: 0px; }"
                        "QTimeEdit::up-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-bottom: 17px solid black; width: 0px; height: 0px; }"
                        "QTimeEdit::up-button { width: 40px; height: 30px;  background: rgba(100,100,100,0); }"
                        "QTimeEdit::up-button:hover { width: 40px; height: 30px;  background: rgba(100,100,100,0) ;}"
                        "QTimeEdit::down-arrow { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid grey; width: 0px; height: 0px; }"
                        "QTimeEdit::down-arrow:hover { border-left: 17px solid none;"
                        "border-right: 17px solid none; border-top: 17px solid black; width: 0px; height: 0px; }"
                        "QTimeEdit::down-button { width: 40px; height: 30px; background: rgba(100,100,100,0);}"
                        "QTimeEdit::down-button:hover { width: 40px; height: 30px; background: rgba(100,100,100,0);}");


    QLabel *settingsTittle =new QLabel();
    settingsTittle->setFixedHeight(20);
    settingsTittle->setText("SETTINGS");
    settingsTittle->setStyleSheet("color: white;font: bold 22px;");

    QLabel *setDateTittle =new QLabel();
    setDateTittle->setFixedHeight(20);
    setDateTittle->setText("SET DATE");
    setDateTittle->setStyleSheet("color: white;font: bold 16px;");

    QLabel *setTimeTittle =new QLabel();
    setTimeTittle->setFixedHeight(20);
    setTimeTittle->setText("SET TIME");
    setTimeTittle->setStyleSheet("color: white;font: bold 16px;");

    QLabel *setPlateTittle =new QLabel();
    setPlateTittle->setFixedHeight(20);
    setPlateTittle->setText("LAST PLATE'S DIGIT");
    setPlateTittle->setStyleSheet("color: white;font: bold 16px;");

    dateSpin =new QDateEdit(QDate::currentDate());
    dateSpin->setDateRange(QDate(2012,1,1),QDate(2020,12,31));
    dateSpin->setStyleSheet(comboStyle1);
    dateSpin->setFixedHeight(60);

    timeSpin =new QTimeEdit(QTime::currentTime());
    timeSpin->setStyleSheet(comboStyle2);
    timeSpin->setDisplayFormat("hh:mm AP");
    timeSpin->setFixedSize(280,60);

    plateSpin= new QSpinBox;
    plateSpin->setRange(0,9);
    plateSpin->setFixedSize(75,60);
    plateSpin->setStyleSheet(comboStyle);


    QPixmap PixmapOkConfig(":/images/okConfig.png");
    QIcon okConfigIcon(PixmapOkConfig);
    QPushButton *okConfigTime =new QPushButton;
    okConfigTime->setFixedSize(120,60);
    okConfigTime->setIcon(okConfigIcon);
    okConfigTime->setIconSize(PixmapOkConfig.rect().size());
    connect(okConfigTime, SIGNAL(clicked()),this, SLOT(SetDateSlot()));


    QHBoxLayout *dateLayout = new QHBoxLayout;
    dateLayout->addWidget(setDateTittle);
    dateLayout->addWidget(dateSpin);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(setTimeTittle);
    timeLayout->addWidget(timeSpin);

    QHBoxLayout *plateLayout = new QHBoxLayout;
    plateLayout->addWidget(setPlateTittle);
    plateLayout->addWidget(plateSpin);
    plateLayout->addWidget(okConfigTime);

    QVBoxLayout *slideWidget5layout=new QVBoxLayout();
    slideWidget5=new QWidget();
    slideWidget5->setLayout(slideWidget5layout);
    slideWidget5->setStyleSheet("background-color: rgb(69,76,76);");

    slideWidget5layout->addWidget(settingsTittle,0,Qt::AlignAbsolute);
 // slideWidget5layout->addLayout(horConfigDateLayout);
    slideWidget5layout->addLayout(dateLayout);
    slideWidget5layout->addLayout(timeLayout);
    slideWidget5layout->addLayout(plateLayout);


}
////////////////////////CREATE SLIDING STACKED////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createSlidingStackedWidget() {
    slidingStacked= new SlidingStackedWidget(this);
    slidingStacked->addWidget(slideWidget2);
    slidingStacked->addWidget(slideWidget4);
    slidingStacked->addWidget(slideWidget1);
    slidingStacked->addWidget(slideWidget3);
    slidingStacked->addWidget(slideWidget5);
    slidingStacked->setSpeed(animTime);

}
////////////////////////CREATE CONNECTIONS////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createConnections() {

    connect(timer1,SIGNAL(timeout()),this,SLOT(OBDRunTime()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(GetFuelData()));
    connect(timer3,SIGNAL(timeout()),this,SLOT(GetDate()));
}

void MainWindow::SetIndexConfigPlaca(){

slideWidget1layout->removeWidget(mykeyboard);
slideWidget6layout->addWidget(mykeyboard);
slidingStacked->setCurrentIndex(5);
this->mykeyboard->focusThis(placaEdit);
}
void MainWindow::SetIndexConfigTime(){
slidingStacked->setCurrentIndex(4);
}
void MainWindow::SetIndexConfigHour(){
slidingStacked->setCurrentIndex(6);
}
void MainWindow::SetIndexKeyBoard(){
    this->mykeyboard->focusThis(LineEdit1);
    ButtonsOff();
    ButtonKeyboard->setIcon(ButtonKeyboardIconOn);
    slidingStacked->setCurrentIndex(2);
}

void MainWindow::SetIndexRun(){
    ButtonsOff();
    ButtonRun->setIcon(ButtonRunIconOn);
    slidingStacked->setCurrentIndex(0);
}

void MainWindow::SetIndexCalendar(){
    ButtonsOff();
    ButtonCalendar->setIcon(ButtonCalendarIconOn);
    slidingStacked->setCurrentIndex(1);
}
void MainWindow::SetIndexMusic(){
    ButtonsOff();
    ButtonMusic->setIcon(ButtonMusicIconOn);
    slidingStacked->setCurrentIndex(3);
}
void MainWindow::ButtonsOff(){
ButtonMusic->setIcon(ButtonMusicIconOff);
ButtonCalendar->setIcon(ButtonCalendarIconOff);
ButtonRun->setIcon(ButtonRunIconOff);
ButtonKeyboard->setIcon(ButtonKeyboardIconOff);
}

////////////////////////CLEAR SLOT////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::ClearCommandSlot(){

LineEdit1->clear();
}

////////////////////////SEND SLOT////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::SendCommandSlot()
{
 timer2->blockSignals(true);
 timer3->blockSignals(true);

 usleep(200000);
tcflush(ttyfd,TCIOFLUSH);
strcpy(CodeSend,(char *)LineEdit1->text().toAscii().constData());
LineEdit1->clear();
int count1=0;
while(CodeSend[count1]!=0)
{
 count1++;
}
CodeSend[count1]='\r';
for(int i=0;i<=count1;i++)
{if(CodeSend[i]=='\r')
    qDebug("%d",CodeSend[i]);
    qDebug("%c",CodeSend[i]);
}
write (ttyfd,CodeSend , count1+1);
read  (ttyfd, CodeAns, sizeof(RpmAnswer));
strcpy(CodeSend,(char *)LineEdit1->text().toAscii().constData());
//close(ttyfd);
code->setText(CodeAns);
tcflush(ttyfd,TCIOFLUSH);
usleep(200000);
timer2->blockSignals(false);
timer3->blockSignals(false);
}
////////////////////////TIMER1////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::OBDRunTime()
{

        slide = slidingStacked->slideInNum();
        //qDebug("slide: %d",slide);
       if((slide==1)){
           qDebug("slide1");
       timer2->blockSignals(true);
       timer3->blockSignals(true);

            qDebug("OBD");
        switch(count)
        {
        case 0:
        tcflush(ttyfd,TCIOFLUSH);
                write (ttyfd, CommandRpm, 6);
                read  (ttyfd, RpmAnswer, sizeof(RpmAnswer));
                while(RpmAnswer[i]!=0)
                {
                 qRpmAnswer[i]=RpmAnswer[i];
                 i++;
                }
                sscanf(qRpmAnswer,"%*s %*s %X %X",&pm_a, &pm_b);
                pm=(((pm_a*256)+pm_b)/4);
                if((pm>0)&&(pm<8000))
                lcd_Rpm->display(pm);
                for(int l=0;l<64;l++){
                     qRpmAnswer[l]=0;
                     RpmAnswer[l]=0;}
                count=1;
                tcflush(ttyfd,TCIOFLUSH);
                break;
            case 1:
            tcflush(ttyfd,TCIOFLUSH);
                write (ttyfd, Commandspeed, 6);
                read  (ttyfd, SpeedAnswer, sizeof(SpeedAnswer));
                while(SpeedAnswer[i]!=0)
                {
                 qSpeedAnswer[i]=SpeedAnswer[i];
                 i++;
                }
                sscanf(qSpeedAnswer,"%*s %*s %X",&vel);

                if((vel>=0)&&(vel<180))

                lcd_vel->display(vel);

                for(int l=0;l<64;l++){
                     qSpeedAnswer[l]=0;
                     SpeedAnswer[l]=0;
                 }
                count=0;
                tcflush(ttyfd,TCIOFLUSH);
            break;
            default: break;
        }
       i=0;


}
       timer2->blockSignals(false);
       timer3->blockSignals(false);
}

////////////////////////TIMER2////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////FUEL///////////////////////////////////////////////////////////////////////////////////
void MainWindow::GetFuelData()
{
    int i1=0;
    int i2=0;
    timer1->blockSignals(true);
    timer3->blockSignals(true);
      usleep(200000);
    qDebug("Fuel");
    tcflush(ttyfd,TCIOFLUSH);
            write (ttyfd, CommandFuel, 6);
            read  (ttyfd, FuelAnswer, sizeof(FuelAnswer));
            while(FuelAnswer[i1]!=0)
            {
             qFuelAnswer[i1]=FuelAnswer[i1];
             i1++;
            }
             qDebug(qFuelAnswer);
            sscanf(qFuelAnswer,"%*s %*s %X",&FuelValue);
            FuelValue=(FuelValue*100)/255;
            if((FuelValue>=0)&&(FuelValue<100))
            FuelLevelChanged(FuelValue);
            for(int l=0;l<64;l++){
                 qFuelAnswer[l]=0;
                 FuelAnswer[l]=0;}
            qDebug("Fuel: %d",FuelValue);
               usleep(200000);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////TEMPERATURE/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            tcflush(ttyfd,TCIOFLUSH);
                    write (ttyfd, CommandTemp, 6);
                    read  (ttyfd, TempAnswer, sizeof(TempAnswer));
                    while(TempAnswer[i2]!=0)
                    {
                     qTempAnswer[i2]=TempAnswer[i2];
                     i2++;
                    }
                    sscanf(qTempAnswer,"%*s %*s %X",&TempValue);
                    //TempValue=TempValue;
                    if((TempValue>=15)&&(TempValue<140))
                    TempLevelChanged(TempValue);
                    for(int l=0;l<64;l++){
                         qTempAnswer[l]=0;
                         TempAnswer[l]=0;}
                     qDebug("Temp: %d",TempValue);
                    usleep(200000);
            timer1->blockSignals(false);
            timer3->blockSignals(false);
 }

////////////////////////TIMER3////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::GetDate()
{
    timer1->blockSignals(true);
    timer2->blockSignals(true);
      usleep(200000);
    qDebug("GetDate");
    char HourText[100];
    char DateText[100];

  tcflush(ttyfd,TCIOFLUSH);

    write(ttyfd,CommandGetTime,sizeof(CommandGetTime));
    qDebug("writegetdate");
    stmReadRTC(ttyfd,comObjectPtr);
    qDebug("readgetdate");
    // qDebug("%s\n",asctime(comObjectPtr));

    if((comObjectPtr->tm_hour<10)&&(comObjectPtr->tm_min<10))
    sprintf(HourText,"0%d:0%d",comObjectPtr->tm_hour,comObjectPtr->tm_min);
    else if(comObjectPtr->tm_min<10)
    sprintf(HourText,"%d:0%d",comObjectPtr->tm_hour,comObjectPtr->tm_min);
    else if(comObjectPtr->tm_hour<10)
    sprintf(HourText,"0%d:%d",comObjectPtr->tm_hour,comObjectPtr->tm_min);
    else
    sprintf(HourText,"%d:%d",comObjectPtr->tm_hour,comObjectPtr->tm_min);

    sprintf(DateText,"%d / %d / %d",comObjectPtr->tm_mday, comObjectPtr->tm_mon,comObjectPtr->tm_year+1900);

    qDebug(DateText);
    qDebug(HourText);
    HourLabel->setText(HourText);
    DateLabel->setText(DateText);
     usleep(200000);
    tcflush(ttyfd,TCIOFLUSH);
    timer1->blockSignals(false);
    timer2->blockSignals(false);
}
////////////////////////SET DATE////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::SetDateSlot()
{

   timer2->blockSignals(true);
   timer3->blockSignals(true);
   qDebug("%d",dateSpin->date().dayOfWeek());
   usleep(200000);
    tcflush(ttyfd,TCIOFLUSH);
    qDebug("set date");
    write(ttyfd,CommandSetTime,sizeof(CommandSetTime));

         comObjectSetDate->tm_sec=0;
         comObjectSetDate->tm_min=timeSpin->time().minute();
         comObjectSetDate->tm_hour=timeSpin->time().hour();
         comObjectSetDate->tm_mday=dateSpin->date().day();
         comObjectSetDate->tm_mon=dateSpin->date().month();
         comObjectSetDate->tm_year=dateSpin->date().year()-1900;
         comObjectSetDate->tm_isdst=0;
         comObjectSetDate->tm_wday=dateSpin->date().dayOfWeek();
         //usleep(1000000);

         stmWriteRTC(ttyfd,comObjectSetDate);
         usleep(200000);
         GetDate();
         SetIndexCalendar();
timer3->blockSignals(false);
timer2->blockSignals(false);
}

void MainWindow::stmWriteRTC (int ttyPort,ComInfo* comObjectPtr){
    unsigned char outBuffer[sizeof(*comObjectPtr)];
    //qDebug("hour sent: %d",comObjectPtr->tm_hour);
    //printf("Write to Stm started ...\n");
    memcpy(outBuffer,comObjectPtr,sizeof(*comObjectPtr));
    write (ttyPort, outBuffer, sizeof(*comObjectPtr));
    //printf("Wrote %c to Stm\n",comObjectPtr->command);
    return;
}

void MainWindow::stmReadRTC (int ttyPort,ComInfo* comObjectPtr){
    unsigned char inBuffer[sizeof(*comObjectPtr)];
    //printf("Read from Stm started ...\n");
    read  (ttyPort, inBuffer, sizeof(*comObjectPtr));
    //printf("Read command OK...\n");
    //cleanComObject(comObjectPtr);
    memcpy(comObjectPtr,inBuffer,sizeof(*comObjectPtr));
    //for(int i=0;i<sizeof(*comObjectPtr);i++)
    //inBuffer[i]= 0;
    //printf("Read from Stm correct\n");
    return;
}

void MainWindow::TempLevelChanged(int mValue)
{
    TempBar->setValue(mValue);

    QString myStyleSheet = "QProgressBar:vertical {"
                  "border: 1px solid gray;"
                  "border-radius: 3px;"
                  "background: black;"
                  "padding: 1px;"
                  "}"
            " QProgressBar::chunk {"\
                           "     background-color: ";

    if(90 <= mValue)
    {
        myStyleSheet.append("rgb(255,0,0);");
    }
    else
    {
        myStyleSheet.append("white;");
    }
    myStyleSheet.append("     height: 5px;"\
                        "     margin: 1px;"\
                        " }");
    TempBar->setStyleSheet(myStyleSheet);
}

void MainWindow::FuelLevelChanged(int nValue)
{
    FuelBar->setValue(nValue);

    QString myStyleSheet = "QProgressBar:vertical {"
                  "border: 1px solid gray;"
                  "border-radius: 3px;"
                  "background: black;"
                  "padding: 1px;"
                  "}"
            " QProgressBar::chunk {"\
                           "     background-color: ";

    if(20 >= nValue)
    {
        myStyleSheet.append("rgb(255,0,0);");
    }
    else if((50 >= nValue)&&(nValue>20))
    {
        myStyleSheet.append("rgb(255,140,0);");
    }
    else
    {
        myStyleSheet.append("white;");
    }
    myStyleSheet.append("     height: 5px;"\
                        "     margin: 1px;"\
                        " }");
    FuelBar->setStyleSheet(myStyleSheet);
}
//////////////////////////////PHONON///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////PHONON//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::addFiles()
 {
    // QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

        QStringList files;
         //QDir dir("/mnt/Music");
         QDir dir("/home/carto/Escritorio/");
         QStringList filters;
         filters<<"*.mp3";
         foreach (QString file, dir.entryList(filters, QDir::Files)){
             files<<QFileInfo(dir, file).absoluteFilePath();
            // qDebug()<<QFileInfo(dir, file).absoluteFilePath();
            }
         foreach (QString file, files){
             qDebug()<<file;
            }
         if (files.isEmpty())
             return;

         int index = sources.size();
         foreach (QString string, files) {
             Phonon::MediaSource source(string);
             sources.append(source);
         }
         if (!sources.isEmpty())
             metaInformationResolver->setCurrentSource(sources.at(index));
        outOfBounds=false;
 }

void MainWindow::about()
{
    QMessageBox::information(this, tr("About Music Player"),
        tr("The Music Player example shows how to use Phonon - the multimedia"
           " framework that comes with Qt - to create a simple music player."));
}

void MainWindow::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    int index = sources.indexOf(mediaObject->currentSource());
    switch (newState) {
        case Phonon::ErrorState:
            if (mediaObject->errorType() == Phonon::FatalError) {
                QMessageBox::warning(this, tr("Fatal Error"),
                mediaObject->errorString());
            } else {
                QMessageBox::warning(this, tr("Error"),
                mediaObject->errorString());
            }
            break;
        case Phonon::PlayingState:
                playAction->setEnabled(false);
                playButton->setEnabled(false);
                pauseButton->setEnabled(true);
                pauseAction->setEnabled(true);
                stopAction->setEnabled(true);
                if(index==0){
                 previousAction->setEnabled(false);
                backwardButton->setEnabled(false);
                }
                else{
                previousAction->setEnabled(true);
                backwardButton->setEnabled(true);
                }
                if(sources.size()<=index+1){
                    forwardAction->setEnabled(false);
                    forwardButton->setEnabled(false);
                }
                else{
                    forwardAction->setEnabled(true);
                    forwardButton->setEnabled(true);
                }
                break;
        case Phonon::StoppedState:
                stopAction->setEnabled(false);
                playAction->setEnabled(true);
                playButton->setEnabled(true);
                pauseAction->setEnabled(false);
                pauseButton->setEnabled(false);
                forwardAction->setEnabled(false);
                forwardButton->setEnabled(false);
                previousAction->setEnabled(false);
                backwardButton->setEnabled(false);
                timeLcd->setText("00:00");
                break;
        case Phonon::PausedState:
                pauseAction->setEnabled(false);
                pauseButton->setEnabled(false);
                stopAction->setEnabled(true);
                playAction->setEnabled(true);
              playButton->setEnabled(true);
              forwardAction->setEnabled(false);
              forwardButton->setEnabled(false);
              previousAction->setEnabled(false);
              backwardButton->setEnabled(false);
                break;
        case Phonon::BufferingState:
                break;
        default:
            ;
    }
}

void MainWindow::tick(qint64 time)
{
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

    timeLcd->setText(displayTime.toString("mm:ss"));


}

void MainWindow::tableClicked(int row, int /* column */)
{
    bool wasPlaying = mediaObject->state() == Phonon::PlayingState;

    mediaObject->stop();
    mediaObject->clearQueue();

    if (row >= sources.size())
        return;

    mediaObject->setCurrentSource(sources[row]);

    if (wasPlaying)
        mediaObject->play();
    else
        mediaObject->stop();
}

void MainWindow::sourceChanged(const Phonon::MediaSource &source)
{
    musicTable->selectRow(sources.indexOf(source));
    timeLcd->setText("00:00");
}

void MainWindow::metaStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    if (newState == Phonon::ErrorState) {
        QMessageBox::warning(this, tr("Error opening files"),
            metaInformationResolver->errorString());
        while (!sources.isEmpty() &&
               !(sources.takeLast() == metaInformationResolver->currentSource())) {}  /* loop */;
        return;
    }

    if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
        return;

    if (metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
            return;

    QMap<QString, QString> metaData = metaInformationResolver->metaData();

    QString title = metaData.value("TITLE");
    if (title == "")
        title = metaInformationResolver->currentSource().fileName();

    QTableWidgetItem *titleItem = new QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *artistItem = new QTableWidgetItem(metaData.value("ARTIST"));
    artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
//    QTableWidgetItem *albumItem = new QTableWidgetItem(metaData.value("ALBUM"));
//    albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
//    QTableWidgetItem *yearItem = new QTableWidgetItem(metaData.value("DATE"));
//    yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);

    int currentRow = musicTable->rowCount();
    musicTable->insertRow(currentRow);
    musicTable->setItem(currentRow, 0, titleItem);
    musicTable->setItem(currentRow, 1, artistItem);
    //musicTable->setItem(currentRow, 2, albumItem);
    //musicTable->setItem(currentRow, 3, yearItem);

    if (musicTable->selectedItems().isEmpty()) {
        musicTable->selectRow(0);
        mediaObject->setCurrentSource(metaInformationResolver->currentSource());
    }

    Phonon::MediaSource source = metaInformationResolver->currentSource();
    int index = sources.indexOf(metaInformationResolver->currentSource()) + 1;
    if (sources.size() > index) {
        metaInformationResolver->setCurrentSource(sources.at(index));
    }
    else {
        //musicTable->resizeColumnsToContents();
        if (musicTable->columnWidth(0) > 300)
            musicTable->setColumnWidth(0, 300);
    }
}

void MainWindow::aboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource()) + 1;
    if (sources.size() > index) {
        mediaObject->enqueue(sources.at(index));
    }
}

void MainWindow::setupActions()
{
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setDisabled(true);
    playButton = new QPushButton;
    playButton->addAction(playAction);
    playButton->setDisabled(true);
    playButton->setFixedHeight(35);
    QPixmap playbutton_enabled_icon(":/images/PlayButtonEnabled.png");
    QPixmap playbutton_disabled_icon(":/images/PlayButtonDisabled.png");
    QIcon playIcon(playbutton_enabled_icon);
    playIcon.addPixmap(playbutton_disabled_icon,QIcon::Disabled);
    playButton->setIcon(playIcon);
    playButton->setIconSize(QSize(50,35));



    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(true);
    pauseButton = new QPushButton;
    pauseButton->addAction(pauseAction);
    pauseButton->setDisabled(true);
    pauseButton->setFixedHeight(35);
    QPixmap pausebutton_enabled_icon(":/images/PauseButtonEnabled.png");
    QPixmap pausebutton_disabled_icon(":/images/PauseButtonDisabled.png");
    QIcon pauseIcon(pausebutton_enabled_icon);
    pauseIcon.addPixmap(pausebutton_disabled_icon,QIcon::Disabled);
    pauseButton->setIcon(pauseIcon);
    pauseButton->setIconSize(QSize(50,35));



    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    stopAction->setShortcut(tr("Ctrl+S"));
    stopAction->setDisabled(true);

    forwardAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("forward"), this);
    forwardAction->setShortcut(tr("Ctrl+N"));
    forwardAction->setEnabled(true);
    forwardButton = new QPushButton;
    forwardButton->addAction(forwardAction);
    forwardButton->setEnabled(false);
    forwardButton->setFixedHeight(35);
    QPixmap forwardbutton_enabled_icon(":/images/forwardButtonEnabled.png");
    QPixmap forwardbutton_disabled_icon(":/images/forwardButtonDisabled.png");
    QIcon forwardIcon(forwardbutton_enabled_icon);
    forwardIcon.addPixmap(forwardbutton_disabled_icon,QIcon::Disabled);
    forwardButton->setIcon(forwardIcon);
    forwardButton->setIconSize(QSize(50,35));

    previousAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous"), this);
    previousAction->setShortcut(tr("Ctrl+R"));
    previousAction->setEnabled(false);
    backwardButton = new QPushButton;
    backwardButton->addAction(previousAction);
    backwardButton->setEnabled(false);
    backwardButton->setFixedHeight(35);
    QPixmap backwardbutton_enabled_icon(":/images/backwardButtonEnabled.png");
    QPixmap backwardbutton_disabled_icon(":/images/backwardButtonDisabled.png");
    QIcon backwardIcon(backwardbutton_enabled_icon);
    backwardIcon.addPixmap(backwardbutton_disabled_icon,QIcon::Disabled);
    backwardButton->setIcon(backwardIcon);
    backwardButton->setIconSize(QSize(50,35));

    addFilesAction = new QAction(tr("Add &Files"), this);
    addFilesAction->setShortcut(tr("Ctrl+F"));
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setShortcut(tr("Ctrl+Q"));

    connect(playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
    connect(playButton, SIGNAL(clicked()), playAction,SIGNAL(triggered()));
    connect(pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
    connect(pauseButton, SIGNAL(clicked()), pauseAction, SIGNAL(triggered()));
    connect(stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));
    connect(forwardAction, SIGNAL(triggered()), this, SLOT(forward()));
    connect(forwardButton, SIGNAL(clicked()), forwardAction,SIGNAL(triggered()));
    connect(previousAction, SIGNAL(triggered()), this, SLOT(backward()));
    connect(backwardButton, SIGNAL(clicked()), previousAction,SIGNAL(triggered()));
    connect(addFilesAction, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
void MainWindow::forward(){

        int index = sources.indexOf(mediaObject->currentSource()) + 1;
        mediaObject->stop();
        if (sources.size() > index) {
            mediaObject->setCurrentSource(sources.at(index));
        }
        if(sources.size()<=index+1)
            //forwardAction->setEnabled(false);

        stateChanged(Phonon::PlayingState,Phonon::PlayingState);
         mediaObject->play();
}
void MainWindow::backward(){


        int index = sources.indexOf(mediaObject->currentSource()) - 1;
        mediaObject->stop();
        if (index>=0) {
            mediaObject->setCurrentSource(sources.at(index));
        }
        if(index==0)
            //previousAction->setEnabled(false);

        stateChanged(Phonon::PlayingState,Phonon::PlayingState);
         mediaObject->play();
}
void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addFilesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
    aboutMenu->addAction(aboutQtAction);
}

void MainWindow::setupUi()
{
    QToolBar *bar = new QToolBar;
    //bar->setAttribute( Qt::WA_TranslucentBackground, false );
    //bar->setAutoFillBackground(true);
    //bar->setStyleSheet("background-color: transparent");
    bar->addAction(previousAction);
    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);
    bar->addAction(forwardAction);

    //seekSlider = new Phonon::SeekSlider(this);
    //seekSlider->setMediaObject(mediaObject);

    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    //volumeSlider->setStyleSheet("handle{color: blue}");

    QLabel *volumeLabel = new QLabel;
    volumeLabel->setPixmap(QPixmap("images/volume.png"));

    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);

    timeLcd = new QLabel;
    timeLcd->setStyleSheet("QLabel { background-color : rgb(76,76,76); color : white;font: bold 20px }");
   // timeLcd->setPalette(palette);

    QStringList headers;
   // headers << tr("Title") << tr("Artist") << tr("Album ") << tr("Year");
    headers << tr("Title") << tr("Artist");
    musicTable = new QTableWidget(0, 2);
    musicTable->setColumnWidth(0,241);
    musicTable->setColumnWidth(1,101);
    musicTable->setHorizontalHeaderLabels(headers);
    musicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    musicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //musicTable->setPalette(palette);
     musicTable->setStyleSheet("background-color: white;font: 14px");
    // musicTable->setStyleSheet("QHeaderView {background-color: blue}");
    connect(musicTable, SIGNAL(cellPressed(int,int)),
            this, SLOT(tableClicked(int,int)));

    seekerLayout = new QHBoxLayout;
    //seekerLayout->addWidget(seekSlider);
    //seekerLayout->addWidget(timeLcd);

    playbackLayout = new QHBoxLayout;
    playbackLayout->addWidget(volumeLabel);
    playbackLayout->addWidget(volumeSlider);
    playbackLayout->addWidget(bar);
    playbackLayout->addStretch();
    playbackLayout->addWidget(timeLcd);
    playbackwidget = new QWidget;
    playbackwidget->setLayout(playbackLayout);
    //playbackwidget->setStyleSheet("background-color: rgb(69,76,76);");
    playbackwidget->setStyleSheet("background-color: rgb(76,76,76);border-width: 0px;border-radius: 5px;border-color: rgb(69,76,76);");

    MainPhononLayout = new QVBoxLayout;
    MainPhononLayout->addWidget(musicTable);
    MainPhononLayout->addLayout(seekerLayout);
    MainPhononLayout->addWidget(playbackwidget);

    widgetPhonon = new QWidget;
    widgetPhonon->setStyleSheet("background-color: rgb(99,106,106);");
    widgetPhonon->setLayout(MainPhononLayout);


    //setCentralWidget(widgetPhonon);
    //setWindowTitle("Phonon Music Player");
}

////////////////////////FILE MANAGEMENT SLOTS////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::fileSaveTest(){
    QFile file(carEventFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QDataStream out(&file);
    out << carEventList1;
    file.close();
}

void MainWindow::fileOpenTest(){
    QFile file(carEventFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }
    QDataStream in(&file);
    in >> carEventList2;
    file.close();
    qDebug()<<carEventList2.at(0).daysToString(15,11,2013);
    qDebug()<<carEventList2.at(0).daysToNumber(15,11,2013);
    return;
}

bool MainWindow::checkPicoPlaca(){

}

