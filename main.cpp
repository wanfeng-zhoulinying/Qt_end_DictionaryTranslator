#include "mainwindow.h"
#include "databasemanager.h"  // 添加这行
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 测试数据库功能
    qDebug() << "=== 测试数据库功能 ===";

    // 1. 获取数据库实例（会自动初始化）
    DatabaseManager& db = DatabaseManager::getInstance();

    // 2. 测试添加历史记录
    bool success = db.addHistory("hello", "你好", "en", "zh");
    qDebug() << "添加历史记录:" << (success ? "success" : "fail");

    success = db.addHistory("world", "周霖營", "en", "zh");
    qDebug() << "添加历史记录:" << (success ? "success" : "fail");

    // 3. 测试查询历史记录
    QVector<HistoryRecord> history = db.getHistory(10);
    qDebug() << "查询到" << history.size() << "条历史记录:";

    for (const auto& record : history) {
        qDebug() << "[" << record.timestamp << "]"
                 << record.word << "->" << record.translation
                 << "(" << record.sourceLang << "->" << record.targetLang << ")";
    }

    // 4. 测试清空历史记录（可以先注释掉，以便查看数据）
    // db.clearHistory();

    MainWindow w;
    w.show();

    return a.exec();
}
