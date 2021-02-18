/** @file
 * The header file of the RND_Utils library.
 */

#ifndef RND_UTILS_H
#define RND_UTILS_H

/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/// Returns wall time since the Epoch in microseconds.
double RND_getWallTime_usec();
/// Returns CPU time in microseconds.
double RND_getCpuTime_usec();

#endif /* RND_UTILS_H */
