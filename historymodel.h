#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "databasemanager.h"  // 包含数据库管理器

class HistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject *parent = nullptr);

    // 必须重写的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 自定义功能函数
    void refreshData();  // 刷新数据
    bool removeHistory(int row);  // 删除指定行的历史记录
    void clearAll();  // 清空所有历史记录

private:
    QVector<HistoryRecord> m_historyData;  // 存储历史记录数据
};

#endif // HISTORYMODEL_H
