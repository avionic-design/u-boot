#ifndef LPC32XX_SPL_H
#define LPC32XX_SPL_H 1

#ifdef CONFIG_SPL_EARLY_DEBUG
extern void printascii(const char *string);
extern void printhex(uint32_t value, unsigned int digits);
#else
static inline void printascii(const char *string)
{
}

static inline void printhex(uint32_t value, unsigned int digits)
{
}
#endif

#endif /* LPC32XX_SPL_H */
