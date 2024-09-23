
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
    _countSend++;

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
            }).Error([&] { });
        },
        false);
}

void NetHandle::SendToApiGet(QString api, Json::Value data)
{
    flogd("SendToApiGet: "<<$Q(api));
    _countSend++;

    QString urlID = api;
    QNetworkRequest req;
    auto url = Network::Api(api);
    QUrlQuery query;
    for(auto it = data.begin();it != data.end();it++)
    {
        QString key = QString::fromStdString(it.name());
        QString val = QString::fromStdString((*it).asString());
        query.addQueryItem(key, val);
        vlogd($Q(key)$Q(val));
    } 
    url.setQuery(query);
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    Network::Get(req, [=](QByteArray &rspData, ...) {
        UtilFunc::ParseJson(rspData.data(), [&](Json::Value &value) {
            flogd($Q(api) $(UtilFunc::WriteJsonFormat(value)));
        }).Error([&] {});
    });
}

void NetHandle::PushTaskPost(Json::Value config)
{
    auto type = config["type"].asString(); 
    int time = config["timer"].asInt(); 
    int count = config["count"].asInt(); 
    auto sp_next = std::make_shared<int>(0);

    if(time > 0 && count != 0 && count != 1)
    {
        QTimer *ti = new QTimer(this);
        connect(ti,&QTimer::timeout,[=](){
            if(type == "post")
            {
                SendToApiPost(QString::fromStdString(config["api"].asString()),config["data"]);
            }
            else if(type == "get")
            {
                SendToApiGet(QString::fromStdString(config["api"].asString()),config["data"]);
            }
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
        if(type == "post")
        {
            SendToApiPost(QString::fromStdString(config["api"].asString()),config["data"]);
        }
        else if(type == "get")
        {
            SendToApiGet(QString::fromStdString(config["api"].asString()),config["data"]);
        }
    }
    emit sn_timer_num(_vecTask.size());

}

void NetHandle::ResetTask()
{
    for(auto &a:_vecTask)
    {
        a->stop();
        a->deleteLater();
    }
    _vecTask.clear();
    emit sn_timer_num(_vecTask.size());

    if(_countSend > 0)
    {
        emit sn_reset_count(_countSend);
        _countSend = 0;
    }
}

int NetHandle::GetCountSend()
{
    return _countSend;
}
