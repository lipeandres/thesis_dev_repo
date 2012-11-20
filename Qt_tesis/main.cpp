#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

#ifdef Q_OS_SYMBIAN
    w.showMaximized();
#else

    w.setMaximumSize(480,272);
    w.setMinimumSize(480,272);
      w.resize(480, 272);
  // w.show();
   w.showFullScreen();
#endif

    return a.exec();
}
