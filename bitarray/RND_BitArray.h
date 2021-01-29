/** @file
 * The header file of the RND_BitArray library.
 *
 * @example bitarray/example.c
 * Here's an example usage of the RND_BitArray library.
 */

#ifndef RND_BITARRAY_H
#define RND_BITARRAY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 *                     STRUCTURES                       *
 ********************************************************/

/// @cond
typedef struct RND_BitArray RND_BitArray;
/// @endcond

/// A bitarray  for compactly storing boolean values.
struct RND_BitArray
{
    /** Holds the bitarray values
     *
     * Every 8-bit element of this array is treated as
     * 8 distinct bits when accessing the @ref RND_BitArray
     * struct with @ref RND_bitArrayGet, @ref RND_bitArraySet
     * and @ref RND_bitArrayToggle.
     */
    uint8_t *bits;
    /** Stores the size of the bitarray (in bits)
     *
     * The actual size of the @ref bits array is equal to
     * ((@ref size + 7) / 8), because the array consists of
     * 8-bit elements.
     *
     * Bitarrays with a @c (@ref size % 8 == 0) may outperform
     * bitarrays with @c (@ref size % 8 != 0) on certain bit operations:
     * - @ref RND_bitArrayAnd
     * - @ref RND_bitArrayOr
     * - @ref RND_bitArrayXor
     */
    size_t size;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new bitarray and returns its pointer.
 *
 * After creation, all bits within the struct are initialized
 * to 0 (false) by default.
 *
 * @param[in] size The size (in bits) of the new bitarray.
 * @returns
 * - the new bitarray's address - success
 * - @c NULL - insufficient memory
 */
RND_BitArray *RND_bitArrayCreate(size_t size);

/** Returns a selected bit's truth value.
 *
 * @param[in] bitarray A pointer to the bitarray.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @returns
 * - @c true - bit is set to true
 * - @c false - bit is set to false or @p index out of range
 * (if @p index is out of range, an appropriate error message is
 * printed to @c stderr)
 */
bool RND_bitArrayGet(const RND_BitArray *bitarray, size_t index);

/** Sets a selected bit to a chosen value.
 *
 * @param[inout] bitarray A pointer to the bitarray.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @param[in] value The value to set the bit to (@c true or @c false).
 * @returns 
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 * - 2 - @p index out of range
 */
int  RND_bitArraySet(RND_BitArray *bitarray, size_t index, bool value);

/** Sets the entire bitarray to a value denoted by a formatted string.
 *
 * It is often the case that the user wants to overwrite the state of the entire
 * bitarray, and doing it by calling @ref RND_bitArraySet for every single bit
 * is simply unfeasible. This function allows you to pass in a formatted string
 * which describes the target state for every bit in a bitarray.
 *
 * The functionality is best understood with examples:
 * @code
 * RND_BitArray *bits = RND_bitArrayCreate(32);
 *
 * // The below lines all do the same thing:
 * RND_bitArraySetf(bits, "0xDEADbeef");    // mixed-case hexadecimal
 * RND_bitArraySetf(bits, "033653337357");  // octal
 * RND_bitArraySetf(bits, "0b11011110101011011011111011101111"); // binary
 * @endcode
 *
 * ADDITIONAL FORMAT RULES:
 * - all spaces and tabs are ignored (e.g. you can write "0b1011 1001" or even " 0 x B  9")
 * - if string length exceedes bitarray size, leftmost extra bits are ignored
 * - if string length is shorter than bitarray size, the extra space is padded with 0s from the left
 *
 * @param[inout] bitarray A pointer to the bitarray.
 * @param[in] format A formatted string denoting the new bitarray.
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 * - 2 - invalid @p format string
 */
int  RND_bitArraySetf(RND_BitArray *bitarray, const char *format);

/** Toggles a selected bit to the opposite value.
 *
 * @param[inout] bitarray A pointer to the bitarray.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @returns 
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 * - 2 - @p index out of range
 */
int  RND_bitArrayToggle(RND_BitArray *bitarray, size_t index);

/** Frees all memory associated with a bitarray.
 *
 * This function should be called on every bitarray created by @ref
 * RND_bitArrayCreate once you no longer need it.
 *
 * @param[in] bitarray A pointer to the bitarray.
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 */
int  RND_bitArrayDestroy(RND_BitArray *bitarray);

/** Returns the size of a bitarray (in bits).
 *
 * @param[in] bitarray A pointer to the bitarray.
 *
 * @returns
 * - value > 0 - the size of the bitarray
 * - 0 - @p bitarray is a NULL-pointer
 */
size_t RND_bitArraySize(const RND_BitArray *bitarray);

/** Prints a comprehensive table of all values stored in a bitarray.
 *
 * This function is designed to be a convenient way to peek at the
 * contents of a bitarray. Its only applicable use is probably debugging.
 *
 * @param[in] bitarray A pointer to the bitarray.
 *
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 */
int  RND_bitArrayPrint(const RND_BitArray *bitarray);

/** Performs the logical AND operation on two bitarrays.
 *
 * The result of the operation will overwrite the contents of
 * @p dest. If the size of @p dest is greater than the size
 * of @p src, the remaining space is padded with 0s from the left.
 * If the size of @p dest is smaller than the size of @p src,
 * the excessive bits are ignored (also from the left).
 *
 * @param[inout] dest A pointer to the bitarray for storing the result.
 * @param[in] src A pointer to any other bitarray (if @c NULL, nothing happens).
 * @returns
 * - 0 - success
 * - 1 - @p dest is a NULL-pointer
 */
int  RND_bitArrayAnd(RND_BitArray *dest, const RND_BitArray *src);

/** Performs the logical OR operation on two bitarrays.
 *
 * The result of the operation will overwrite the contents of
 * @p dest. If the size of @p dest is greater than the size
 * of @p src, the remaining space is padded with 0s from the left.
 * If the size of @p dest is smaller than the size of @p src,
 * the excessive bits are ignored (also from the left).
 *
 * @param[inout] dest A pointer to the bitarray for storing the result.
 * @param[in] src A pointer to any other bitarray (if @c NULL, nothing happens).
 * @returns
 * - 0 - success
 * - 1 - @p dest is a NULL-pointer
 */
int  RND_bitArrayOr(RND_BitArray *dest, const RND_BitArray *src);

/** Performs the logical XOR operation on two bitarrays.
 *
 * The result of the operation will overwrite the contents of
 * @p dest. If the size of @p dest is greater than the size
 * of @p src, the remaining space is padded with 0s from the left.
 * If the size of @p dest is smaller than the size of @p src,
 * the excessive bits are ignored (also from the left).
 *
 * @param[inout] dest A pointer to the bitarray for storing the result.
 * @param[in] src A pointer to any other bitarray (if @c NULL, nothing happens).
 * @returns
 * - 0 - success
 * - 1 - @p dest is a NULL-pointer
 */
int  RND_bitArrayXor(RND_BitArray *dest, const RND_BitArray *src);

/** Applies negation to every bit in a bitarray.
 *
 * This is equivalent to running @ref RND_bitArrayToggle for every bit.
 *
 * @param[inout] bitarray A pointer to the bitarray.
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 */
int  RND_bitArrayNegate(RND_BitArray *bitarray);

#endif /* RND_BITARRAY_H */
