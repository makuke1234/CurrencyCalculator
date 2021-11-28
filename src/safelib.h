#ifndef SAFELIB_H
#define SAFELIB_H

#ifdef __cplusplus
extern "C"
{
#endif




#include <stdint.h>

/**
 * @brief Finds the first occurrence of the byte string pointed to by substr
 * 
 * @param str pointer to the byte string to examine
 * @param maxCount maximum amount of characters to scan in str
 * @param substr pointer to the null-terminated byte string to search for
 * @return Pointer to the first character of the found substring in str, NULL,
 * if such substring is not found
 */
const char * strstr_s(const char * restrict str, const size_t maxCount, const char * const restrict substr);
/**
 * @brief Scans the byte string pointer to by dest for any character from the
 * null-terminated byte string pointed to by breakset
 * 
 * @param dest pointer to the byte string to be analyzed
 * @param maxCount maximum amount of characters to scan in dest
 * @param breakset pointer to the null-terminated byte string that contains the
 * characters to search for
 * @return Pointer to the first character in dest, that is also in breakset, NULL,
 * if no such character exists
 */
const char * strpbrk_s(const char * restrict dest, const size_t maxCount, const char * const restrict breakset);
/**
 * @brief Finds the first occurrence of ch in the byte string pointed to by str
 * 
 * @param str pointer to the byte string to be analyzed
 * @param maxCount maximum amount of characters to scan in str
 * @param ch character to search for
 * @return Pointer to the found character in str, NULL, if no such character is found
 */
const char * strchr_s(const char * restrict str, const size_t maxCount, const char ch);
/**
 * @brief Finds the last occurrence of ch in the byte string pointer to by str
 * 
 * @param str pointer to the byte string to be analyzed
 * @param maxCount maximum amount of characters to scan in str
 * @param ch character to search for
 * @return Pointer to the last found character in str, NULL, if no such character is found
 */
const char * strrchr_s(const char * restrict str, const size_t maxCount, const char ch);


#ifdef __cplusplus
}
#endif
#endif

