#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QtGui>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>
#include <QSlider>
#include <QCursor>
#include <QTimer>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <QList>
#include <QFileSystemModel>
#include "SlidingStackedWidget.h"
#include "widgetKeyBoard.h"
#include <QList>
#include "carevent.h"

class QAction;
class QTableWidget;
class QLCDNumber;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void SendCommandSlot();
    void forward();
    void backward();
    void ClearCommandSlot();
    void SetIndexKeyBoard();
    void SetIndexRun();
    void SetIndexMusic();
    void SetIndexCalendar();
    void SetIndexConfigTime();
    void SetIndexConfigPlaca();
    void SetIndexConfigHour();
    void SetDateSlot();
    void FuelLevelChanged(int nValue);
    void TempLevelChanged(int mValue);
    void OBDRunTime();
    void GetFuelData();
    void GetDate();
    void ButtonsOff();

protected:
    // void timerEvent(QTimerEvent*);
    void createGuiControlComponents();
    void createMainLayout();
    void createSubSlidingWidgets();
    void createConnections();
    void createSlidingStackedWidget();
    QPushButton *ButtonMusic;
    QPushButton *ButtonKeyboard;
    QPushButton *ButtonRun;
    QPushButton *ButtonCalendar;

    QPixmap PixmapMusicOn;
    QPixmap PixmapMusicOff;
    QIcon ButtonMusicIconOn;
    QIcon ButtonMusicIconOff;

    QPixmap PixmapRunOn;
    QPixmap PixmapRunOff;
    QIcon ButtonRunIconOn;
    QIcon ButtonRunIconOff;


    QPixmap PixmapKeyboardOn;
    QPixmap PixmapKeyboardOff;
    QIcon ButtonKeyboardIconOn;
    QIcon ButtonKeyboardIconOff;

    QPixmap PixmapCalendarOn;
    QPixmap PixmapCalendarOff;
    QIcon ButtonCalendarIconOn;
    QIcon ButtonCalendarIconOff;




    QProgressBar *FuelBar;
    QProgressBar *TempBar;

    QCheckBox *checkWrap;
    QCheckBox *checkVertical;
  //  QSlider *slideSpeed;
    QComboBox *listAll;
    QLabel *HourLabel;
    QLabel *DateLabel;
    QLCDNumber *speedDisplay;
      QSlider* pSlider;


    SlidingStackedWidget *slidingStacked;
    QHBoxLayout *mainLayout;
    QVBoxLayout *RightPaneLayout;
    QVBoxLayout *LeftPaneLayout;
    QVBoxLayout *MainCenterPanelLayout;
    QHBoxLayout *seekerLayout;
    QHBoxLayout *playbackLayout;
    QVBoxLayout *MainPhononLayout;
    QHBoxLayout *PrevNextLayout;
    QGridLayout *mediaLayout;

    QWidget *centralWidget;
    QWidget *slideWidget1;
    QWidget *slideWidget2;
    QWidget *slideWidget3;
    QWidget *slideWidget4;
    QWidget *slideWidget5;
    QWidget *slideWidget6;
    QWidget *slideWidget7;
    QWidget *RightPanelWidget;
    QWidget *MainCenterPanelWidget;
    QVBoxLayout *slideWidget6layout;
    QVBoxLayout *slideWidget1layout;

    int animTime;

private:
    void setupActions();
    void setupMenus();
    void setupUi();



   // Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    QList<Phonon::MediaSource> sources;
    QFileSystemModel *Music_Library;
    bool outOfBounds;

    QAction *playAction;
    QPushButton *pauseButton;
    QPushButton *playButton;

    QPushButton *forwardButton;
    QPushButton *backwardButton;
    QAction *pauseAction;
    QAction *stopAction;
    QAction *forwardAction;
    QAction *previousAction;
    QAction *addFilesAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QLabel *timeLcd;
    QTableWidget *musicTable;
     QTimer *timer1;
     QTimer *timer2;
     QTimer *timer3;
    int i,j,b;
    //using namespace std;
    bool flag;

    char CommandRpm[16];
    char Commandspeed[16];
    char CommandAte0[16];
    char CommandFuel[16];
    char CommandTemp[16];
    char CommandGetTime[17];
    char CommandSetTime[17];
    char FuelAnswer[16];
    char TempAnswer[16];
    char RpmAnswer[64];
    char SpeedAnswer[64];
    char Ate0Answer[64];
    char qFuelAnswer[64];
    char qTempAnswer[64];
    char qRpmAnswer[64];
    char qSpeedAnswer[64];
    char qAte0Answer[64];
    char CodeAns[64];
    char ans[1];
    char CodeSend[64];
    void stmRead	(int, char[]);
    void stmWrite	(int, char[]);
    void cleanComObject(unsigned char[]);
    int ttyfd;
    QString Ans;
    int slide;
    int count;
    int count1;
    int vel;
    int pm_a;
    int pm_b;
    int pm;
    int FuelValue;
    int TempValue;
    QLCDNumber *lcd_vel;
    QLCDNumber *lcd_Rpm;
    QLabel *ObdTitleLabel;
    QLabel *code;
    QLabel *SpeedTitle;
    QLabel *RpmTitle;
    QLineEdit *LineEdit1;
    QLineEdit *placaEdit;
    QWidget *widgetPhonon;
    QWidget *playbackwidget;
  //  QLineEdit *LineEdit2;
    QPushButton *send;
    QPushButton *clear;
    QPushButton *setdate;

    QLabel *FuelImage;
    typedef struct  customtm ComInfo;
    ComInfo *comObjectPtr;
    ComInfo *comObjectSetDate;

    widgetKeyBoard *mykeyboard;
    QSpinBox *plateSpin;
    QSpinBox *monthCombo;
    QSpinBox *yearCombo;
    QSpinBox *hourCombo;
    QSpinBox *minuteCombo;
    QSpinBox *wdayCombo;
    QDateEdit *dateSpin;
    QTimeEdit *timeSpin;
    QList<CarEvent> carEventList1;
    QList<CarEvent> carEventList2;
    QString carEventFilePath;

private slots:
    void stmReadRTC	(int,ComInfo*);
    void stmWriteRTC(int,ComInfo*);
    void addFiles();
    void about();
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void tick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void metaStateChanged(Phonon::State newState, Phonon::State oldState);
    void aboutToFinish();
    void tableClicked(int row, int column);
    void fileSaveTest();
    void fileOpenTest();
};

#endif // MAINWINDOW_H
