#ifndef UYK_SAVEFILE_H
#define UYK_SAVEFILE_H

#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>

bool savefile(const QString& filename, std::function<void(QTextStream&)> pFunc);

#endif  // UYK_SAVEFILE_H
