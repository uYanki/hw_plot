#ifndef GETLOCALIP_H
#define GETLOCALIP_H

#include <QNetworkInterface>
#include <QStringList>

QStringList getlocalip();

#include <QRegExpValidator>
#define LIMIT_PORT "^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"  // 0~65535
// new QRegExpValidator(QRegExp(LIMIT_PORT), this)

#endif  // GETLOCALIP_H
