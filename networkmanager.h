#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// 前向声明
class QNetworkReply;

class NetworkManager : public QObject  // 继承QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    // 翻译文本的公共接口
    void translateText(const QString &text,
                       const QString &fromLang = "auto",
                       const QString &toLang = "zh");

signals:
    // 翻译完成信号
    void translationFinished(const QString &originalText,
                             const QString &translatedText,
                             const QString &fromLang,
                             const QString &toLang);
    // 翻译错误信号
    void translationError(const QString &errorString);

private slots:
    // 处理网络请求完成
    void onTranslationFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;

    // 百度翻译API请求构造
    QString buildBaiduRequest(const QString &text,
                              const QString &from,
                              const QString &to);

    // 解析百度翻译响应
    QString parseBaiduResponse(const QByteArray &data);
};

#endif // NETWORKMANAGER_H
