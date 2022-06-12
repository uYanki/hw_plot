#ifndef UYK_SAVEFILE_H
#define UYK_SAVEFILE_H

#include <QFile>
#include <QDateTime>
#include <QString>
#include <QTextStream>
#include <QStandardPaths>
#include <QFileDialog>

bool savefile(QString suffix, std::function<void(QTextStream&)> pFunc);

#endif // UYK_SAVEFILE_H
