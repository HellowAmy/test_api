#ifndef NETWORK_H
#define NETWORK_H
#include "UtilFunc.h"
#include "UtilMacro.h"
#include <functional>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qurlquery.h>
#include <qvector.h>
#include <qwebsocket.h>
#include <string>
#include <QTimer>
#include <QEventLoop>
#include <QByteArray>
#include "NetHandle.h"
#include "NetHandle.h"





using HeaderList = const QList<QNetworkReply::RawHeaderPair>;

using RspCallback = std::function<void(QByteArray &, int, HeaderList &)>;

#define TAKETOKEN(req) req.setRawHeader("Authorization", _token)

#define TAKEDEVID(value)                                                                                               \
    value["Ip"] = _hostIp;                                                                                             \
    value["Port"] = _hostPort;






namespace Network
{
/// @brief 发送post请求
/// @param req 请求头
/// @param data 请求载体
/// @param callback 服务器响应回调函数
/// @param takeToken 是否带上token
void Post(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken = true);

/// @brief 发送multipart post请求（参考http multpart/form-data格式）
/// @param req 请求头
/// @param multipart 请求体
/// @param callback 服务器响应回调
/// @param takeToken 是否带上token
void Post(QNetworkRequest &req, QHttpMultiPart *multipart, RspCallback callback, bool takeToken = true);

/// @brief 发送get请求(
/// @param req 请求头
/// @param callback 服务器响应回调
/// @param takeTooen 是否带上token
void Get(QNetworkRequest &req, RspCallback callback, bool takeTooen = true);

/// @brief 发送get请求(带负载)
/// @param req
/// @param data
/// @param callback
/// @param takeTooen
void Get(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeTooen = true);

/// @brief 发送put请求
/// @param req 请求头
/// @param data 请求体
/// @param callback 服务器响应回调
/// @param takeToken 是否带上token
void Put(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken = true);

/// @brief 发送delete请求
/// @param req 请求头
/// @param data 请求体
/// @param callback 服务器响应回调
/// @param takeToken 是否带上token
void Delete(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken = true);

/// @brief 发送Patch请求
/// @param req 请求头
/// @param data 请求体
/// @param callback 服务器响应回调
/// @param takeToken 是否带上token
void Patch(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken = true);

/// @brief 请求后等待服务器响应
/// @param rsp 等待响应的响应体
/// @param callback 服务器响应回调
void WaitForRsp(QNetworkReply *rsp, RspCallback &callback);

/// @brief 请求体预处理
/// @param preData 预处理数据
/// @param req 请求头
/// @param takeToken 是否带上token
/// @return 预处理后的数据
QByteArray PreprocessReq(const QByteArray &preData, QNetworkRequest &req, bool takeToken = true);

/// @brief 响应体预处理
/// @param preData 预处理数据
/// @return 预处理后的数据
QByteArray PreprocessRsp(const QByteArray &preData);

QUrl Api(const QString &path);

/// @brief 初始化
void init(std::function<void(bool)> callback);

extern QNetworkAccessManager *_netWorkmanager;

extern QString _serviceIp;

extern unsigned short _servicePort;

extern const QString _scheme;

extern QString _hostIp;

extern QString _devId;

extern unsigned short int _hostPort;

extern QByteArray _token;

extern QByteArray _refreshToken;

extern QString _account;

extern QString _passwd;

extern int _updateTime;

extern int _reqTimeOut;


}; // namespace Network





struct ReceivedEnum
{
    enum CmdTypeEnum
    {
        HEARTBEAT = 0x01,
        // 话筒类操作
        MicState = 0x06,
        // 扫描话筒结果 (反馈两次)
        MicScanResult = 0x08,
        // 扫描在线设备
        SCANDEVICE = 0x09,
        // 签到
        SignVoteOp = 0x07,
        // EQ类
        MicEQState = 0x0D,
        // 话筒模式类
        MicMode = 0x11,
        // wifi话筒的电量及信号
        WifiPowerSignal = 0x13,
        // 服务
        SERVICE = 0x14,
        // 单元机声控阈值与关闭时间
        MicSensitivity = 0x39,
        // 话筒灵敏度
        MicSens = 0x3A,
        // 自动增益单元参数(第四代)
        ModifyGain = 0x73,
        // 话筒音量
        ModifyMicVol = 0x72,
        // 摄像跟踪参数（全局）
        ZXPhotoTrackParam = 0x59,
        // 语音转写参数（全局）
        ZXTransParam = 0x5A,
        // 反馈抑制器参数
        ZXFeedParam = 0x5B,
        // 所有固定点参数
        ZXFixedPointParam = 0x5C,
        // 陷波点参数
        ZXRetNotParam = 0x5D,
        // 话筒处理器灵敏度数
        ZXSensProcessorMicParam = 0x5E,
        // 输入参量均衡器 （废弃）
        // InParameterEQ = 0x88,
        // 输入图示均衡器 （废弃）
        // InGraphicalEQ = 0x89,
        // 输出参量均衡器
        OutParameterEQ = 0x9F,
        // 输出图示均衡器
        OutGraphicalEQ = 0xA0,
        // 输出均衡器开关
        OutEQOPen = 0x9E,

        // // 图示均衡器开关
        // ZXEQBalance = 0x5f,
        // // 图示均衡器0x1-0xF频段参数
        // ZXEQBalanceParam = 0x60,
        // 主机IP地址
        HostIp = 0x64,
        // 子网地址
        SubnetAddr = 0x65,
        // 网关地址
        GatewayAddr = 0x66,
        // 版本信息
        Version = 0x67,
        // 摄像跟踪参数（单元）
        ZXPhotoTrackUnitParam = 0x68,
        // 动态点参数
        ZXDynamicPointsParam = 0x69,
        // 话筒音量
        ZXMicVol = 0x70,
        // 签到返回
        WvVote = 0x20,
        // 话筒个数及ID相关
        MicIdSeries = 0x12,
        // 注册码序列号
        HostSerialnumber = 0x15,
        // 注册信息
        HostRegisterInfo = 0x16,
        // 主机连接状态
        IsConnect = 0x50,
        // 陷波器（反馈抑制器）参数(音量，静音开关)
        ModifLineVol = 0x74,
        // 陷波器（反馈抑制器）参数(不包含音量，静音，固定点，动态点)
        ModifLine = 0x75,
        // 陷波器（反馈抑制器）参数(固定点参数）
        ModifLineFixed = 0x76,
        // 陷波器（反馈抑制器）参数(动态点参数)
        ModifLineDynamic = 0x77,
        // 电容灵敏度
        MicCapacitance = 0x80,
        // 设备主动推送捕获的固定点
        ModifLineFixedFrequency = 0x78,
        // 设备主动推送捕获的动态点
        ModifLineDynamicFrequency = 0x79,
        // 反馈抑制器版本信息
        ModifLineVersion = 0x81,
        // 自动混音
        AutoMixMode = 0x85,
        // 输入自动混音
        InputAutoMixData = 0x86,
        // 主从机信息
        MasterSlaveHost = 0x82,
        // 话筒右键均衡器
        MicEQValueList = 0x94,
        // 查询闪避器数据反馈
        InputDuckingData = 0x98,
        // 查询闪避器开关
        InputDuckingOpen = 0x96,
        // 查询分频器数据反馈
        InputDividerData = 0x87,
        // 话筒静音反相信息
        MicMuteInfo = 0x95,
        // 输入通道静音反相信息
        InputMuteInfo = 0x97,
        // 麦克风智能混音
        MicAutoMix = 0x9B,
        // 配置面板智能混音
        ConfigAutoMix = 0x9C,
        // 矩阵列表
        MatrixList = 0x9A,
        // 话筒跳动音量条
        InputVolBar = 0xA1,
        // 本地输入跳动音量条
        MicVolBar = 0xA2,
        // // 输出静音反向信息
        // OutputMuteInfo = 0x9D,
        // 主机音量的Line Out/In推送
        LineOutVolume = 0x84,

        MicMatrix = 0xA3
    };
};




void ReceviedMsg(Json::Value & sjson);


class WebSock : public QObject
{
    DECL_SINGLE(WebSock);

  public:
    bool Connect(const QString &connectionToken)
    {
        if (_sock)
        {
            _sock->deleteLater();
            _sock = nullptr;
        }
        _sock = new QWebSocket;
        QEventLoop loop;
        QTimer timer;
        QUrlQuery query;
        query.addQueryItem("id", connectionToken);
        QUrl url;
        url.setPath("/NoticeHub");
        url.setScheme("ws");
        url.setHost(Network::_serviceIp);
        url.setPort(Network::_servicePort);
        url.setQuery(query);
        auto conf = _sock->sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        conf.setProtocol(QSsl::SslProtocol::AnyProtocol);
        _sock->setSslConfiguration(conf);
        QObject::connect(_sock, &QWebSocket::stateChanged, &loop, &QEventLoop::quit);
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(10000);
        _sock->open(url);
        loop.exec();
        if (_sock->state() != QAbstractSocket::ConnectedState)
        {
            _sock->deleteLater();
            _sock = nullptr;
            return false;
        }
        QObject::connect(_sock, &QWebSocket::textMessageReceived, this, &WebSock::OnMessage);
        QObject::connect(_sock, &QWebSocket::binaryMessageReceived, this, &WebSock::OnBinaryMsg);
        QObject::connect(_sock, &QWebSocket::disconnected, this, &WebSock::OnDisconnected);
        QObject::connect(_sock, static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
                         this, &WebSock::OnError);
        Json::Value value;
        value["protocol"] = "json";
        value["version"] = 1;
        QByteArray data = UtilFunc::WriteJson(value).c_str();
        data.append(30);
        _sock->sendTextMessage(data);
        auto heartTimer = new QTimer(_sock);
        QObject::connect(heartTimer, &QTimer::timeout, _sock, [_sock = _sock]() {
            Json::Value value;
            value["type"] = 6;
            QByteArray data = UtilFunc::WriteJson(value).c_str();
            data.append(30);
            _sock->sendTextMessage(data);
        });
        heartTimer->start(5000);
        return true;
    }
    void Discnnect()
    {
        if (_sock)
        {
            _sock->disconnect();
            _sock->deleteLater();
            _sock = nullptr;
        }
    }

  protected:
    void OnDisconnected();

    void OnError(QAbstractSocket::SocketError error)
    {
        qCritical() << "error,websocket error occured,code" << error;
        if (_sock)
        {
            qCritical() << "error,websocket error occured,error info :" << _sock->errorString();
            _sock->deleteLater();
            _sock = nullptr;
        }
    }
    void OnBinaryMsg(const QByteArray &msg)
    {
        qWarning() << "recive binary msg," << msg;
    }

    void OnMessage(const QString &message)
    {
        auto msgList = message.split("\u001e", Qt::SkipEmptyParts, Qt::CaseInsensitive);
        for (auto &msg : msgList)
        {
            UtilFunc::ParseJson(msg, [&](Json::Value &value) {
                if (value["type"].asInt() == 6)
                {
                    return;
                }

                auto arguments = value["arguments"][0].asString();
                UtilFunc::ParseJson(arguments, ReceviedMsg).Error([&]() {
                    qCritical() << "error can not parse data ,---" << QString::fromStdString(arguments);
                });

            });
        }
    }

  private:
    QWebSocket *_sock = nullptr;
};




#endif // NETWORK_H