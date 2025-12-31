#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>  // 包含QObject头文件

class NetworkManager : public QObject  // 继承QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

signals:
};

#endif // NETWORKMANAGER_H
