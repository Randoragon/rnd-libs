/** @file
 * The header file of the RND_QueueLL library.
 * This library is perfectly functional, although I don't
 * use it anymore because linked lists perform worse than
 * regular arrays on modern computers due to CPU caches.
 * That's why I set out to rewrite this library with an array
 * implementation and that's what I'll be using instead.
 *
 * @example queueLL/example.c
 * Here's an example usage of the RND_QueueLL library.
 */

#ifndef RND_QUEUE_LL_H
#define RND_QUEUE_LL_H

#include <stdlib.h>

/********************************************************
 *                      STRUCTURES                      *
 ********************************************************/

/// @cond
typedef struct RND_QueueLL RND_QueueLL;
/// @endcond

/** A FIFO queue structure for arbitrary data.
 *
 * The implementation is analogous to a linked list 
 * (hence the LL suffix) - the first structure points
 * to the 2nd, which points to the 3rd and so on.
 * The last element points to NULL.
 */
struct RND_QueueLL
{
    /// A pointer to the stored data.
    void *data;
    /// A pointer to the next queue element.
    RND_QueueLL *next;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty queue and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each queue element is an initialized
 * @ref RND_QueueLL struct, so an empty queue is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the queue,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all queue
 * functions intake @c RND_QueueLL** instead of @c
 * RND_QueueLL*).
 * @returns @c NULL
 */
RND_QueueLL *RND_queueLLCreate();

/** Appends an element to the end of a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_queueLLPush(RND_QueueLL **queue, const void *data);

/** Returns a pointer to the front element of a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns
 * - the front element's @ref RND_QueueLL::data - success
 * - @c NULL - the queue is empty
 */
void *RND_queueLLPeek(const RND_QueueLL **queue);

/** Removes the front element from a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_QueueLL::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_queueLLPop(RND_QueueLL **queue, int (*dtor)(const void*));

/** Removes an element from a queue by index.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] index The index of the element to remove (starting at front = 0).
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_QueueLL::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_queueLLRemove(RND_QueueLL **queue, size_t index, int (*dtor)(const void*));

/** Removes all elements from a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_QueueLL::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_queueLLClear(RND_QueueLL **queue, int (*dtor)(const void*));

/** Frees all memory associated with a queue.
 *
 * This is a wrapper for @ref RND_queueLLClear, because
 * due to implementation details, an empty queue does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_queueLLDestroy instead of @c
 * RND_queueLLClear when we mean to destroy it.
 */
int RND_queueLLDestroy(RND_QueueLL **queue, int (*dtor)(const void*));

/** Returns the number of elements in a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The number of elements in the queue.
 */
size_t RND_queueLLSize(const RND_QueueLL **queue);

/** Passes each element of a queue through a custom function.
 *
 * This function works much like a "for each" loop,
 * each element in the queue (in order front->back)
 * will be passed to the @p map function, allowing
 * it to do anything you like (for example changing
 * the data stored in each element, or printing all
 * elements).
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] map A pointer to a function which intakes
 * a queue element pointer (@c RND_QueueLL*) and
 * that element's index within the queue (@c size_t).
 * The function must return 0 for success, and any
 * other value for failure.
 * @returns
 * - 0 - success
 * - 1 - queue is empty or map function is @c NULL
 * - 2 - some @p map returned non-0 (this means that
 *   mapping the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_queueLLMap(RND_QueueLL **queue, int (*map)(RND_QueueLL*, size_t));

/** Prints the contents of a queue
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a queue. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_queueLLMap
 * with the @p map parameter set to @ref RND_queueLLPrintMap.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The value returned by @ref RND_queueLLMap
 * (using @p queue and @ref RND_queueLLPrintMap as
 * parameters).
 */
int RND_queueLLPrint(const RND_QueueLL **queue);

/** Prints a single queue element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_queueLLPrint, in conjunction with @ref
 * RND_queueLLMap.
 */
int RND_queueLLPrintMap(const RND_QueueLL *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a queue are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_queueLLPop, @ref RND_queueLLClear or @ref
 * RND_queueLLDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_queueLLDtorFree(const void *data);

#endif /* RND_QUEUE_LL_H */
