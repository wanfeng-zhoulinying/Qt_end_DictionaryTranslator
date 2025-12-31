#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

// 历史记录数据结构
struct HistoryRecord {
    QString word;           // 查询的单词
    QString translation;    // 翻译结果
    QString sourceLang;     // 源语言
    QString targetLang;     // 目标语言
    QString timestamp;      // 查询时间
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static DatabaseManager& getInstance();

    // 初始化数据库
    bool initializeDatabase();

    // 历史记录操作
    bool addHistory(const QString &word, const QString &translation,
                    const QString &sourceLang = "auto", const QString &targetLang = "zh");

    QVector<HistoryRecord> getHistory(int limit = 50);
    bool clearHistory();

    // 收藏夹操作（后续实现）
    // 缓存操作（后续实现）

private:
    explicit DatabaseManager(QObject *parent = nullptr);

    // 禁止拷贝和赋值
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_database;  // 数据库连接
    bool createTables();       // 创建数据表
};

#endif // DATABASEMANAGER_H
