
#include "main_window.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include <QUdpSocket>
#include <QHBoxLayout>
#include <QIcon>

main_window::main_window(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("国产主机协议测试");
    this->setWindowIcon(QIcon("../config/pic/logo.ico"));

    timer_id = startTimer(1000);

    QVBoxLayout *lay_main = new QVBoxLayout(this);
    {
        QHBoxLayout *lay = new QHBoxLayout;
        _lab_status_sdk = new QLabel(this);
        _lab_status_host = new QLabel(this);
        _but_connect = new QPushButton("[连接主机]",this);

        lay->addWidget(_lab_status_sdk);
        lay->addWidget(_lab_status_host);
        lay->addWidget(_but_connect);
        lay_main->addLayout(lay);
    }
    {
        QHBoxLayout *lay = new QHBoxLayout;
        _but_send_api = new QPushButton("[发送配置指定协议]",this);
        _edit_api = new QLineEdit(this);
        _edit_api->resize(_edit_api->width(),_but_send_api->height());

        lay->addWidget(_edit_api);
        lay->addWidget(_but_send_api);
        lay_main->addLayout(lay);
    }
    {
        QHBoxLayout *lay = new QHBoxLayout;
        _but_update_file = new QPushButton("[更新配置文件并发送]",this);
        _but_send_json = new QPushButton("[发送自定义协议]",this);

        lay->addWidget(_but_update_file);
        lay->addWidget(_but_send_json);
        lay_main->addLayout(lay);
    }
    {
        QVBoxLayout *lay = new QVBoxLayout;
        _edit_json = new QTextEdit(this);
        _edit_json->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

        _lab_err_tips = new QListWidget(this);
        _lab_err_tips->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
        _lab_err_tips->setMaximumHeight(150);

        lay->addWidget(_edit_json);
        lay->addWidget(_lab_err_tips);
        lay_main->addLayout(lay);
    }

    SetErrTips("显示错误提示列表",false);
    SetStatusSdk(false);
    SetStatusHost(false);
    
    connect(_but_connect,&QPushButton::clicked,[=](){
        auto json = _sp_protocol_->GetApiConfig("/api/Login");
        if(json["api"].asString() != "")
        {
            _sp_net_->SendToApiPost(QString::fromStdString(json["api"].asString()),json["data"]);
        }
    });
    
    connect(_but_send_api,&QPushButton::clicked,[=](){
        auto json = _sp_protocol_->GetApiConfig(_edit_api->text());
        QString api = QString::fromStdString(json["api"].asString());
        if(api != "")
        {
            _sp_net_->PushTaskPost(json);
            SetErrTips("发送成功",false);
        }
        else 
        {
            SetErrTips(QString("配置文件不存在协议: %1").arg(api));
        }
    });

    connect(_but_send_json,&QPushButton::clicked,[=](){
        auto sjson = _edit_json->toPlainText();
        UtilFunc::ParseJson(sjson, [&](Json::Value &json) {
            QString api = QString::fromStdString(json["api"].asString());
            if(api != "")
            {
                _sp_net_->PushTaskPost(json);
                SetErrTips("发送成功",false);
            }
            else 
            {
                SetErrTips(QString("发送的协议接口类型不能为空"));
            }
        }).Error([=](){
            SetErrTips(QString("协议内容解析失败: %1").arg(sjson));
        });
    });

    connect(_but_update_file,&QPushButton::clicked,[=](){
        _sp_net_->ResetTask();
        _sp_protocol_->ReloadFile();
        auto vec = _sp_protocol_->GetUpdateApiList();
        for(auto &a:vec)
        {
            _sp_net_->PushTaskPost(a);
        }
        SetErrTips(QString("发送更新的配置文件接口个数: %1").arg(vec.size()),false);
    });

    connect(_sp_net_,&NetHandle::sn_connect_host,[=](int type){
        if(type == 1)
        {
            timer_count = 0;
            SetStatusHost(true);
        }
        else 
        {
            SetStatusHost(false);
        }
    });
    connect(_sp_net_,&NetHandle::sn_connect_sdk,[=](bool connect){
        SetStatusSdk(connect);
        if(connect == false)
        {
            SetStatusHost(false);    
        }
    });
    connect(_sp_net_,&NetHandle::sn_err_tips,[=](QString info){
        SetErrTips(info);
    });
}

main_window::~main_window()
{
}

void main_window::SetStatusSdk(bool ok)
{
    if(ok)
    {
        _but_connect->setEnabled(true);
        _lab_status_sdk->setText("[SDK连接中]");
    }
    else 
    {
        _but_connect->setEnabled(false);
        _lab_status_sdk->setText("[SDK已离线]");
    }
}

void main_window::SetStatusHost(bool ok)
{
    if(ok)
    {
        _lab_status_host->setText("[主机连接中]");
    }
    else 
    {
        _lab_status_host->setText("[主机已离线]");
    }
}

void main_window::SetErrTips(QString info,bool err)
{
    QString s;
    if(err)
    {
        s = "[错误提示: %1]";
    }
    else 
    {
        s = "[状态: %1]";
    }
    s = s.arg(info);
    _lab_err_tips->addItem(s);
}

void main_window::timerEvent(QTimerEvent *event)
{
    if(timer_id = event->timerId())
    {
        timer_count++;
        if(timer_count >= 3)
        {
            SetStatusHost(false);
        }
    }
}
