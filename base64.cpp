#include "base64.h" //支持迅雷 快车 QQ


base64::base64()
{
}

QString base64::thunderURL(QString url)
{
    QByteArray baurl = url.toLocal8Bit();
    baurl = QByteArray::fromBase64(baurl);
    url = QString::fromLocal8Bit(baurl);
    url.remove(0, 2);
    url.chop(2);
    qDebug() << url;
    return url;
}

QString base64::flashgetURL(QString url)
{
    if(url.split("://").size()<2)
        return url;
    url=url.split("://").at(1);
    url=url.split("&").at(1);
    QByteArray baurl=url.toLocal8Bit();
    baurl=QByteArray::fromBase64(baurl);
    url=QString::fromLocal8Bit(baurl);
    url=url.left(url.length()-10);
    url=url.right(url.length()-10);
    return url;
}

QString base64::qqdlURL(QString url)
{
    if(url.split("://").size()<2)
        return url;
    url=url.split("://").at(1);
    QByteArray baurl=url.toLocal8Bit();
    baurl=QByteArray::fromBase64(baurl);
    url=QString::fromLocal8Bit(baurl);
    return url;
}
