#include "Encrypt.h"

#ifdef ENCRYPT_LIB_FOUND
#include <mbedtls/aes.h>
#include <mbedtls/base64.h>
#endif

#include <string.h>

#include "UtilMacro.h"
bool padding(Buffer src, size_t srcLen, PaddingType paddingType, size_t blockSize, ResultFunc callback)
{
    Buffer dst = nullptr;
    size_t dstLen = 0;
    blockSize = paddingType == PaddingType::PKCS5 ? 8 : blockSize;
    switch (paddingType)
    {
    case PaddingType::Zero: {
        dstLen = (srcLen + blockSize - 1) / blockSize * blockSize;
        dst = new BufferEle[dstLen];
        memset(dst, 0, dstLen);
        memcpy(dst, src, srcLen);
    }
    break;
    case PaddingType::PKCS5:
    case PaddingType::PKCS7: {
        dstLen = (srcLen / blockSize + 1) * blockSize;
        dst = new BufferEle[dstLen];
        memset(dst + srcLen, dstLen - srcLen, dstLen - srcLen);
        memcpy(dst, src, srcLen);
    }
    break;
    default: {
        dstLen = srcLen;
        dst = new BufferEle[dstLen];
        memcpy(dst, src, srcLen);
    }
    break;
    }
    callback(dst, dstLen);
    SAFE_DELETE_ARR(dst);
    return true;
}

bool unpadding(const Buffer src, size_t srcLen, PaddingType paddingType, size_t blockSize, ResultFunc callback)
{
    Buffer dst = nullptr;
    size_t dstLen = 0;
    switch (paddingType)
    {
    case PaddingType::PKCS5:
    case PaddingType::PKCS7: {
        dstLen = srcLen - src[srcLen - 1];
        dst = new BufferEle[dstLen];
        memcpy(dst, src, dstLen);
    }
    break;
    default: {
        dstLen = srcLen;
        dst = new BufferEle[dstLen];
        memcpy(dst, src, dstLen);
    }
    break;
    }
    callback(dst, dstLen);
    SAFE_DELETE_ARR(dst);
    return true;
}

bool base64encode(Buffer src, size_t srcLen, ResultFunc callback)
{
#ifndef ENCRYPT_LIB_FOUND
    callback(src, srcLen);
    return false;
#else
    Buffer dst = nullptr;
    size_t dstLen = 0;
    mbedtls_base64_encode(nullptr, dstLen, &dstLen, src, srcLen);
    dst = new BufferEle[dstLen];
    mbedtls_base64_encode(dst, dstLen, &dstLen, src, srcLen);
    callback(dst, dstLen);
    SAFE_DELETE_ARR(dst);
    return true;
#endif
}
bool base64decode(Buffer src, size_t srcLen, ResultFunc callback)
{
#ifndef ENCRYPT_LIB_FOUND
    callback(src, srcLen);
    return false;
#else
    Buffer dst = nullptr;
    size_t dstLen = 0;
    mbedtls_base64_decode(nullptr, dstLen, &dstLen, src, srcLen);
    dst = new BufferEle[dstLen];
    mbedtls_base64_decode(dst, dstLen, &dstLen, src, srcLen);
    callback(dst, dstLen);
    SAFE_DELETE_ARR(dst);
    return true;
#endif
}

bool aesCBCEncrypt(const Buffer src, size_t srcLen, const Buffer key, int keyLen, const Buffer iv, PaddingType pad,
                   ResultFunc callback)
{
#ifndef ENCRYPT_LIB_FOUND
    callback(src, srcLen);
    return false;
#else
    bool flag = false;
    padding(src, srcLen, pad == PaddingType::PKCS5 ? PaddingType::PKCS7 : pad, 16,
            [&](const Buffer afterPad, size_t afterPadLen) {
                Buffer dst = new BufferEle[afterPadLen];
                size_t dstLen = afterPadLen;
                Buffer tmpIv = new BufferEle[16];
                memcpy(tmpIv, iv, 16);
                do
                {
                    mbedtls_aes_context context;
                    mbedtls_aes_init(&context);
                    if (mbedtls_aes_setkey_enc(&context, key, keyLen))
                    {
                        break;
                    }
                    if (mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_ENCRYPT, afterPadLen, tmpIv, afterPad, dst))
                    {
                        break;
                    }
                    callback(dst, dstLen);
                    mbedtls_aes_free(&context);
                } while (false);
                SAFE_DELETE_ARR(dst);
                SAFE_DELETE_ARR(tmpIv);
                flag = true;
            });
    return flag;
#endif
}

bool aesCBCDecrypt(const Buffer src, size_t srcLen, const Buffer key, int keyLen, const Buffer iv, PaddingType pad,
                   ResultFunc callback)
{
#ifndef ENCRYPT_LIB_FOUND
    callback(src, srcLen);
    return false;
#else
    Buffer dst = new BufferEle[srcLen];
    size_t dstLen = srcLen;
    Buffer tmpIv = new BufferEle[16];
    bool flag = false;
    memcpy(tmpIv, iv, 16);
    do
    {
        mbedtls_aes_context context;
        mbedtls_aes_init(&context);
        if (mbedtls_aes_setkey_dec(&context, key, keyLen))
        {
            break;
        }
        if (mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_DECRYPT, srcLen, tmpIv, src, dst))
        {
            break;
        }
        flag = unpadding(dst, dstLen, pad, 16, callback);
        mbedtls_aes_free(&context);
    } while (false);
    SAFE_DELETE_ARR(dst);
    SAFE_DELETE_ARR(tmpIv);
    return flag;
#endif
}

bool aesCBCEncrypt128(Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback)
{
    return aesCBCEncrypt(src, srcLen, key, 128, iv, pad, callback);
}
bool aesCBCDecrypt128(Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback)
{
    return aesCBCDecrypt(src, srcLen, key, 128, iv, pad, callback);
}
bool aes128AndBase64En(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback)
{
    bool flag = false;
    aesCBCEncrypt128(src, srcLen, key, iv, pad,
                     [&](const Buffer dst, size_t dstLen) { flag = base64encode(dst, dstLen, callback); });
    return flag;
}
bool aes128AndBase64De(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback)
{
    bool flag = false;
    base64decode(src, srcLen, [&](const Buffer dst, size_t dstLen) {
        flag = aesCBCDecrypt128(dst, dstLen, key, iv, pad, callback);
    });
    return flag;
}

bool aesCBCEncrypt256(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback)
{
    return aesCBCEncrypt(src, srcLen, key, 256, iv, pad, callback);
}

bool aesCBCDecrypt256(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                      ResultFunc callback)
{
    return aesCBCDecrypt(src, srcLen, key, 256, iv, pad, callback);
}

bool aes256AndBase64En(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback)
{
    bool flag = false;
    aesCBCEncrypt256(src, srcLen, key, iv, pad,
                     [&](const Buffer dst, size_t dstLen) { flag = base64encode(dst, dstLen, callback); });
    return flag;
}

bool aes256AndBase64De(const Buffer src, size_t srcLen, const Buffer key, const Buffer iv, PaddingType pad,
                       ResultFunc callback)
{
    bool flag = false;
    base64decode(src, srcLen, [&](const Buffer dst, size_t dstLen) {
        flag = aesCBCDecrypt256(dst, dstLen, key, iv, pad, callback);
    });
    return flag;
}
