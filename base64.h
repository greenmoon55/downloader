#ifndef BASE64_H
#define BASE64_H

#include <QString>
#include <QStringList>
#include <QDebug>

class base64
{
public:
    base64();
    static QString thunderURL(QString url);
    static QString flashgetURL(QString url);
    static QString qqdlURL(QString url);
};

#endif // BASE64_H
