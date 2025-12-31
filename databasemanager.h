#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>  // 确保包含QObject头文件

class DatabaseManager : public QObject  // 必须继承QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);

signals:
};

#endif // DATABASEMANAGER_H
