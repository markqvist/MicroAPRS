/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER

#include <cfg/compiler.h>

#if 0
#include <windows.h>
#else

/* These types must be 16-bit, 32-bit or larger integer */
typedef int             INT;
typedef unsigned        UINT;

/* These types must be 8-bit integer */
typedef int8_t          CHAR;
typedef uint8_t         UCHAR;
typedef uint8_t         BYTE;

/* These types must be 16-bit integer */
typedef int16_t         SHORT;
typedef uint16_t        USHORT;
typedef uint16_t        WORD;
typedef uint16_t        WCHAR;

/* These types must be 32-bit integer */
typedef int32_t         LONG;
typedef uint32_t        ULONG;
typedef uint32_t        DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOL;

#endif

#define _INTEGER
#endif
