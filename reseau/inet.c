/**
 */
#include <arpa/inet.h>

static inline uint16 htons(uint16 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __builtin_bswap16(x);
#else
    return x;
#endif
}

static inline uint16 ntohs(uint16 x)
{
    return htons(x);
}

static inline uint32 htonl(uint32 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __builtin_bswap32(x);
#else
    return x;
#endif
}

static inline uint32 ntohl(uint32 x)
{
    return htonl(x);
}
