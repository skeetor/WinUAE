#pragma once

/*#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>*/

#define UNUSED(x) (void)(x)

// Makro to generate a string out of a makroparameter
// #define __DEFINE_TO_STRING(Test)  -> "Test"

#ifndef STRINGIFY
#define __STR2__(x) #x
#define STRINGIFY(x) __STR2__(x)
#endif

 /**
  * sizeof struct/class member.
  * Example: sizeof_member(mystruct, membername)
  */
#define sizeof_member(type, member) sizeof(((type *)0)->member)
