#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

/**
 * @brief word size = 1 in wramp architecture
 * this means sizeof(unsigned int) == sizeof(char) == 1
 * whereas sizeof(unsigned int) == 4, sizeof(char) == 1 in other architectures
 * This has implications on disk.c, where it is pre-compiled on other architectures
 * in fs/fsutil/fsutil.c, but used in wramp architecture.
 * Thus sizing conversion are required
 * 
 */

#define WORD_SIZE                       (sizeof(unsigned int))
#define TO_WORD_SIZE(x)                 (x / WORD_SIZE)
#define TO_CHAR_SIZE_FROM_WRAMP(x)      (x * WORD_SIZE)
#define ARCH_CHAR_SIZE_WRAMP(x)         (x /= 4)
#define DEARCH_CHAR_SIZE_WRAMP(x)       (x *= 4)

#endif
