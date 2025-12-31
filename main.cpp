#include "mainwindow.h"
#include "databasemanager.h"
#include "historymodel.h"  // 添加这行
#include <QApplication>
#include <QDebug>
#include <QTableView>  // 用于测试显示

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "=== 测试数据库功能 ===";

    // 1. 测试数据库功能
    DatabaseManager& db = DatabaseManager::getInstance();

    // 添加一些测试数据
    db.addHistory("hello", "你好", "en", "zh");
    db.addHistory("world", "周霖營", "en", "zh");
    db.addHistory("thank you", "慢走", "en", "zh");
    db.addHistory("goodbye", "不送", "en", "zh");

    // 2. 测试HistoryModel
    qDebug() << "\n=== 测试HistoryModel ===";
    HistoryModel model;

    qDebug() << "模型行数:" << model.rowCount();
    qDebug() << "模型列数:" << model.columnCount();

    // 测试数据访问
    if (model.rowCount() > 0) {
        QModelIndex index = model.index(0, 0);  // 第一行第一列
        QString word = model.data(index, Qt::DisplayRole).toString();
        qDebug() << "第一行单词:" << word;

        index = model.index(0, 1);  // 第一行第二列
        QString translation = model.data(index, Qt::DisplayRole).toString();
        qDebug() << "第一行翻译:" << translation;
    }

    // 3. 简单测试TableView显示（可选）
    QTableView tableView;
    tableView.setModel(&model);
    tableView.setWindowTitle("历史记录测试");
    tableView.resize(600, 300);
    tableView.show();

    MainWindow w;
    w.show();

    return a.exec();
}
