#ifndef _UTILMACRO_H
#define _UTILMACRO_H
#include "UtilFunc.h"
#include <assert.h>
#include <cmath>
#include <type_traits>
#define SAFE_DELETE(x)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((x) != nullptr)                                                                                            \
            delete (x);                                                                                                \
        (x) = nullptr;                                                                                                 \
    } while (0)
#define SAFE_DELETE_ARR(x)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((x) != nullptr)                                                                                            \
            delete[] (x);                                                                                              \
        (x) = nullptr;                                                                                                 \
    } while (0)

#ifndef MIN
#define MIN(x, y) (x) < (y) ? (x) : (y)
#endif

#ifndef MAX
#define MAX(x, y) (x) > (y) ? (x) : (y)
#endif

#define ASSERT_LOG(expr, log)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            qCritical() << (log);                                                                                      \
            assert(false);                                                                                             \
        }                                                                                                              \
    } while (false)

#define DECL_SINGLE(Class)                                                                                             \
  private:                                                                                                             \
    Class();                                                                                                           \
    virtual ~Class();                                                                                                  \
    static Class *_that;                                                                                               \
                                                                                                                       \
  public:                                                                                                              \
    static Class *instance();                                                                                          \
    static void uninstance();                                                                                          \
                                                                                                                       \
  private:

#define DEF_SINGLE(Class)                                                                                              \
    Class *Class::_that;                                                                                               \
    Class *Class::instance()                                                                                           \
    {                                                                                                                  \
        if (_that == nullptr)                                                                                          \
        {                                                                                                              \
            _that = new Class;                                                                                         \
        }                                                                                                              \
        return _that;                                                                                                  \
    }                                                                                                                  \
    void Class::uninstance()                                                                                           \
    {                                                                                                                  \
        SAFE_DELETE(_that);                                                                                            \
    }

#define rmref(t) typename std::remove_reference<t>::type

#define BEGIN_TIMING auto ___begin = std::chrono::high_resolution_clock::now();

#define END_TIMING                                                                                                     \
    auto ___end = std::chrono::high_resolution_clock::now();                                                           \
    qDebug() << "timing is " << std::chrono::duration_cast<std::chrono::milliseconds>(___end - ___begin).count()       \
             << " milliseconds";

#define BtnClickCall(btn, ...)                                                                                         \
    QObject::connect((btn), &std::remove_reference<decltype(*(btn))>::type::clicked, __VA_ARGS__)

#define SelfType std::remove_reference<decltype(*this)>::type
#endif //_UTILMACRO_H
