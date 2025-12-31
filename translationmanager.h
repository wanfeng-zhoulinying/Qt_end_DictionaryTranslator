#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>  // 包含QObject头文件

class TranslationManager : public QObject  // 继承QObject
{
    Q_OBJECT

public:
    explicit TranslationManager(QObject *parent = nullptr);

signals:
};

#endif // TRANSLATIONMANAGER_H
