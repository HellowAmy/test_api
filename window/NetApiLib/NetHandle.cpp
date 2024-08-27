
#include "NetHandle.h"

NetHandle::NetHandle(QObject *parent) : QObject(parent)
{

}

NetHandle::~NetHandle()
{
}



void NetHandle::SendToApiPost(QString api, Json::Value data)
{
    flogd("SendToApiPost: "<<$Q(api));

    data["Ip"] = Network::_hostIp.toStdString();
    data["Port"] = Network::_hostPort;
    auto url = Network::Api(api);
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    Network::Post(
        req, UtilFunc::WriteJson(data).c_str(),
        [=](QByteArray &rspData, ...) {
            UtilFunc::ParseJson(rspData.data(), [&](Json::Value &value) {
                flogd($Q(api) $(UtilFunc::WriteJsonFormat(value)));   
            }).Error([&] {  });
        },
        false);
}

void NetHandle::SendToApiGet(QString api, Json::Value data)
{

}

void NetHandle::PushTaskPost(Json::Value config)
{
    int time = config["timer"].asInt(); 
    int count = config["count"].asInt(); 
    auto sp_next = std::make_shared<int>(0);

    if(time > 0 && count != 0 && count != 1)
    {
        QTimer *ti = new QTimer(this);
        connect(ti,&QTimer::timeout,[=](){
            SendToApiPost(QString::fromStdString(config["api"].asString()),config["data"]);
            if(count != -1)
            {
                (*sp_next)++;
                if(*sp_next >= count)
                {
                    ti->stop();
                }
            }
        });
        ti->start(time);
        _vecTask.push_back(ti);
    }
    else if(count == 1)
    {
        SendToApiPost(QString::fromStdString(config["api"].asString()),config["data"]);
    }

}

void NetHandle::ResetTask()
{
    for(auto &a:_vecTask)
    {
        a->stop();
        a->deleteLater();
    }
    _vecTask.clear();
}
