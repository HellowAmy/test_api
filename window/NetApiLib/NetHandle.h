
#ifndef NETHANDLE_H
#define NETHANDLE_H

#include "UtilFunc.h"
#include "UtilMacro.h"
#include <QObject>
#include "Tvlog.h"
#include "Network.h"
#include <memory>
#include <vector>

class NetHandle : public QObject
{
    Q_OBJECT
public:
    NetHandle(QObject *parent = nullptr);
    ~NetHandle();

    void SendToApiPost(QString api,Json::Value data);

    void SendToApiGet(QString api,Json::Value data);

    void PushTaskPost(Json::Value config);

    void ResetTask();

    int GetCountSend();

signals:
    void sn_connect_host(int type);

    void sn_connect_sdk(bool connect);

    void sn_err_tips(QString info);

    void sn_timer_num(int type);

    void sn_reset_count(int type);

protected:
    int _countSend = 0;
    std::vector<QTimer*> _vecTask;

private:

};

static NetHandle *_sp_net_ = Tsingle_d<NetHandle>::get();


#endif // NETHANDLE_H
