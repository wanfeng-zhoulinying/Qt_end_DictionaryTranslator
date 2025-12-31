#include "databasemanager.h"
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

// 单例实例获取
DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

// 构造函数
DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    initializeDatabase();
}

// 初始化数据库
bool DatabaseManager::initializeDatabase()
{
    qDebug() << "Initializing database...";

    // 1. 获取应用数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);

    // 2. 创建目录（如果不存在）
    if (!dir.exists()) {
        if (!dir.mkpath(dataPath)) {
            qDebug() << "Failed to create directory:" << dataPath;
            return false;
        }
        qDebug() << "Created directory:" << dataPath;
    }

    // 3. 设置数据库路径
    QString dbPath = dataPath + "/dictionary.db";
    qDebug() << "Database path:" << dbPath;

    // 4. 打开数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError();
        return false;
    }

    qDebug() << "Database opened successfully";

    // 5. 创建数据表
    return createTables();
}

// 创建数据表
bool DatabaseManager::createTables()
{
    QSqlQuery query;

    // 创建历史记录表
    QString createHistoryTable =
        "CREATE TABLE IF NOT EXISTS search_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "word TEXT NOT NULL, "
        "translation TEXT, "
        "source_lang TEXT DEFAULT 'auto', "
        "target_lang TEXT DEFAULT 'zh', "
        "search_time DATETIME DEFAULT CURRENT_TIMESTAMP)";

    if (!query.exec(createHistoryTable)) {
        qDebug() << "Failed to create history table:" << query.lastError();
        return false;
    }

    qDebug() << "History table created successfully";
    return true;
}

// 添加历史记录
bool DatabaseManager::addHistory(const QString &word, const QString &translation,
                                 const QString &sourceLang, const QString &targetLang)
{
    if (!m_database.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO search_history (word, translation, source_lang, target_lang) "
                  "VALUES (?, ?, ?, ?)");
    query.addBindValue(word);
    query.addBindValue(translation);
    query.addBindValue(sourceLang);
    query.addBindValue(targetLang);

    if (!query.exec()) {
        qDebug() << "Failed to add history:" << query.lastError();
        return false;
    }

    qDebug() << "History added:" << word << "->" << translation;
    return true;
}

// 获取历史记录
QVector<HistoryRecord> DatabaseManager::getHistory(int limit)
{
    QVector<HistoryRecord> history;

    if (!m_database.isOpen()) {
        qDebug() << "Database is not open";
        return history;
    }

    QSqlQuery query;
    query.prepare("SELECT word, translation, source_lang, target_lang, "
                  "datetime(search_time, 'localtime') as search_time "
                  "FROM search_history "
                  "ORDER BY search_time DESC "
                  "LIMIT ?");
    query.addBindValue(limit);

    if (query.exec()) {
        while (query.next()) {
            HistoryRecord record;
            record.word = query.value("word").toString();
            record.translation = query.value("translation").toString();
            record.sourceLang = query.value("source_lang").toString();
            record.targetLang = query.value("target_lang").toString();
            record.timestamp = query.value("search_time").toString();
            history.append(record);
        }
    } else {
        qDebug() << "Failed to get history:" << query.lastError();
    }

    return history;
}

// 清空历史记录
bool DatabaseManager::clearHistory()
{
    if (!m_database.isOpen()) {
        return false;
    }

    QSqlQuery query;
    if (!query.exec("DELETE FROM search_history")) {
        qDebug() << "Failed to clear history:" << query.lastError();
        return false;
    }

    qDebug() << "History cleared";
    return true;
}
