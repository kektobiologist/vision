/* This file contains common definitions and settings for Team Id Code
 * and their color codes and the number of Bots
 */

#ifndef COMDEF_H
#define COMDEF_H

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <winsock2.h>
#endif // WIN32

//**************** Common definitions ******************//
#define NOT_USED(x)      ((void)(x))

#define MINIMUM(a,b) ({typeof(a) _a = (a); typeof(b) _b = (b); (_a < _b) ? _a : _b;})
#define MAXIMUM(a,b) ({typeof(a) _a = (a); typeof(b) _b = (b); (_a > _b) ? _a : _b;})
#define ABSOLUTE(a)  ({typeof(a) _a = (a); (_a < 0) ? -_a : _a;})


namespace Strategy
{

} // namespace Strategy


#endif // COMDEF_H
