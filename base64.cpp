#include "base64.h"
// 支持迅雷 快车 旋风专用链
// www.pc6.com可以用来测试

base64::base64()
{
}

void base64::decodeBase64(QString& str)
{
    QByteArray ba = str.toLocal8Bit();
    ba = QByteArray::fromBase64(ba);
    str = QString::fromLocal8Bit(ba);
    qDebug() << str;
}

QString base64::thunderURL(QString url)
{
    decodeBase64(url);
    url.remove(0, 2);
    url.chop(2);
    qDebug() << url;
    return url;
}

QString base64::flashgetURL(QString url)
{
    url = url.split("&").at(0);
    decodeBase64(url);
    url.remove(0, 10);
    url.chop(10);
    return url;
}

QString base64::qqdlURL(QString url)
{
    decodeBase64(url);
    return url;
}
