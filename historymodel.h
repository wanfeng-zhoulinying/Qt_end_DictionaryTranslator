#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QObject>  // 包含QObject头文件

class HistoryModel : public QObject  // 继承QObject
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject *parent = nullptr);

signals:
};

#endif // HISTORYMODEL_H
