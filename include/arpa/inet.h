/**
 * @file include/arpa/inet.h
 * @brief Définition des fonctions de base
 */
#ifndef ARPA_DEF
#define ARPA_DEF

static inline uint16_t htons(uint16_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __builtin_bswap16(x);
#else
    return x;
#endif
}

static inline uint16_t ntohs(uint16_t x)
{
    return htons(x);
}

static inline uint32_t htonl(uint32_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __builtin_bswap32(x);
#else
    return x;
#endif
}

static inline uint32_t ntohl(uint32_t x)
{
    return htonl(x);
}
#endif // ARPA_DEF
