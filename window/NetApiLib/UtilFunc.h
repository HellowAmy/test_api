#ifndef UTILFUNC_H
#define UTILFUNC_H
#include "json.h"
#include <cmath>
#include <functional>
#include <qbytearray.h>
#include <qobject.h>
#include <qstring.h>
#include <string>
#include "Tvlog.h"
namespace UtilFunc
{
inline std::string WriteJson(const Json::Value &value)
{
    Json::FastWriter writer;
    return writer.write(value);
}

inline std::string WriteJsonFormat(const Json::Value &value)
{
    Json::StreamWriterBuilder writerBuilder;
    return Json::writeString(writerBuilder, value);
}

inline QString WriteJsonFormatQStr(const Json::Value &value)
{
    Json::StreamWriterBuilder writerBuilder;
    return QString::fromStdString(Json::writeString(writerBuilder, value));
}

inline auto ParseJson(const char *jsonData, std::function<void(Json::Value &)> callback)
{
    struct
    {
        bool _fail = false;
        void Error(std::function<void()> callback)
        {
            
            if (_fail && callback)
            {
                callback();
            }
        }
    } ret;
    try
    {
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(jsonData, value))
        {
            callback(value);
        }
        else
        {
            throw "";
        }
    }
    catch (...)
    {
        ret._fail = true;
        flogd("ParseJson Error: " << $(jsonData));
    }
    return ret;
}
inline auto ParseJson(const QByteArray &jsonData, std::function<void(Json::Value &)> callback)
{
    return ParseJson(jsonData.data(), callback);
}
inline auto ParseJson(const QString &jsonData, std::function<void(Json::Value &)> callback)
{
    return ParseJson(jsonData.toUtf8().data(), callback);
}
inline auto ParseJson(const std::string &jsonData, std::function<void(Json::Value &)> callback)
{
    return ParseJson(jsonData.c_str(), callback);
}

template <typename T> inline void waitAll(T &seq)
{
    for (auto &i : seq)
    {
        if (i.joinable())
        {
            i.join();
        }
    }
}

template <typename SignalType, typename SIGNAL, typename ReciverType, typename SlotType, typename Ret,
          typename... ArgsType>
auto connectNs(SignalType *sender, SIGNAL signal, ReciverType *receiver, Ret (SlotType::*slot)(ArgsType...),
               Qt::ConnectionType connectionType = Qt::AutoConnection)
{
    return QObject::connect(
        sender, signal, receiver,
        [receiver, slot](ArgsType... args) {
            receiver->blockSignals(true);
            (receiver->*slot)(args...);
            receiver->blockSignals(false);
        },
        connectionType);
}

template <typename SignalType, typename SIGNAL, typename ReciverType, typename Cls, typename Ret, typename... ArgsType>
auto connectNs(SignalType *sender, SIGNAL signal, ReciverType *receiver, Cls obj, Ret (Cls::*)(ArgsType...) const,
               Qt::ConnectionType connectionType = Qt::AutoConnection)
{
    return QObject::connect(
        sender, signal, receiver,
        [receiver, obj](ArgsType... args) {
            receiver->blockSignals(true);
            obj(args...);
            receiver->blockSignals(false);
        },
        connectionType);
}

template <typename SignalType, typename SIGNAL, typename ReciverType, typename Functor>
auto connectNs(SignalType sender, SIGNAL signal, ReciverType receiver, Functor slot,
               Qt::ConnectionType connectionType = Qt::AutoConnection)
{
    return connectNs(sender, signal, receiver, slot, &Functor::operator(), connectionType);
}

template <bool flag> struct AssignHelper
{
};
template <> struct AssignHelper<true>
{
    static void Assign(Json::Int &attr, const Json::Value &value)
    {
        if (!value.isInt())
        {
            return;
        }
        attr = value.asInt();
    }
    static void Assign(Json::Int64 &attr, const Json::Value &value)
    {
        if (!value.isInt64())
        {
            return;
        }
        attr = value.asInt64();
    }
    static void Assign(Json::UInt &attr, const Json::Value &value)
    {
        if (!value.isUInt())
        {
            return;
        }
        attr = value.asUInt();
    }
    static void Assign(Json::UInt64 &attr, const Json::Value &value)
    {
        if (!value.isUInt64())
        {
            return;
        }
        attr = value.asUInt64();
    }
    // static void Assign(QString &attr, const Json::Value &value)
    // {
    //     if (!value.isString())
    //     {
    //         return;
    //     }
    //     attr = value.asQString();
    // }
    static void Assign(std::string &attr, const Json::Value &value)
    {
        if (!value.isString())
        {
            return;
        }
        attr = value.asString();
    }
    static void Assign(double &attr, const Json::Value &value)
    {
        if (!value.isDouble())
        {
            return;
        }
        attr = value.asDouble();
    }
    static void Assign(float &attr, const Json::Value &value)
    {
        if (!value.isDouble())
        {
            return;
        }
        attr = value.asFloat();
    }
    static void Assign(bool &attr, const Json::Value &value)
    {
        if (!value.isBool())
        {
            return;
        }
        attr = value.asBool();
    }
};

template <> struct AssignHelper<false>
{
    template <typename T> static void Assign(T &&attr, Json::Value &value)
    {
        value = attr;
    }
};
template <bool flag, typename T1, typename T2> void Assign(T1 &&attr, T2 &&value)
{
    AssignHelper<flag>::Assign(attr, value);
}
inline double EuclideanDistance(double x, double y)
{
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}
inline double ConvertToDecimal(long long int n)
{
    double ret = n;
    while (ret >= 1 || ret <= -1)
    {
        ret /= 10.0;
    }
    return ret;
}

template <typename Func, typename... ArgTypes> auto bind(Func &&func, ArgTypes &&...args)
{
    return [=]() { return func(args...); };
}

};     // namespace UtilFunc
#endif // UTILFUNC_H