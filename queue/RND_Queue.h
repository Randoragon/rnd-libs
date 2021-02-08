/** @file
 * The header file of the RND_Queue library.
 *
 * @example queue/example.c
 * Here's an example usage of the RND_Queue library.
 */

#ifndef RND_QUEUE_H
#define RND_QUEUE_H

#include <stdlib.h>

/********************************************************
 *                      STRUCTURES                      *
 ********************************************************/

/// @cond
typedef struct RND_Queue RND_Queue;
/// @endcond

/** A FIFO queue structure for arbitrary data.
 *
 * The implementation is a dynamic array of void pointers
 * (ordered first to last). In order to ensure enqueue and
 * dequeue operations are O(1), the @ref RND_Queue::data
 * block is treated like a contiguous loop (as elements
 * get dequeued, the start of the array is emptied, and
 * new values may loop around to the start of the array
 * when being enqueued). The downside of this approach is
 * the necessity of rearranging the entire queue when
 * increasing its capacity.
 */
struct RND_Queue
{
    /// An array of pointers to the stored data.
    void **data;
    /// The number of elements on the queue.
    size_t size;
    /// The address of the first element.
    void **head;
    /// The address of the last element.
    void **tail;
    /// The size of the @ref RND_Queue::data array (this
    /// number will change dynamically).
    size_t capacity;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty queue and returns its pointer.
 *
 * @param[in] capacity The number of elements to preallocate
 * for (cannot be 0). This number will be automatically doubled each
 * time more space is needed due to pushing elements.
 * @returns
 * - the new queue's address - success
 * - @c NULL - insufficient memory or invalid @p capacity
 *   value
 */
RND_Queue *RND_queueCreate(size_t capacity);

/** Appends an element to the end of a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] data A pointer to the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - malloc failed (insufficient memory)
 */
int RND_queuePush(RND_Queue *queue, const void *data);

/** Returns a pointer to the front element of a queue.
 *
 * This function exists only for the sake of library
 * completeness, but you may use @ref RND_Queue::head
 * with the exact same effect.
 *
 * @param[inout] queue A pointer to the queue.
 * @returns
 * - @ref RND_Queue::head - success
 * - @c NULL - @p queue is @c NULL
 */
void *RND_queuePeek(const RND_Queue *queue);

/** Removes the front element from a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Queue::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_queuePop(RND_Queue *queue, int (*dtor)(const void*));

/** Removes an element from a queue by index.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] index The index of the element to remove (starting at head = 0).
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Queue::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_queueRemove(RND_Queue *queue, size_t index, int (*dtor)(const void*));

/** Removes all elements from a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Queue::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_queueClear(RND_Queue *queue, int (*dtor)(const void*));

/** Frees all memory associated with a queue.
 *
 * First frees the contents of a queue with @ref
 * RND_queueClear, then frees the @ref RND_Queue
 * struct itself.
 *
 * @param[inout] queue A pointer to the queue.
 * @param [in] dtor This argument is passed directly
 * to @ref RND_queueClear.
 */
int RND_queueDestroy(RND_Queue *queue, int (*dtor)(const void*));

/** Returns the number of elements in a queue.
 *
 * This function exists only for the sake of library
 * completeness, but you may use @ref RND_Queue::size
 * with the exact same effect.
 *
 * @param[in] queue A pointer to the queue.
 * @returns
 * - the size of the queue (@ref RND_Queue::size) - success
 * - 0 - if @p queue is @c NULL (or queue is empty)
 */
size_t RND_queueSize(RND_Queue *queue);

/** Prints the contents of a queue
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a queue. Its only applicable
 * use is probably debugging.
 *
 * @param[in] queue A pointer to the queue.
 */
int RND_queuePrint(RND_Queue *queue);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a queue are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_queuePop, @ref RND_queueClear or @ref
 * RND_queueDestroy.
 *
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_queueDtorFree(const void *data);

/** Creates a copy of a queue.
 *
 * Whether the copy is shallow or deep is entirely
 * dependent on the @p cpy function.
 *
 * There is no cleanup before returning error code 3+, so
 * in order to safely recover without leaking memory,
 * it is necessary to call @ref RND_queueDestroy on @p dest.
 *
 * @param[out] dest An empty container for the copy. This
 * has to point to an allocated block of memory, but it
 * cannot be an initialized queue, or else memory will leak
 * (i.e. use @c malloc, but not @ref RND_queueCreate).
 * @param[in] src A pointer to the hashmap to copy to @p dest.
 * @param[inout] cpy A pointer to a function which intakes
 * a @ref RND_Queue::data element and copies it, returning
 * the address of the copy or @c NULL for failure.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 * - 2 - one of @p src and @p dest is @c NULL
 * - 3+ - @p cpy returned error
 */
int RND_queueCopy(RND_Queue *dest, const RND_Queue *src, void* (*cpy)(const void *));

#endif /* RND_QUEUE_H */
