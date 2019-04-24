/** @file common.h
 *  @brief Common macro definitions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef COMMON_H
#define COMMON_H

#define INIT_CLEAR (0)
#define BOOLEAN_TRUE (1)
#define BOOLEAN_FALSE (0)
#define NULL_CHAR (0)
#define SINGLE_INCREMENT (1)

#define enum_len(MAX_VAL) (MAX_VAL + 1)
#define pointerdiff2len(x) (x + 1)
#define max_str_len(MAX_VAL) (MAX_VAL - 1)  // prevents null overwrite
#define MIN(a, b) ((a < b) ? a : b)

typedef void (*VOID_FUNC_PTR)(void);

#endif /* COMMON_H */