#ifndef _ENCRYPT_H
#define _ENCRYPT_H
#include <functional>
#include <stddef.h>
using Buffer = unsigned char *;
using BufferEle = unsigned char;
using ResultFunc = std::function<void(const Buffer dst, size_t dstSize)>;
enum class PaddingType
{
    PKCS7,
    PKCS5,
    Zero,
    None,
};
bool padding(const Buffer src, size_t srcLen, PaddingType paddingType, size_t blockSize, ResultFunc callback);

bool unpadding(const Buffer src, size_t srcLen, PaddingType paddingType, size_t blockSize, ResultFunc callback);

bool base64encode(const Buffer src, size_t srcLen, ResultFunc callback);
bool base64decode(const Buffer src, size_t srcLen, ResultFunc callback);

bool aesCBCEncrypt(const Buffer src, size_t srcLen, const Buffer key, int keyLen, const Buffer iv, PaddingType pad,
                   ResultFunc callback);
bool aesCBCDecrypt(const Buffer src, size_t srcLen, const Buffer key, int keyLen, const Buffer iv, PaddingType pad,
                   ResultFunc callback);

bool aesCBCEncrypt128(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback);
bool aesCBCDecrypt128(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback);

bool aes128AndBase64En(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback);

bool aes128AndBase64De(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback);

bool aesCBCEncrypt256(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback);
bool aesCBCDecrypt256(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback);

bool aes256AndBase64En(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback);

bool aes256AndBase64De(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback);

#endif