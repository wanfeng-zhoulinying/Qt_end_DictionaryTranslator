#include "mainwindow.h"
#include "databasemanager.h"
#include "historymodel.h"
#include "networkmanager.h"  // 添加这行
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "=== 测试网络翻译功能 ===";

    // 创建网络管理器实例
    NetworkManager networkManager;

    // 连接翻译完成信号
    QObject::connect(&networkManager, &NetworkManager::translationFinished,
                     [](const QString &original, const QString &translated,
                        const QString &from, const QString &to) {
                         qDebug() << "翻译成功:";
                         qDebug() << "原文:" << original;
                         qDebug() << "译文:" << translated;
                         qDebug() << "语言:" << from << "->" << to;

                         // 测试：将结果保存到数据库
                         DatabaseManager::getInstance().addHistory(original, translated, from, to);
                     });

    // 连接错误信号
    QObject::connect(&networkManager, &NetworkManager::translationError,
                     [](const QString &error) {
                         qDebug() << "翻译错误:" << error;
                     });

    // 测试翻译
    networkManager.translateText("THe flower is very beautiful", "en", "zh");

    MainWindow w;
    w.show();

    return a.exec();
}
