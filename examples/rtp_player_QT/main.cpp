#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


#include <av_log.h>

void qdebug_log(LogLevel le, const char* log)
{
    qDebug()<<"av_engine log:"<<le<<"\t"<<log;
}


int main(int argc, char *argv[])
{
    av_init();
    av_set_logger(qdebug_log);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
