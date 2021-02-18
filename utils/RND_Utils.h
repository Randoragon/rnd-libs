/** @file
 * The header file of the RND_Utils library.
 */

#ifndef RND_UTILS_H
#define RND_UTILS_H

/********************************************************
 *                       MACROS                         *
 ********************************************************/

/// Returns the maximum of two numbers
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
/// Returns the minimum of two numbers
#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))
/** Clamps a value inbetween two numbers
 *
 * @param X the number to clamp
 * @param Y bottom boundary
 * @param Z top boundary
 */
#define CLAMP(X, Y, Z) (MAX((MIN((X), (Z))), (Y)))

/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/// Returns wall time since the Epoch in microseconds.
double RND_getWallTime_usec();
/// Returns CPU time in microseconds.
double RND_getCpuTime_usec();

#endif /* RND_UTILS_H */
