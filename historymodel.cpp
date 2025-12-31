#include "historymodel.h"
#include <QDebug>

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    refreshData();  // 构造函数中初始化数据
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_historyData.size();
}

int HistoryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 5;  // 5列：单词、翻译、源语言、目标语言、时间
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_historyData.size())
        return QVariant();

    const HistoryRecord &record = m_historyData.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return record.word;          // 单词
        case 1: return record.translation;    // 翻译
        case 2: return record.sourceLang;    // 源语言
        case 3: return record.targetLang;    // 目标语言
        case 4: return record.timestamp;     // 时间
        default: return QVariant();
        }
    }

    // 文本对齐角色
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "单词";
        case 1: return "翻译结果";
        case 2: return "源语言";
        case 3: return "目标语言";
        case 4: return "查询时间";
        default: return QVariant();
        }
    }
    return QVariant();
}

void HistoryModel::refreshData()
{
    beginResetModel();  // 通知视图数据将要重置

    // 从数据库获取最新数据
    m_historyData = DatabaseManager::getInstance().getHistory();

    endResetModel();   // 通知视图数据重置完成

    qDebug() << "HistoryModel数据已刷新，共" << m_historyData.size() << "条记录";
}

bool HistoryModel::removeHistory(int row)
{
    if (row < 0 || row >= m_historyData.size())
        return false;

    // 这里先简单实现，后续可以添加具体的删除逻辑
    qDebug() << "删除历史记录行:" << row;
    return true;
}

void HistoryModel::clearAll()
{
    if (DatabaseManager::getInstance().clearHistory()) {
        refreshData();  // 清空成功后刷新数据
        qDebug() << "已清空所有历史记录";
    }
}
