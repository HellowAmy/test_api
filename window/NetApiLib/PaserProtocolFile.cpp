
#include "PaserProtocolFile.h"

PaserProtocolFile::PaserProtocolFile(QObject *parent) : QObject(parent)
{
    


    _file_config = "../config/protocol.json";
    ReloadFile();
}

PaserProtocolFile::~PaserProtocolFile()
{
}

void PaserProtocolFile::ReloadFile()
{
    QFile f(_file_config);
    if(f.open(QIODevice::ReadOnly))
    {
        QString str = f.readAll();
        UtilFunc::ParseJson(str.toStdString().c_str(), [&](Json::Value &value) {
            _sjson = value;            
            flogd("Read Protocol: " << $(UtilFunc::WriteJsonFormat(value)));   
        });
        f.close();
    }
    flogd("== ReloadFile ==");
}

Json::Value PaserProtocolFile::GetApiData(QString api)
{
    for(int i=0;i<_sjson["ApiList"].size();i++)
    {
        if(_sjson["ApiList"][i]["api"] == api.toStdString())
        {
            return _sjson["ApiList"][i]["data"];
        }
    }
    return Json::Value();
}

Json::Value PaserProtocolFile::GetApiConfig(QString api)
{
    for(int i=0;i<_sjson["ApiList"].size();i++)
    {
        if(_sjson["ApiList"][i]["api"] == api.toStdString())
        {
            return _sjson["ApiList"][i];
        }
    }
    return Json::Value();
}

std::vector<Json::Value> PaserProtocolFile::GetUpdateApiList()
{
    std::vector<Json::Value> vec;
    for(int i=0;i<_sjson["ApiList"].size();i++)
    {
        if(_sjson["ApiList"][i]["update"].asBool())
        {
            vec.push_back(_sjson["ApiList"][i]);
        }
    }
    return vec;
}
