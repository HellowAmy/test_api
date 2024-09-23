#include "Network.h"
#include "Encrypt.h"
#include <qeventloop.h>
#include <qhttpmultipart.h>
#include <qsettings.h>
#include <qtimer.h>
#include "Tvlog.h"
// 改为30秒可以延长主机消息回复时间，否则会出现超时取消请求的问题
#ifdef DEBUG
#define ENCRYPT 0
#else
#define ENCRYPT 0
#endif

#ifndef ENCRYPT_LIB_FOUND
#undef ENCRYPT
#define ENCRYPT 0
#endif
void SignalRInit(std::function<void(bool)> callback);


std::function<void(bool)> _callback_ = nullptr; 


static int _initTime = 0;
void SignalRInit(std::function<void(bool)> callback)
{
    vlogd($Q(Network::_serviceIp) $(Network::_servicePort) $Q(Network::_hostIp) $(Network::_hostPort));

    Network::_reqTimeOut = 5000;
    QNetworkRequest req;
    auto url = Network::Api("/NoticeHub/negotiate");
    QUrlQuery query;
    query.addQueryItem("negotiateVersion", "1");
    url.setQuery(query);
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  "Microsoft SignalR/7.0 (7.0.3+febee99db845fd8766a13bdb391a07c3ee90b4ba; Windows NT; "
                  ".NET; .NET Framework 4.8.4645.0)");
    Network::Post(
        req, "",
        [=](QByteArray &rspData, int code, ...) {
            bool flag = false;
            if (code == 200)
            {
                UtilFunc::ParseJson(rspData, [&](Json::Value &value) {
                    flogd("Login: " << $(UtilFunc::WriteJsonFormat(value)));   
                    
                    std::string connectionToken = value["connectionToken"].asString();
                    _initTime = 0;
                    bool ok = WebSock::instance()->Connect(QString::fromStdString(connectionToken));
                    if(ok == false)
                    {
                        flag = true;
                    }
                }).Error([&]() {
                    qCritical() << "can not parse the signalr connection token";
                    qDebug() << rspData;
                    flag = true;
                });
            }
            else
            {
                qCritical() << "can not get signalr connection token";
                flag = true;
            }
            if (flag)
            {
                _initTime++;
                if (_initTime >= 10000)
                {
                    Network::_reqTimeOut = 30000;
                    callback(false);
                    return;
                }
                vlogd("== Reconnect ==");
                QTimer::singleShot(1000, [=]() { SignalRInit(callback); });
            }
            else
            {
                Network::_reqTimeOut = 30000;
                callback(true);
            }
        },
        false);
}


WebSock::WebSock()
{
}
WebSock::~WebSock()
{
}
DEF_SINGLE(WebSock)


namespace Network
{
QNetworkAccessManager *_netWorkmanager = nullptr;

QString _serviceIp = "127.0.0.1";

unsigned short _servicePort = 5000;

const QString _scheme = "http";

static BufferEle _key[32]{0};

static BufferEle _iv[16]{0};

QString _hostIp = "";

unsigned short int _hostPort = 50000;

QByteArray _token = "";

QByteArray _refreshToken = "";

QString _devId = "";

// QString _account = "";

// QString _passwd = "";

int _reqTimeOut = 30000;


void Post(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken)
{
    QByteArray arr = PreprocessReq(data, req, takeToken);
    auto rsp = _netWorkmanager->post(req, arr);
    WaitForRsp(rsp, callback);
}

void Post(QNetworkRequest &req, QHttpMultiPart *multipart, RspCallback callback, bool takeToken)
{
    if (takeToken)
    {
        TAKETOKEN(req);
    }
    auto rsp = _netWorkmanager->post(req, multipart);
    multipart->setParent(rsp);
    WaitForRsp(rsp, callback);
}

void Get(QNetworkRequest &req, RspCallback callback, bool takeToken)
{
    PreprocessReq(QByteArray(), req, takeToken);
    auto rsp = _netWorkmanager->get(req);
    WaitForRsp(rsp, callback);
}

void Get(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken)
{
    QByteArray arr = PreprocessReq(data, req, takeToken);
    auto rsp = _netWorkmanager->sendCustomRequest(req, "Get", arr);
    WaitForRsp(rsp, callback);
}

void Put(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken)
{
    QByteArray arr = PreprocessReq(data, req, takeToken);
    auto rsp = _netWorkmanager->sendCustomRequest(req, "Put", arr);
    WaitForRsp(rsp, callback);
}

void Delete(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken)
{
    QByteArray arr = PreprocessReq(data, req, takeToken);
    auto rsp = _netWorkmanager->sendCustomRequest(req, "DELETE", arr);
    WaitForRsp(rsp, callback);
}

void Patch(QNetworkRequest &req, const QByteArray &data, RspCallback callback, bool takeToken)
{
    QByteArray arr = PreprocessReq(data, req, takeToken);
    auto rsp = _netWorkmanager->sendCustomRequest(req, "PATCH", arr);
    WaitForRsp(rsp, callback);
}

void WaitForRsp(QNetworkReply *rsp, RspCallback &callback)
{
    auto timer = new QTimer;
    QObject::connect(
        rsp, &QNetworkReply::finished, rsp,
        [=]() {
            timer->stop();
            timer->deleteLater();
            QByteArray deData;
            if (rsp->bytesAvailable() > 0)
            {
                deData = PreprocessRsp(rsp->readAll());
            }
            if (callback)
            {
                callback(deData, rsp->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt(),
                         rsp->rawHeaderPairs());
            }
        },
        Qt::ConnectionType::QueuedConnection);
    QObject::connect(timer, &QTimer::timeout, timer, [rsp, timer]() {
        rsp->abort();
        timer->deleteLater();
    });
    timer->start(_reqTimeOut);
}

QByteArray PreprocessReq(const QByteArray &preData, QNetworkRequest &req, bool takeToken)
{
    if (takeToken)
    {
        TAKETOKEN(req);
    }
#if ENCRYPT == 0
    return preData;
#endif
    QByteArray ret;
    aes256AndBase64En((const Buffer)preData.data(), preData.size(), _key, _iv, PaddingType::Zero,
                      [&](const Buffer dst, size_t dstLen) { ret.append((const char *)dst, dstLen); });
    return ret;
}

QByteArray PreprocessRsp(const QByteArray &preData)
{
#if ENCRYPT == 0
    return preData;
#endif
    QByteArray ret;
    aes256AndBase64De((const Buffer)preData.data(), preData.size(), _key, _iv, PaddingType::Zero,
                      [&](const Buffer dst, size_t dstLen) { ret.append((const char *)dst, dstLen); });
    return ret;
}

QUrl Api(const QString &path)
{
    QUrl url;
    url.setScheme(_scheme);
    url.setHost(_serviceIp);
    url.setPort(_servicePort);
    url.setPath(path);
    return url;
}


void init(std::function<void(bool)> callback)
{
    _callback_ = callback;
    _netWorkmanager = new QNetworkAccessManager;
    _netWorkmanager->setAutoDeleteReplies(true);
    _key[0] = '8';
    _key[1] = '7';
    _key[2] = 'a';
    _key[3] = 'e';
    _key[4] = 'f';
    _key[5] = '2';
    _key[6] = '9';
    _key[7] = 'e';
    _key[8] = '7';
    _key[9] = '1';
    _key[10] = 'a';
    _key[11] = 'd';
    _key[12] = '4';
    _key[13] = '8';
    _key[14] = '7';
    _key[15] = '3';
    _key[16] = '9';
    _key[17] = '1';
    _key[18] = '9';
    _key[19] = '3';
    _key[20] = '3';
    _key[21] = '8';
    _key[22] = 'e';
    _key[23] = '6';
    _key[24] = '2';
    _key[25] = '3';
    _key[26] = '1';
    _key[27] = '5';
    _key[28] = '8';
    _key[29] = '8';
    _key[30] = 'a';
    _key[31] = 'e';
    QSettings settings("../config/config.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    _serviceIp = settings.value("Server/ServerIp", "127.0.0.1").toString();
    _servicePort = settings.value("Server/ServerPort", 5000).toUInt();
    _hostIp = settings.value("Server/HostIP", "172.16.21.21").toString();
    _hostPort = settings.value("Server/HostPort", 50000).toUInt();

    SignalRInit(callback);
}
} // namespace Network




void ReceviedMsg(Json::Value &rspValue)
{
    auto &value = rspValue["Data"];
    int typeCmd = value["CmdType"].asInt();
    if (typeCmd != ReceivedEnum::HEARTBEAT && typeCmd != ReceivedEnum::IsConnect && typeCmd != ReceivedEnum::InputVolBar &&
        typeCmd != ReceivedEnum::MicVolBar && rspValue["Code"].asInt64() != 5)
    {
        // qDebug() << UtilFunc::WriteJson(rspValue).c_str();
        flogd("ReceviedMsg: " << $(UtilFunc::WriteJsonFormat(rspValue)));   
    }
    switch (typeCmd)
    {
    case ReceivedEnum::HEARTBEAT: {

        break;
    }
    case ReceivedEnum::IsConnect: {
        int type = value["Type"].asInt64();
        emit _sp_net_->sn_connect_host(type);
        break;
    }

    }
}

void WebSock::OnDisconnected()
{
    if (_sock)
    {
        _sock->deleteLater();
        _sock = nullptr;
    }
    emit _sp_net_->sn_connect_sdk(false);

    if(_callback_)
    {
        SignalRInit(_callback_);
    }

}
