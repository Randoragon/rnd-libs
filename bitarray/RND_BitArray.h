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

/// A bitarray (also called bitarray) for compactly storing boolean values.
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
     * (@ref size / 8), because the array consists of
     * 8-bit elements.
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
 * @param[in] bitarray A pointer to an initialized @ref RND_BitArray struct.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @returns
 * - @c true - bit is set to true
 * - @c false - bit is set to false or @p index out of range
 * (if @p index is out of range, an appropriate error message is
 * printed to @c stderr)
 */
bool RND_bitArrayGet(RND_BitArray *bitarray, size_t index);

/** Sets a selected bit to a chosen value.
 *
 * @param[inout] bitarray A pointer to an initialized @ref RND_BitArray struct.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @param[in] value The value to set the bit to (@c true or @c false).
 * @returns 
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 * - 2 - @p index out of range
 */
int  RND_bitArraySet(RND_BitArray *bitarray, size_t index, bool value);

/** Toggles a selected bit to the opposite value.
 *
 * @param[inout] bitarray A pointer to an initialized @ref RND_BitArray struct.
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
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 */
int  RND_bitArrayDestroy(RND_BitArray *bitarray);

/** Returns the size of a bitarray (in bits).
 *
 * @param[in] bitarray A pointer to an initialized @ref RND_BitArray struct.
 *
 * @returns
 * - value > 0 - the size of the bitarray
 * - 0 - @p bitarray is a NULL-pointer
 */
size_t RND_bitArraySize(RND_BitArray *bitarray);

/** Prints a comprehensive table of all values stored in a bitarray.
 *
 * This function is designed to be a convenient way to peek at the
 * contents of a bitarray. Its only applicable use is probably debugging.
 *
 * @param[in] bitarray A pointer to an initialized @ref RND_BitArray struct.
 *
 * @returns
 * - 0 - success
 * - 1 - @p bitarray is a NULL-pointer
 */
int  RND_bitArrayPrint(RND_BitArray *bitarray);

#endif