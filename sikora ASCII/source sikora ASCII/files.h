#ifndef FILES_H
#define FILES_H

#include <QObject>
#include <QWidget>
#include <QTextStream>
#include <QFile>
#include <QDir>

static QString file_setup = "setup_sikora.txt";
static QString file_direction = "/home";

class files
{
private:

public:
    QString *fileName;
    QFile file;
    QDir directionName;
};

#endif // FILES_H
