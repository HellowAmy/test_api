
#ifndef PASERPROTOCOLFILE_H
#define PASERPROTOCOLFILE_H

#include <QObject>
#include <QFile>
#include "UtilFunc.h"
#include "UtilMacro.h"
#include "Tvlog.h"


class PaserProtocolFile : public QObject
{
    Q_OBJECT
public:
    PaserProtocolFile(QObject *parent = nullptr);
    ~PaserProtocolFile();


    void ReloadFile();
    Json::Value GetApiData(QString api);
    Json::Value GetApiConfig(QString api);

    std::vector<Json::Value> GetUpdateApiList();

signals:

protected:
    QString _file_config;
    Json::Value _sjson;


private:
    
};

static PaserProtocolFile *_sp_protocol_ = Tsingle_d<PaserProtocolFile>::get();

#endif // PASERPROTOCOLFILE_H
