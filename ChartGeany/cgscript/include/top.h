#line 1 "prepend"
/* top.h
   Prepended source to CGScript programs

   Copyright (c) Lucas Tsatiris 2016-2020
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   3. Neither the name of the copyright holder nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

/*
    Preceded underscore indicates variables, functions and definitions for
    internal use
 */

/* Instruct gcc to use msc structure packing when we compile modules
   using gcc or clang */


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wkeyword-macro"
#endif

#ifndef _CGSCRIPT_VERSION

/* Language Version */
#define _CGSCRIPT_VERSION       0x00010004 /* 1.4 */
#define _CGSCRIPT_VERSION_STR   "1.4 (beta)"

/* CGTOOL */
#ifndef CGTOOL

/*** Custom types and definitions ***/
/* TLS definition */
#if (defined(__PCC__) || (__GNU_MAJOR__<4) || \
    ((__GNU_MAJOR__==4) && (__GNU_MINOR__<3))) && \
    !defined (__clang__)
#define ModuleLocal static
#else
#define ModuleLocal static __thread
#endif

/* Module property definition */
#ifdef __PCC__
typedef char        Property;
#else
typedef const char  Property;
#endif /* __PCC__ */

/* Object handler */
typedef void * _ObjectHandler_t;
#ifdef _CGSCRIPT_MODULE
static void _ObjectCleanup (_ObjectHandler_t *obj);
#define ObjectHandler_t  __attribute__ ((__cleanup__(_ObjectCleanup))) _ObjectHandler_t
#else
typedef _ObjectHandler_t ObjectHandler_t;
#endif

/* Event types */
typedef enum
{
  EV_MOUSE_ON_BAR = 1,      /* mouse pointer changed bar */
  EV_INPUT_VAR              /* input variable changed */
} Event_t;

/* Object types */
typedef enum
{
  LABEL_OBJECT,             /* label                */
  TEXT_OBJECT,              /* text                 */
  VLINE_OBJECT,             /* verical              */
  HLINE_OBJECT,             /* horizontial          */
  LINE_OBJECT,              /* trend line           */
  _CHANNEL_OBJECT,          /* channel (not used)   */
  FIBO_OBJECT,              /* fibonacci            */
  SUBCHART_OBJECT,          /* subchart object      */
  VBARS_OBJECT,             /* vertical bars        */
  CURVE_OBJECT,             /* curve                */
  DOT_OBJECT,               /* dot                  */
  CONTAINER_OBJECT          /* container            */
} Object_t;

/* Price types */
typedef enum
{
  OPEN_PRICE,               /* open price           */
  CLOSE_PRICE,              /* close price          */
  HIGH_PRICE,               /* high price           */
  LOW_PRICE                 /* low price            */
} Price_t;

/* Error code types */
typedef enum
{
  CGSERR_OK = -4999,                    /* no error */
  CGSERR_INITIALIZATION_FAILED,         /* initialization failed */
  CGSERR_RUNTIME_ERROR,                 /* run-time error */
  CGSERR_UNINITIALIZED,                 /* uninitialized module */
  CGSERR_ALREADY_INITIALIZED,           /* module is initialized */
  CGSERR_NO_PARENT_CHART,               /* no parent */
  CGSERR_NO_MODULE,                     /* no module */
  CGSERR_NO_ARRAY,                      /* no array */
  CGSERR_ARRAY_CREATION_AFTER_INIT,     /* array creation after Init() execution */
  CGSERR_ARRAY_ELEM_BIG,                /* array element too big */
  CGSERR_ARRAY_ELEM_ZERO,               /* array element size is zero */
  CGSERR_ARRAY_CREATION_FAILED,         /* array creation failed */
  CGSERR_ARRAY_SUBSCRIPT,               /* array subscript out of range */
  CGSERR_MEMORY,                        /* out of memory */
  CGSERR_TIMEFRAME,                     /* invalid time frame */
  CGSERR_INVALID_ARGUMENT,              /* invalid argument */
  CGSERR_NEGATIVE_SHIFT,                /* negative shift */
  CGSERR_INVALID_MODTYPE,               /* invalid module type */
  CGSERR_INTERNAL_100,                  /* internal error 100 */
  CGSERR_INTERNAL_101,                  /* internal error 101 */
  CGSERR_FPE,                           /* floating point exception */
  CGSERR_SEGV,                          /* segmentation fault */
  CGSERR_ILL,                           /* illegal instruction */
  CGSERR_SIGNAL                         /* signal not handled */
} ErrCode_t;

/* Text adjustment types */
typedef enum
{
  TEXT_VADJUST_NORMAL,
  TEXT_VADJUST_CENTER,
  TEXT_VADJUST_ABOVE,
  TEXT_VADJUST_BELOW,
  TEXT_HADJUST_NORMAL,
  TEXT_HADJUST_CENTER,
  TEXT_HADJUST_LEFT,
  TEXT_HADJUST_RIGHT
} TextAdjustment_t;

/* Time frame types */
typedef enum
{
  TF_DAY,                   /* daily */
  TF_WEEK,                  /* weekly */
  TF_MONTH,                 /* monthly */
  TF_YEAR                   /* yearly */
} TimeFrame_t ;

/* Candle types */
typedef enum
{
  CTYPE_CANDLE,             /* standard candle */
  CTYPE_HEIKINASHI          /* heikin-ashi */
} Candle_t;

/* Bar data types */
typedef struct
{
  double High;              /* high */
  double Low;               /* low */
  double Open;              /* open */
  double Close;             /* close */
  double AdjClose;          /* adjusted close */
  double Volume;            /* volume */
  unsigned short Year;      /* year */
  unsigned short Month;     /* month */
  unsigned short Day;       /* day */
  char Date[16];            /* full date */
  char Time[16];            /* time */
  char Id[256];             /* bar id */
} BarData_t;

/* Array type */
typedef void * _Array_t;

#ifdef _CGSCRIPT_MODULE
static void _ArrayCleanup (_Array_t *arr);
#define Array_t  __attribute__ ((__cleanup__(_ArrayCleanup))) _Array_t
#else
typedef _Array_t Array_t;
#endif

/* String type */
typedef void * _String_t;

#ifdef _CGSCRIPT_MODULE
static void _StringCleanup (_String_t *str);
#define String_t  __attribute__ ((__cleanup__(_StringCleanup))) _String_t
#else
typedef _String_t String_t;
#endif

/* Color type */
typedef unsigned int Color_t;
#define COLOR_WHITE         4294967295
#define COLOR_BLACK         4278190080
#define COLOR_RED           4294901760
#define COLOR_DARKRED       4289331200
#define COLOR_GREEN         4278255360
#define COLOR_DARKGREEN     4278233600
#define COLOR_BLUE          4278190335
#define COLOR_DARKBLUE      4278190208
#define COLOR_CYAN          4278255615
#define COLOR_DARKCYAN      4278222976
#define COLOR_MAGENTA       4294902015
#define COLOR_DARKMAGENTA   4286578816
#define COLOR_YELLOW        4294967040
#define COLOR_DARKYELLOW    4286611456
#define COLOR_GRAY          4288716964
#define COLOR_DARKGRAY      4286611584
#define COLOR_LIGHTGRAY     4290822336

/* Font weight type */
typedef int FontWeight_t;
static const FontWeight_t
  FONTWEIGHT_LIGHT      = 25,
  FONTWEIGHT_NORMAL     = 50,
  FONTWEIGHT_DEMIBOLD   = 63,
  FONTWEIGHT_BOLD       = 75,
  FONTWEIGHT_BLACK      = 87;

/* Symbols  */
static const char
  UPWARDS_ARROW[] = "&#8593;",
  DOWNWARDS_ARROW[] = "&#8595;",
  UPWARDS_WHITE_ARROW[] = "&#8679;",
  DOWNWARDS_WHITE_ARROW[] = "&#8681;",
  WHITE_UP_POINTING_INDEX[] = "&#9757;",
  WHITE_DOWN_POINTING_INDEX[] = "&#9759;";

/* Input variable name size */
#define _VAR_NAME_SIZE      63

/* Input variable types */
typedef enum
{
  _VAR_INTEGER,             /* integer variable */
  _VAR_DOUBLE,              /* double variable */
  _VAR_COLOR                /* color variable */
} _Variable_t;

/* Input variable registry record */
typedef struct
{
  double        defaultValue;   /* default value */
  double        minValue;       /* minimum value */
  double        maxValue;       /* maximum value */
  Color_t       *colorVar;      /* color variable */
  double        *doubleVar;     /* double variable */
  int           *intVar;        /* integer variable */
  _Variable_t   type;           /* variable type */
  int           show;           /* variable appears after module's name
                                   in parenthesis (int vars only) */
  char          name[_VAR_NAME_SIZE + 1];   /* variable name */
} _InputVariableRegistryRec;

#ifdef _CGSCRIPT_MODULE /* Module compilation */

/*** stddef.h ***/
#ifndef NULL
#define NULL ((void*)0)
#endif

typedef __SIZE_TYPE__       size_t;
typedef __PTRDIFF_TYPE__    ssize_t;
typedef __WCHAR_TYPE__      wchar_t;
typedef __PTRDIFF_TYPE__    ptrdiff_t;
typedef __PTRDIFF_TYPE__    intptr_t;
typedef __SIZE_TYPE__       uintptr_t;

#define offsetof(type, field) ((size_t)&((type *)0)->field)

/*** ISO C99 stdint.h definitions and types ***/
#ifdef __INT8_TYPE__
typedef __INT8_TYPE__           int8_t;
#else
typedef signed char             int8_t;
#endif

#ifdef __INT16_TYPE__
typedef __INT16_TYPE__          int16_t;
#else
typedef short int               int16_t;
#endif

#ifdef __INT32_TYPE__
typedef __INT32_TYPE__          int32_t;
#else
typedef int                     int32_t;
#endif

#ifdef __INT64_TYPE__
typedef __INT64_TYPE__          int64_t;
#else
#if __WORDSIZE == 64
typedef long int                int64_t;
#else
typedef long long int           int64_t;
#endif
#endif

#ifdef __UINT8_TYPE__
typedef __UINT8_TYPE__          uint8_t;
#else
typedef unsigned char           uint8_t;
#endif

#ifdef __UINT16_TYPE__
typedef __UINT16_TYPE__         uint16_t;
#else
typedef unsigned short int      uint16_t;
#endif

#ifdef __UINT32_TYPE__
typedef __UINT32_TYPE__         uint32_t;
#else
typedef unsigned int            uint32_t;
#endif

#ifdef __UINT64_TYPE__
typedef __UINT64_TYPE__         uint64_t;
#else
#if __WORDSIZE == 64
typedef unsigned long int       uint64_t;
#else
typedef unsigned long long int  uint64_t;
#endif
#endif

/*** ISO C99 inttypes.h definitions and types ***/
#if __WORDSIZE == 64
#define __PRI64_PREFIX    "l"
#define __PRIPTR_PREFIX   "l"
#else
#define __PRI64_PREFIX    "ll"
#define __PRIPTR_PREFIX
#endif

/* Macros for printing format specifiers.  */

/* Decimal notation.  */
#define PRId8      "d"
#define PRId16     "d"
#define PRId32     "d"
#define PRId64     __PRI64_PREFIX "d"

#define PRIdLEAST8 "d"
#define PRIdLEAST16    "d"
#define PRIdLEAST32    "d"
#define PRIdLEAST64    __PRI64_PREFIX "d"

#define PRIdFAST8  "d"
#define PRIdFAST16 __PRIPTR_PREFIX "d"
#define PRIdFAST32 __PRIPTR_PREFIX "d"
#define PRIdFAST64 __PRI64_PREFIX "d"

#define PRIi8      "i"
#define PRIi16     "i"
#define PRIi32     "i"
#define PRIi64     __PRI64_PREFIX "i"

#define PRIiLEAST8 "i"
#define PRIiLEAST16    "i"
#define PRIiLEAST32    "i"
#define PRIiLEAST64    __PRI64_PREFIX "i"

#define PRIiFAST8  "i"
#define PRIiFAST16 __PRIPTR_PREFIX "i"
#define PRIiFAST32 __PRIPTR_PREFIX "i"
#define PRIiFAST64 __PRI64_PREFIX "i"

/* Octal notation.  */
#define PRIo8      "o"
#define PRIo16     "o"
#define PRIo32     "o"
#define PRIo64     __PRI64_PREFIX "o"

#define PRIoLEAST8 "o"
#define PRIoLEAST16    "o"
#define PRIoLEAST32    "o"
#define PRIoLEAST64    __PRI64_PREFIX "o"

#define PRIoFAST8  "o"
#define PRIoFAST16 __PRIPTR_PREFIX "o"
#define PRIoFAST32 __PRIPTR_PREFIX "o"
#define PRIoFAST64 __PRI64_PREFIX "o"

/* Unsigned integers.  */
#define PRIu8      "u"
#define PRIu16     "u"
#define PRIu32     "u"
#define PRIu64     __PRI64_PREFIX "u"

#define PRIuLEAST8 "u"
#define PRIuLEAST16    "u"
#define PRIuLEAST32    "u"
#define PRIuLEAST64    __PRI64_PREFIX "u"

#define PRIuFAST8  "u"
#define PRIuFAST16 __PRIPTR_PREFIX "u"
#define PRIuFAST32 __PRIPTR_PREFIX "u"
#define PRIuFAST64 __PRI64_PREFIX "u"

/* Lowercase hexadecimal notation.  */
#define PRIx8      "x"
#define PRIx16     "x"
#define PRIx32     "x"
#define PRIx64     __PRI64_PREFIX "x"

#define PRIxLEAST8 "x"
#define PRIxLEAST16    "x"
#define PRIxLEAST32    "x"
#define PRIxLEAST64    __PRI64_PREFIX "x"

#define PRIxFAST8  "x"
#define PRIxFAST16 __PRIPTR_PREFIX "x"
#define PRIxFAST32 __PRIPTR_PREFIX "x"
#define PRIxFAST64 __PRI64_PREFIX "x"

/* Uppercase hexadecimal notation.  */
#define PRIX8      "X"
#define PRIX16     "X"
#define PRIX32     "X"
#define PRIX64     __PRI64_PREFIX "X"

#define PRIXLEAST8 "X"
#define PRIXLEAST16    "X"
#define PRIXLEAST32    "X"
#define PRIXLEAST64    __PRI64_PREFIX "X"

#define PRIXFAST8  "X"
#define PRIXFAST16 __PRIPTR_PREFIX "X"
#define PRIXFAST32 __PRIPTR_PREFIX "X"
#define PRIXFAST64 __PRI64_PREFIX "X"

/* Macros for printing `intmax_t' and `uintmax_t'.  */
#define PRIdMAX    __PRI64_PREFIX "d"
#define PRIiMAX    __PRI64_PREFIX "i"
#define PRIoMAX    __PRI64_PREFIX "o"
#define PRIuMAX    __PRI64_PREFIX "u"
#define PRIxMAX    __PRI64_PREFIX "x"
#define PRIXMAX    __PRI64_PREFIX "X"

/* Macros for printing `intptr_t' and `uintptr_t'.  */
#define PRIdPTR    __PRIPTR_PREFIX "d"
#define PRIiPTR    __PRIPTR_PREFIX "i"
#define PRIoPTR    __PRIPTR_PREFIX "o"
#define PRIuPTR    __PRIPTR_PREFIX "u"
#define PRIxPTR    __PRIPTR_PREFIX "x"
#define PRIXPTR    __PRIPTR_PREFIX "X"

/*** ISO C99 limits.h definitions and types ***/
#ifndef __INT32_MAX__
#define __INT32_MAX__   2147483647
#endif

#ifndef __INT32_MIN__
#define __INT32_MIN__   (-INT32_MAX-1)
#endif

#ifndef __UINT32_MAX__
#define __UINT32_MAX__  4294967295U
#endif

#ifndef __INT64_MAX__
#define __INT64_MAX__   9223372036854775807LL
#endif

#ifndef __INT64_MIN__
#define __INT64_MIN__   (-INT64_MAX-1)
#endif

#ifndef __UINT64_MAX__
#define __UINT64_MAX__  18446744073709551615ULL
#endif

#define INT_MIN     __INT32_MIN__
#define INT_MAX     __INT32_MAX__
#define UINT_MAX    __UINT32_MAX__

#define LONG_MAX    __LONG_MAX__
#define LONG_MIN    (-LONG_MAX - 1L)
#if __SIZEOF_LONG__==4
#define ULONG_MAX   __UINT32_MAX__
#else
#define ULONG_MAX   __UINT64_MAX__
#endif

#define LLONG_MAX   __LONG_LONG_MAX__
#define LLONG_MIN   (-LLONG_MAX - 1LL)
#define ULLONG_MAX  __UINT64_MAX__

/*** ISO C99 float.h definitions and types ***/
#define FLT_TRUE_MIN 1.40129846432481707092e-45F
#define FLT_MIN 1.17549435082228750797e-38F
#define FLT_MAX 3.40282346638528859812e+38F
#define FLT_EPSILON 1.1920928955078125e-07F

#define FLT_MANT_DIG 24
#define FLT_MIN_EXP (-125)
#define FLT_MAX_EXP 128

#define FLT_DIG 6
#define FLT_MIN_10_EXP (-37)
#define FLT_MAX_10_EXP 38

#define DBL_TRUE_MIN 4.94065645841246544177e-324
#define DBL_MIN 2.22507385850720138309e-308
#define DBL_MAX 1.79769313486231570815e+308
#define DBL_EPSILON 2.22044604925031308085e-16

#define DBL_MANT_DIG 53
#define DBL_MIN_EXP (-1021)
#define DBL_MAX_EXP 1024

#define DBL_DIG 15
#define DBL_MIN_10_EXP (-307)
#define DBL_MAX_10_EXP 308

/*** ISO C99 stdbool.h definitions and types ***/
#ifndef true
#define true  1
#endif

#ifndef false
#define false 0
#endif

#ifndef _HAS_BOOL
#define _HAS_BOOL
typedef _Bool bool;
#endif

/*** Prototypes of Standard C Library functions ***/
/* stdio.h */
#ifndef DEBUG_BUILD
#define puts(x)            (-1)
#endif

int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);

/* string.h */
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strncat(char *dest, const char *src, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s);

/* stdlib.h */
int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);
double atof(const char *nptr);

double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);

long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);

int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);

void qsort(void *base, size_t nmemb, size_t size,
                  int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base,
                     size_t nmemb, size_t size,
                     int (*compar)(const void *, const void *));

void *lsearch(const void *key, void *base, size_t *nmemb,
                size_t size, int(*compar)(const void *, const void *));

/* math.h */
double sqrt(double x);
float sqrtf(float x);
long double sqrtl(long double x);

double cos(double x);
float cosf(float x);
long double cosl(long double x);

double sin(double x);
float sinf(float x);
long double sinl(long double x);

double tan(double x);
float tanf(float x);
long double tanl(long double x);

double atan(double x);
float atanf(float x);
long double atanl( long double x);

double log(double x);
float logf(float x);
long double logl(long double x);

double log10(double x);
float log10f(float x);
long double log10l(long double x);

double log2(double x);
float log2f(float x);
long double log2l(long double x);

double ldexp(double x, int exp);
float ldexpf(float x, int exp);
long double ldexpl(long double x, int exp);

double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);

double modf(double x, double *iptr);
float modff(float x, float *iptr);
long double modfl(long double x, long double *iptr);

double frexp(double x, int *exp);
float frexpf(float x, int *exp);
long double frexpl(long double x, int *exp);

double scalbln(double x, long int exp);
float scalblnf(float x, long int exp);
long double scalblnl(long double x, long int exp);

/*** API functions ***/
/* stdio.h */
/* printf (3) hook. redirects output to debug console */
typedef int (*_printf_ptr)(const char *str, ...);
ModuleLocal _printf_ptr printf;

/*** Implementation ***/
/* Prototypes */
static bool GetFirstRun (void);
static ErrCode_t GetLastError (void);
static void SetLastError (ErrCode_t err);
static void SetObjectType (Object_t type);
static void InputIntegerVariable (const char *name,
                                  int value,
                                  int min,
                                  int max,
                                  int *var,
                                  bool show);
static void InputDoubleVariable (const char *name,
                                 double value,
                                 double min,
                                 double max,
                                 double *var,
                                 bool show);
static void InputColorVariable (const char *name,
                                Color_t value,
                                Color_t *var,
                                bool show);

/* Global variables and definitions */
#define CGSCRIPT_VERSION    _CGSCRIPT_VERSION
#define TF_CURRENT          ChartCurrentTF ()
#define FirstRun            GetFirstRun ()

ModuleLocal Event_t         LastEvent = 0;

/* Returns true when debugging is on, false otherwise */
static bool Debug (void);

/* ModuleThreadId */
static uint64_t ModuleThreadId (void);

/*** Chart ***/
/* ChartWidth */
static int ChartWidth (void);

/* ChartHeight */
static int ChartHeight (void);

/* ChartForeColor */
static Color_t ChartForeColor (void);

/* ChartBackColor */
static Color_t ChartBackColor (void);

/* ChartCurrentTF */
static TimeFrame_t ChartCurrentTF (void);

/* ChartCurrentBar */
static int ChartCurrentBar (void);

/*** Common ***/
/* GetParentTitle */
static _String_t GetParentTitle (void);

/* GetModuleName */
static _String_t GetModuleName (void);

/* GetFractionalDigits */
static int GetFractionalDigits (double num);

/*** Bars ***/
/* NBars */
static int NBars (TimeFrame_t tf);

/* NBarsHA */
static int NBarsHA (TimeFrame_t tf);

/* Bar */
static BarData_t Bar (TimeFrame_t tf, int shift);

/* BarHA */
static BarData_t BarHA (TimeFrame_t tf, int shift);

/* NVisibleBars */
static int NVisibleBars (void);

/* NewestVisibleBar */
static int NewestVisibleBar (void);

/* OldestVisibleBar */
static int OldestVisibleBar (void);

/* Definitions for quick access of bar data */
#define Open(s)         Bar(TF_CURRENT,s).Open
#define High(s)         Bar(TF_CURRENT,s).High
#define Low(s)          Bar(TF_CURRENT,s).Low
#define Close(s)        Bar(TF_CURRENT,s).Close
#define Volume(s)       Bar(TF_CURRENT,s).Volume
#define Year(s)         Bar(TF_CURRENT,s).Year
#define Month(s)        Bar(TF_CURRENT,s).Month
#define Day(s)          Bar(TF_CURRENT,s).Day
#define Date(s)         Bar(TF_CURRENT,s).Date
#define Time(s)         Bar(TF_CURRENT,s).Time
#define Id(s)           ((const char *) Bar(TF_CURRENT,s).Id)
#define OpenHA(s)       BarHA(TF_CURRENT,s).Open
#define HighHA(s)       BarHA(TF_CURRENT,s).High
#define LowHA(s)        BarHA(TF_CURRENT,s).Low
#define CloseHA(s)      BarHA(TF_CURRENT,s).Close
#define IdHA(s)         ((const char *) BarHA(TF_CURRENT,s).Id)

/*** Prototypes and definitions for Arrays ***/
/* ArrayCreate */
static _Array_t ArrayCreate (int elemsize, int maxdim) ;

/* ArrayAppend */
static int ArrayAppend (_Array_t arr, void *elem);
#define cArrayAppend(a,e)       ArrayAppend(a,(void *)&e)

/* ArrayPut */
static void ArrayPut (_Array_t arr, int idx, void *elem);
#define cArrayPut(a,i,e)        ArrayPut(a,i,(void *)&e)

/* ArraySize */
static int ArraySize (_Array_t arr);

/* ArrayGet */
static void * ArrayGet (_Array_t arr, int index);
#define sArrayGet(a,i)      (*(short *)ArrayGet(a,i))
#define usArrayGet(a,i)     (*(unsigned short *)ArrayGet(a,i))
#define iArrayGet(a,i)      (*(int *)ArrayGet(a,i))
#define uiArrayGet(a,i)     (*(unsigned int *)ArrayGet(a,i))
#define lArrayGet(a,i)      (*(long *)ArrayGet(a,i))
#define ulArrayGet(a,i)     (*(unsigned long *)ArrayGet(a,i))
#define llArrayGet(a,i)     (*(long long *)ArrayGet(a,i))
#define ullArrayGet(a,i)    (*(unsigned long long *)ArrayGet(a,i))
#define fArrayGet(a,i)      (*(float *)ArrayGet(a,i))
#define dArrayGet(a,i)      (*(double *)ArrayGet(a,i))
#define ldArrayGet(a,i)     (*(long double *)ArrayGet(a,i))

/* ArrayReset */
static void ArrayReset (_Array_t arr);

/* ValueSet */
static void ValueSet (_ObjectHandler_t objptr, double value, int shift);

/*** Objects ***/
/* AddLabel */
static _ObjectHandler_t AddLabel (void);

/* AddText */
static _ObjectHandler_t AddText (void);

/* AddHorizontalLine */
static _ObjectHandler_t AddHorizontalLine (void);

/* AddVerticalLine */
static _ObjectHandler_t AddVerticalLine (const char *id);

/* AddCurve */
static _ObjectHandler_t AddCurve (void);

/* AddVerticalBars */
static _ObjectHandler_t AddVerticalBars (void);

/* RemoveAllChildren */
static void RemoveAllChildren (void);

/* RemoveChild */
static void RemoveChild (_ObjectHandler_t childptr);

/* SetText */
static void SetText (_ObjectHandler_t handler, _String_t text);

/* SetPrice */
static void SetPrice (_ObjectHandler_t handler, double price);

#define SetLevel(a,b)       SetPrice(a,b)
#define SetY(a,b)           SetPrice(a,b)

/* SetColor */
static void SetColor (_ObjectHandler_t handler, Color_t color);

/* SetColorRGB */
static void SetColorRGB (_ObjectHandler_t handler, int r, int g, int b);

/* SetXY */
static void SetXY (_ObjectHandler_t handler, int x, int y);

/* SetFontSize */
static void SetFontSize (_ObjectHandler_t handler, int fontsize);

/* SetFontWeight */
static void SetFontWeight (_ObjectHandler_t handler, FontWeight_t fontweight);

/* SetPeriod */
static void SetPeriod (_ObjectHandler_t handler, int period);

/* GetColor */
static Color_t GetColor (_ObjectHandler_t handler);

/* GetPeriod */
static int GetPeriod (_ObjectHandler_t handler);

/* SetThickness */
static void SetThickness (_ObjectHandler_t handler, int thickness);

/* AttachText */
static void AttachText (_ObjectHandler_t handler, const char *id, double price);

/* SetSymbol */
static void SetSymbol (_ObjectHandler_t handler, const char *symbol);

/* SetHAdjustment */
static void SetHAdjustment (_ObjectHandler_t handler, int adj);

/* SetVAdjustment */
static void SetVAdjustment (_ObjectHandler_t handler, int adj);

/* SetSubChartRange */
static void SetSubChartRange (double min, double max);

/* SetRefresh */
static void SetRefresh (_ObjectHandler_t handler);

/* NeedsUpdate */
static bool NeedsUpdate ();

/* LoopCounter */
static uint64_t LoopCounter ();

/*** String ***/
/* StrCpy */
static _String_t StrCpy (_String_t dst, _String_t src);

/* Cstr2Str */
static _String_t Cstr2Str (_String_t dst, const char *src);

/* StrCat */
static _String_t StrCat (_String_t dst, _String_t src);

/* Str2NCstr */
static char *Str2NCstr (char *cstr, _String_t str, size_t n);

/* StrLen */
static size_t StrLen (_String_t str);

/* StrCmp */
static int StrCmp (_String_t str1, _String_t str2);

/* StrCstrCmp */
static int StrCstrCmp (_String_t str1, const char *str2);

/* Str2Cstr */
static const char *Str2Cstr (_String_t str) ;

/* StrPrint */
#define StrPrint(s)         printf(Str2Cstr(s))

/* StrInit */
static _String_t StrInit (const char *initstr);

/*** Error ***/
/* StrError2 public API */
static const char * StrError2 (int code);

/* StrError */
static _String_t StrError (int code);

/*** Technical analysis ***/
/* ADX */
static _Array_t fADX (TimeFrame_t tf, int period);

/* ATR */
static _Array_t fATR (TimeFrame_t tf, int period);

/* AROONUP */
static _Array_t fAROONUP (TimeFrame_t tf, int period);

/* AROONDOWN */
static _Array_t fAROONDOWN (TimeFrame_t tf, int period);

/* BBANDSUPPER */
static _Array_t fBBANDSUPPER (TimeFrame_t tf, int period, Price_t appliedprice);

/* BBANDSLOWER */
static _Array_t fBBANDSLOWER (TimeFrame_t tf, int period, Price_t appliedprice);

/* BBANDSMIDDLE */
static _Array_t fBBANDSMIDDLE (TimeFrame_t tf, int period, Price_t appliedprice);

/* CCI */
static _Array_t fCCI (TimeFrame_t tf, int period);

/* DMX */
static _Array_t fDMX (TimeFrame_t tf, int period);

/* EMA */
static _Array_t fEMA (TimeFrame_t tf, int period, Price_t appliedprice);

/* Generic EMA */
static _Array_t gEMA (int period, _Array_t data);

/* STOCHSLOWK */
static _Array_t fSTOCHSLOWK (TimeFrame_t tf, int period);

/* STOCHSLOWD */
static _Array_t fSTOCHSLOWD (TimeFrame_t tf, int period);

/* STOCHFASTK */
static _Array_t fSTOCHFASTK (TimeFrame_t tf, int period);

/* STOCHFASTD */
static _Array_t fSTOCHFASTD (TimeFrame_t tf, int period);

/* MACD */
static _Array_t fMACD (TimeFrame_t tf, int period, Price_t appliedprice);

/* MACDSIGNAL */
static _Array_t fMACDSIGNAL (TimeFrame_t tf, int period, Price_t appliedprice);

/* MACDHIST */
static _Array_t fMACDHIST (TimeFrame_t tf, int period, Price_t appliedprice);

/* MFI */
static _Array_t fMFI (TimeFrame_t tf, int period);

/* MOMENTUM */
static _Array_t fMOMENTUM (TimeFrame_t tf, int period, Price_t appliedprice);

/* PSAR */
static _Array_t fPSAR (TimeFrame_t tf, int period);

/* ROC */
static _Array_t fROC (TimeFrame_t tf, int period, Price_t appliedprice);

/* RSI */
static _Array_t fRSI (TimeFrame_t tf, int period, Price_t appliedprice);

/* SMA */
static _Array_t fSMA (TimeFrame_t tf, int period, Price_t appliedprice);

/* Generic SMA */
static _Array_t gSMA (int period, _Array_t data);

/* STDDEV */
static _Array_t fSTDDEV (TimeFrame_t tf, int period, Price_t appliedprice);

/* WILLR */
static _Array_t fWILLR (TimeFrame_t tf, int period);

/* disabled standard library functions */
#define system(a)                       0
#define __builtin_malloc(n)             NULL
#define __builtin_alloca(n)             NULL
#define malloc(n)                       NULL
#define valloc(n)                       NULL
#define alloca(n)                       NULL
#define calloc(a,b)                     NULL
#define realloc(a,b)                    NULL
#define aligned_alloc(a,b)              NULL
#define posix_memalign(a,b,c)           0
#define memalign(a,b)                   NULL
#define pvalloc(a)                      NULL
#define free(x)
#define abort()
#define exit(a)
#define raise(a)                        0
#define kill(a,b)                      -1
#define killpg(a,b)                    -1
#define pthread_kill(a,b)               0
#define _exit(a)
#define _Exit(a)
#define mmap(a,b,c,d,e,f)               NULL
#define mmap2(a,b,c,d,e,f)              NULL
#define munmap(a,b)                    -1
#define fopen(a,b)                      NULL
#define fdopen(a,b)                     NULL
#define freopen(a,b,c)                  NULL
#define popen(a,b)                      NULL
#define dlopen(a,b)                     NULL
#define dlmopen(a,b)                    NULL
#define dlcose(a)                       0
#define LoadLibrary(a)                  NULL
#define pthread_create(a,b,c,d)         0
#define signal(a,b)                     0
#define sigaction(a,b,c)               -1
#define fork                            0
#define vfork                           0
#define inline              static
#define register
#define volatile
#define extern              static

#endif /* _CGSCRIPT_MODULE */

#endif /* CGTOOL */

#endif /* _CGSCRIPT_VERSION */

/*** pragmas ***/
#line 1 "_SOURCE_NAME_"
