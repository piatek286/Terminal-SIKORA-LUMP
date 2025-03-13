#ifndef FILES_H
#define FILES_H

#include <QObject>
#include <QWidget>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

static QString file_setup = "setup_sikora.txt";
static QString file_direction = "/home";//windows
//static const QString& file_direction = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);//linux

class files
{
private:

public:
    QString *fileName;
    QFile file;
    QDir directionName;
};

#endif // FILES_H
