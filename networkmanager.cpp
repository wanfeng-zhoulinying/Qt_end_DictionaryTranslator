#include "networkmanager.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QFile>
#include <QIODevice>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>


NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // 连接网络请求完成信号
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &NetworkManager::onTranslationFinished);
}

void NetworkManager::translateText(const QString &text,
                                   const QString &fromLang,
                                   const QString &toLang)
{
    if (text.isEmpty()) {
        emit translationError("翻译文本不能为空");
        return;
    }

    qDebug() << "开始翻译:" << text << "从" << fromLang << "到" << toLang;

    // 构建百度翻译API请求
    QString requestUrl = buildBaiduRequest(text, fromLang, toLang);

    if (requestUrl.isEmpty()) {
        emit translationError("API配置错误");
        return;
    }

    // 创建网络请求
    QNetworkRequest request;
    request.setUrl(QUrl(requestUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", "DictionaryTranslator/1.0");

    // 发送GET请求
    m_networkManager->get(request);
}

QString NetworkManager::buildBaiduRequest(const QString &text,
                                          const QString &from,
                                          const QString &to)
{
    // 从config.json读取配置
    QFile configFile("config.json");
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开配置文件 config.json";
        qDebug() << "请在同级目录创建config.json文件，格式参考：";
        qDebug() << "{\"baidu_translate\":{\"app_id\":\"YOUR_APP_ID\",\"secret_key\":\"YOUR_KEY\"}}";
        return "";
    }

    QByteArray configData = configFile.readAll();
    configFile.close();

    QJsonParseError jsonError;
    QJsonDocument configDoc = QJsonDocument::fromJson(configData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "配置文件JSON格式错误:" << jsonError.errorString();
        return "";
    }

    QJsonObject configObj = configDoc.object();
    if (!configObj.contains("baidu_translate")) {
        qDebug() << "配置文件中缺少baidu_translate配置项";
        return "";
    }

    QJsonObject baiduConfig = configObj["baidu_translate"].toObject();
    QString appid = baiduConfig["app_id"].toString();
    QString key = baiduConfig["secret_key"].toString();

    if (appid.isEmpty() || key.isEmpty()) {
        qDebug() << "请先在config.json中配置百度翻译API的app_id和secret_key";
        qDebug() << "当前配置: app_id=" << appid << "key=" << (key.isEmpty() ? "空" : "已设置");
        return "";
    }

    // 生成随机盐值
    QString salt = QString::number(QDateTime::currentSecsSinceEpoch());

    // 构建签名：appid + 原文 + salt + 密钥
    QString signString = appid + text + salt + key;

    // 计算MD5签名
    QByteArray signMd5 = QCryptographicHash::hash(
        signString.toUtf8(), QCryptographicHash::Md5);
    QString sign = QString(signMd5.toHex());

    // 构建请求URL
    QString baseUrl = "http://api.fanyi.baidu.com/api/trans/vip/translate";
    QUrlQuery query;
    query.addQueryItem("q", text);
    query.addQueryItem("from", from);
    query.addQueryItem("to", to);
    query.addQueryItem("appid", appid);
    query.addQueryItem("salt", salt);
    query.addQueryItem("sign", sign);

    return baseUrl + "?" + query.toString();
}

void NetworkManager::onTranslationFinished(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = QString("网络错误: %1").arg(reply->errorString());
        qDebug() << errorString;
        emit translationError(errorString);
        reply->deleteLater();
        return;
    }

    // 读取响应数据
    QByteArray responseData = reply->readAll();
    QString originalText = reply->url().query(QUrl::FullyDecoded);

    qDebug() << "收到翻译响应:" << responseData;

    // 解析响应
    QString translatedText = parseBaiduResponse(responseData);

    if (translatedText.isEmpty()) {
        emit translationError("翻译结果解析失败");
    } else {
        // 提取请求参数用于信号传递
        QUrlQuery query(reply->url());
        QString fromLang = query.queryItemValue("from");
        QString toLang = query.queryItemValue("to");
        QString original = query.queryItemValue("q");

        emit translationFinished(original, translatedText, fromLang, toLang);
    }

    reply->deleteLater();
}

QString NetworkManager::parseBaiduResponse(const QByteArray &data)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << parseError.errorString();
        return "";
    }

    if (!doc.isObject()) {
        qDebug() << "响应格式不是JSON对象";
        return "";
    }

    QJsonObject rootObj = doc.object();

    // 检查错误码
    if (rootObj.contains("error_code")) {
        int errorCode = rootObj["error_code"].toInt();
        QString errorMsg = rootObj["error_msg"].toString();
        qDebug() << "API错误:" << errorCode << errorMsg;
        return "";
    }

    // 提取翻译结果
    if (rootObj.contains("trans_result")) {
        QJsonArray transArray = rootObj["trans_result"].toArray();
        if (!transArray.isEmpty()) {
            QJsonObject firstResult = transArray[0].toObject();
            if (firstResult.contains("dst")) {
                QString result = firstResult["dst"].toString();
                qDebug() << "翻译结果:" << result;
                return result;
            }
        }
    }

    qDebug() << "未找到翻译结果";
    return "";
}
