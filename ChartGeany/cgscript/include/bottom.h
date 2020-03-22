/**/
#line 1 "append"
/* bottom.h
   Appended source to CGScript programs

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

#define _SUPPRESS_WARNING(a) (void)a

/* Returns true if time frame is valid, false otherwise */
#define _validtf(t)     (t>TF_YEAR?false:true)

#ifdef DEBUG_BUILD
#ifndef __TINYC__
typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)

/*
 *   _cprintf: prints a message on operating system's console
 */
int  vsprintf(char *str, const char *format, va_list ap);
int  puts(const char *s);

static int
_cprintf (const char *str, ...)
{
  int result;
  char consolestr[1024];
  va_list ap;
  va_start(ap, str);
  result = vsprintf (consolestr, str, ap);
  va_end(ap);
  puts (consolestr);
  return result;
}
#endif /* __TINYC__ */
#endif /* DEBUG_BUILD */

/* Returns true when debugging is on, false otherwise */
static bool
Debug (void)
{
#ifdef DEBUG_BUILD
  return true;
#else
  return false;
#endif /* DEBUG_BUILD */
}

/* Macro _UNIONPTR(ChartWidth) will create the following code:

   ModuleLocal _ChartWidth_ptr _ChartWidth_core;
   typedef union
   {
     _ChartWidth_ptr ChartWidth;
     void *ptr;
   } _ChartWidth_union;
 */
#define _UNIONPTR(name) \
    ModuleLocal _##name##_ptr _ ##name##_core; \
    typedef union\
    { _##name##_ptr name; void *ptr; } _##name##_union

/* macro _ASSIGNFUNC2PTR(ChartWidth) expands as:

  _ChartWidth_union ChartWidth_u;
  _GetFunctionFromRegistry (regsize, "ChartWidth", &(ChartWidth_u.ptr));
  _ChartWidth_core = ChartWidth_u.ChartWidth;
  _SUPPRESS_WARNING(ChartWidth)

*/

#define _ASSIGNFUNC2PTR(name) \
    _##name##_union name##_u; \
    _GetFunctionFromRegistry (regsize,#name, &(name##_u.ptr));\
    _##name##_core = name##_u.name;\
    _SUPPRESS_WARNING(name)

#ifdef  _GGSCRIPT_NATIVE_WIN
#define _GCSCRIPT_EXPORT __attribute__ ((dllexport))
#else
#define _GCSCRIPT_EXPORT __attribute__ ((visibility ("default")))
#endif

/* Module types */
typedef enum
{
  _CHART_OBJECT             /* chart object */
} _Module_t;

/*** Global module variables ***/
ModuleLocal _Module_t   _ModuleType = -1;
ModuleLocal Object_t    _ObjectType = -1;
ModuleLocal ErrCode_t   _LastError;
ModuleLocal uint64_t    _LoopCounter = 0;
ModuleLocal bool        _FirstRun = true;
ModuleLocal _Array_t     _ValueSet = NULL;
ModuleLocal _Array_t     _InputVariableRegistry = NULL;
ModuleLocal char        _MODULEID[256];
ModuleLocal bool        _NeedsUpd = true;
ModuleLocal void        *_PARENT_CHART = NULL;
ModuleLocal void        *_THIS = NULL;
ModuleLocal bool        _Active = true;
ModuleLocal bool        _Initialized = false;
ModuleLocal double      _rangemin = 0, _rangemax = 100;
ModuleLocal char _compiler[32] = "UNKNOWN";

/* Disable module's execution */
static void _disable (const char *fname, ErrCode_t errcode);

/*** Error ***/
/* StrError2 public API */
typedef const char * (*_StrError2_ptr) (const _ObjectHandler_t objptr, int);
_UNIONPTR (StrError2);

static const char *
StrError2 (int code)
{
  if (_THIS == NULL)
  {
    _disable (__func__, CGSERR_NO_MODULE);
    return NULL;
  }

  return _StrError2_core (_THIS, code);
}

/* Disable module's execution */
static void
_disable (const char *fname, ErrCode_t errcode)
{
  _Active = false;
  LastEvent = 0;

  SetLastError (errcode);
  if (Debug ())
    printf ("= Error %d. Module %s stopped by function %s: %s",
             errcode, _MODULEID, fname, _StrError2_core (_THIS, errcode));
}

/* StrError */
ModuleLocal _String_t _ErrorString = NULL;
static _String_t
StrError (int code)
{
  if (_ErrorString == NULL)
    _ErrorString = StrInit (NULL);

  Cstr2Str (_ErrorString, StrError2 (code));
  return _ErrorString;
}
/*** End of error ***/

/* Loop counter */
static uint64_t
LoopCounter ()
{
  return _LoopCounter;
}

/* ModuleThreadId */
static uint64_t
ModuleThreadId (void)
{
#if __SIZEOF_POINTER__==4
  typedef union
  {
    uint32_t rslt;
    void *ptr;
  } Ptr_Union_t;
#else
  typedef union
  {
    uint64_t rslt;
    void *ptr;
  } Ptr_Union_t;
#endif

  Ptr_Union_t ptrunion;
  ptrunion.ptr = _THIS;
  return (uint64_t) ptrunion.rslt;
}

/* Return true when Loop() is running for the first time */
static bool
GetFirstRun ()
{
  return _FirstRun;
}

/* InitValueSet */
typedef _Array_t (*_InitValueSet_ptr) (void *);
ModuleLocal _InitValueSet_ptr _InitValueSet_core;
typedef union
{
  _InitValueSet_ptr _InitValueSet;
  void *ptr;
} _InitValueSet_union;

static _Array_t
_InitValueSet (void)
{
  return _InitValueSet_core (_THIS);
}

/* SetInputVariables */
typedef void (*_SetInputVariables_ptr) (_ObjectHandler_t objptr, _Array_t variables);
_UNIONPTR(SetInputVariables);

static void
SetInputVariables (_ObjectHandler_t handler, _Array_t variables)
{
  if (!_Active)
    return;

  if (handler == NULL)
    _SetInputVariables_core (_THIS, variables);
  else
    _SetInputVariables_core (handler, variables);
}

/* SetRange */
typedef void (*_SetRange_ptr) (_ObjectHandler_t objptr, double min, double max);
_UNIONPTR(SetRange);

static void
SetRange (_ObjectHandler_t handler, double min, double max)
{
  if (!_Active)
    return;

  if (handler == NULL)
    _SetRange_core (_THIS, min, max);
  else
    _SetRange_core (handler, min, max);
}

/* ArrayExists */
typedef bool (*_ArrayExists_ptr) (const _Array_t arr);
_UNIONPTR(ArrayExists);

static bool
ArrayExists (const _Array_t arr)
{
  return _ArrayExists_core (arr);
}

/* _ArrayChecks */
static bool
_ArrayChecks (const _Array_t arr, const char *func)
{
  if (!_Active)
    return false;

  if (strncmp (func, "ArrayCreate", 32) != 0 &&
      strncmp (func, "ArrayDestroyAll", 32) != 0)
  {
    if (arr == NULL || ArrayExists (arr) == false)
    {
      _disable (func, CGSERR_NO_ARRAY);
      return false;
    }
  }

  return true;
}

/* ArrayDestroy */
typedef void (*_ArrayDestroy_ptr) (_Array_t arr);
ModuleLocal _ArrayDestroy_ptr _ArrayDestroy_core;
typedef union
{
  _ArrayDestroy_ptr _ArrayDestroy;
  void *ptr;
} _ArrayDestroy_union;

static void
_ArrayDestroy (_Array_t arr)
{
  _ArrayDestroy_core (arr);
}

/* ObjectDestroy */
typedef void (*_ObjectDestroy_ptr) (_ObjectHandler_t obj);
ModuleLocal _ObjectDestroy_ptr _ObjectDestroy_core;
typedef union
{
  _ObjectDestroy_ptr _ObjectDestroy;
  void *ptr;
} _ObjectDestroy_union;

static void
_ObjectDestroy (_ObjectHandler_t obj)
{
  _ObjectDestroy_core (obj);
}

/* Clean up array when it goes out of scope */
static void
_ArrayCleanup (_Array_t *arr)
{
  _ArrayDestroy (*arr);
}

/* ArrayDestroyAll */
typedef void (*_ArrayDestroyAll_ptr) (const _ObjectHandler_t objptr);
ModuleLocal _ArrayDestroyAll_ptr _ArrayDestroyAll_core;
typedef union
{
  _ArrayDestroyAll_ptr _ArrayDestroyAll;
  void *ptr;
} _ArrayDestroyAll_union;

static void
_ArrayDestroyAll (const _ObjectHandler_t objptr)
{
  if (objptr == NULL)
    return;

  _ArrayDestroyAll_core (objptr);
}

/* StringDestroy */
typedef void (*_StringDestroy_ptr) (const _ObjectHandler_t objptr, _String_t s);
ModuleLocal _StringDestroy_ptr _StringDestroy_core;
typedef union
{
  _StringDestroy_ptr _StringDestroy;
  void *ptr;
} _StringDestroy_union;

static void
_StringDestroy (_String_t s)
{
  if (_THIS == NULL)
    return;

  _StringDestroy_core (_THIS, s);
}

/* Clean up string when it goes out of scope */
static void
_StringCleanup (_String_t *str)
{
  _StringDestroy (*str);
}

/* StringDestroyAll */
typedef void (*_StringDestroyAll_ptr) (const _ObjectHandler_t objptr);
ModuleLocal _StringDestroyAll_ptr _StringDestroyAll_core;
typedef union
{
  _StringDestroyAll_ptr _StringDestroyAll;
  void *ptr;
} _StringDestroyAll_union;

static void
_StringDestroyAll (void)
{
  if (_THIS == NULL)
    return;

  _StringDestroyAll_core (_THIS);
}

/* Clean up object when it goes out of scope */
static void
_ObjectCleanup (_ObjectHandler_t *obj)
{
  _ObjectDestroy (*obj);
}

/* Function registry */
typedef struct
{
  void *ptr;        /* function's pointer */
  char name[56];    /* function name (55 chars + NULL) */
} _FunctionRegistryRec;

ModuleLocal _FunctionRegistryRec *_FunctionRegistry;
ModuleLocal TimeFrame_t _TF_CURRENT = -1;

/* Needs Update */
static bool
_NeedsUpdate ()
{
  static double last_close;
  static TimeFrame_t tf;
  static int nbrs, last_period;

  if (!_Active)
    return false;

  if (FirstRun)
  {
    tf = TF_CURRENT;
    nbrs = NBars(tf);
    last_period = GetPeriod(NULL);
    last_close = Close(0);
    return true;
  }

  if (!(tf != TF_CURRENT || nbrs != NBars(TF_CURRENT) || last_close != Close(0) ||
      last_period != GetPeriod(NULL)))
    return false;

  tf = TF_CURRENT;
  nbrs = NBars(tf);
  last_close = Close (0);
  last_period = GetPeriod(NULL);

  return true;
}

/*
 * set debug mode
 */
typedef void (*_SetDebugMode_ptr) (_ObjectHandler_t objptr, bool mode);
ModuleLocal _SetDebugMode_ptr _SetDebugMode_core;

typedef union
{
  _SetDebugMode_ptr _SetDebugMode;
  void *ptr;
} _SetDebugMode_union;

static void
_SetDebugMode (_ObjectHandler_t objptr, bool mode)
{
  _SetDebugMode_core (objptr, mode);
}


/*
 *  assign function pointer to void pointer
 */
static int
_FunctionRegistryCompar (const void *v1, const void *v2)
{
  _FunctionRegistryRec
    *r1 = (_FunctionRegistryRec  *) v1,
    *r2 = (_FunctionRegistryRec  *) v2;

  return strncmp (r1->name, r2->name, 55);
}

static void
_GetFunctionFromRegistry (size_t regsize, const char *name, void **ptr)
{
  _FunctionRegistryRec key, *res;

  if (!_Active)
    return;

  key.name[55] = 0;
  strncpy (key.name, name, 55);
  res = bsearch (&key, _FunctionRegistry, regsize,
                 sizeof (_FunctionRegistryRec), _FunctionRegistryCompar);

  if (res != NULL)
    *ptr = res->ptr;
  else
    *ptr = NULL;

  if (*ptr == NULL)
    _disable ("Init", CGSERR_INTERNAL_100);
}

/* function pointers assignments */
static void _AssignCGScriptFunctionsToPtrs (size_t regsize);

/*
 *   Module initialization function
 */
int _GCSCRIPT_EXPORT
_CGModuleInitObject (void *registry, size_t registrysize, void *parent,
               int *object_type, void *self, void *data)
{

  int rslt = _LastError = CGSERR_OK;

  if (_Initialized)
  {
    _disable ("Init", CGSERR_ALREADY_INITIALIZED);
    return CGSERR_ALREADY_INITIALIZED;
  }

  _SUPPRESS_WARNING(data);

  _MODULEID[255] = 0;
  strncpy (_MODULEID, MODID, 255);
  _Active = true;
  _FunctionRegistry = (_FunctionRegistryRec *) registry;

  _AssignCGScriptFunctionsToPtrs ((size_t) registrysize);
  if (!_Active)
    return _LastError;

  if (parent == NULL)
  {
    _disable ("Init", CGSERR_NO_PARENT_CHART);
    return CGSERR_NO_PARENT_CHART;
  }

  if (self == NULL)
  {
    _disable ("Init", CGSERR_NO_MODULE);
    return CGSERR_NO_MODULE;
  }

  _ModuleType = _CHART_OBJECT;
  _PARENT_CHART = parent;
  _THIS = self;
  _TF_CURRENT = ChartCurrentTF ();
  _InputVariableRegistry = ArrayCreate ((int) sizeof(_InputVariableRegistryRec), 16);

  rslt = Init ();
  if (rslt != CGSERR_OK)
    return rslt;

  if (object_type != NULL)
  {
    if (_PARENT_CHART != NULL)
      *object_type = _ObjectType;
    else
      *object_type = -1;
  }

  if (Debug ())
    _SetDebugMode (_THIS, true);

  _Initialized = true;

  if (Debug ())
    printf ("\nModule %s started", _MODULEID);

  return rslt;
}

/*
 *   Module loop function
 */
int _GCSCRIPT_EXPORT
_CGModuleLoop (void)
{
  if (!_Active)
    return _LastError;

  if (!_Initialized)
  {
    _disable ("Loop", CGSERR_UNINITIALIZED);
    return CGSERR_UNINITIALIZED;
  }

  if (_FirstRun)
  {
    if (_ObjectType == LABEL_OBJECT)
    {
      _String_t str = StrInit ("Label");
      SetXY (NULL, 0,0);
      SetFontSize (NULL, 10);
      SetColor (NULL, ChartForeColor ());
      SetFontWeight (NULL, FONTWEIGHT_NORMAL);
      SetText (NULL, str);
    }

    if (_ObjectType == TEXT_OBJECT)
    {
      _String_t str = StrInit ("Text");
      SetXY (NULL, 50,50);
      SetFontSize (NULL, 10);
      SetColor (NULL, ChartForeColor ());
      SetFontWeight (NULL, FONTWEIGHT_NORMAL);
      SetText (NULL, str);
    }

    if (_ObjectType == CONTAINER_OBJECT || _ObjectType == SUBCHART_OBJECT)
    {
      if (ArraySize (_InputVariableRegistry) > 0)
        SetInputVariables (NULL, _InputVariableRegistry);
    }

    if (_ObjectType == SUBCHART_OBJECT)
    {
      SetRange (_THIS, _rangemin, _rangemax);
    }

    _ValueSet = _InitValueSet ();
    if (_ValueSet == NULL)
    {
      _disable ("ValueSet", CGSERR_MEMORY);
      return CGSERR_MEMORY;
    }
  }

  if (LastEvent != 0)
    _NeedsUpd = true;
  else
    _NeedsUpd = _NeedsUpdate ();

  if (!_Active)
    return _LastError;

  int rslt = Loop ();

  LastEvent = 0;
  _FirstRun = false;
  _LoopCounter ++;

  return rslt;
}

/*
 *   Module event function
 */
void _GCSCRIPT_EXPORT
_CGModuleEvent (int event)
{
  if (!_Initialized)
    return;

  if (_FirstRun)
    return;

  if (!_Active)
    return;

  LastEvent = event;
  Event ();

  if (LastEvent == EV_INPUT_VAR)
  {
    SetRefresh (NULL);
    Loop ();
  }

  return;
}

/*
 *   Module finish function
 */
void _GCSCRIPT_EXPORT
_CGModuleFinish (void)
{
  if (!_Initialized)
    return;

  if (_PARENT_CHART != NULL && _THIS != NULL)
    _ArrayDestroyAll (_THIS);

  _StringDestroyAll ();

  _Active = false;

  if (Debug ())
    printf ("\nModule %s terminated", _MODULEID);

  _SUPPRESS_WARNING(COLOR_WHITE);
  _SUPPRESS_WARNING(COLOR_BLACK);
  _SUPPRESS_WARNING(COLOR_RED);
  _SUPPRESS_WARNING(COLOR_DARKRED);
  _SUPPRESS_WARNING(COLOR_GREEN);
  _SUPPRESS_WARNING(COLOR_DARKGREEN);
  _SUPPRESS_WARNING(COLOR_BLUE);
  _SUPPRESS_WARNING(COLOR_DARKBLUE);
  _SUPPRESS_WARNING(COLOR_CYAN);
  _SUPPRESS_WARNING(COLOR_DARKCYAN);
  _SUPPRESS_WARNING(COLOR_MAGENTA);
  _SUPPRESS_WARNING(COLOR_DARKMAGENTA);
  _SUPPRESS_WARNING(COLOR_YELLOW);
  _SUPPRESS_WARNING(COLOR_DARKYELLOW);
  _SUPPRESS_WARNING(COLOR_GRAY);
  _SUPPRESS_WARNING(COLOR_DARKGRAY);
  _SUPPRESS_WARNING(COLOR_LIGHTGRAY);
  _SUPPRESS_WARNING(FONTWEIGHT_LIGHT);
  _SUPPRESS_WARNING(FONTWEIGHT_NORMAL);
  _SUPPRESS_WARNING(FONTWEIGHT_DEMIBOLD);
  _SUPPRESS_WARNING(FONTWEIGHT_BOLD);
  _SUPPRESS_WARNING(FONTWEIGHT_BLACK);
  _SUPPRESS_WARNING(MODAUTHOR);
  _SUPPRESS_WARNING(MODVERSION);
  _SUPPRESS_WARNING(MODTYPE);
  _SUPPRESS_WARNING(WHITE_DOWN_POINTING_INDEX);
  _SUPPRESS_WARNING(WHITE_UP_POINTING_INDEX);
  _SUPPRESS_WARNING(DOWNWARDS_WHITE_ARROW);
  _SUPPRESS_WARNING(UPWARDS_WHITE_ARROW);
  _SUPPRESS_WARNING(DOWNWARDS_ARROW);
  _SUPPRESS_WARNING(UPWARDS_ARROW);
  _SUPPRESS_WARNING(SetSubChartRange);
}

/*
 * Module's set of values
 */
_Array_t _GCSCRIPT_EXPORT
_CGModuleValueSet (void)
{
  if (!_Active)
    return NULL;

  return _ValueSet;
}

/*
 *  Deactivate module
 */
void _GCSCRIPT_EXPORT
_CGModuleDeactivate (const char *fname, ErrCode_t errcode)
{
  _disable (fname, errcode);
}

/*
 *  Compiler used to compile the module
 */
const char _GCSCRIPT_EXPORT *
_CGModuleCompiler (void)
{
  _compiler[31] = 0;

#ifdef __TINYC__
  strncpy (_compiler, "TINYC", 31);
#endif

#ifdef __PCC__
  strncpy (_compiler, "PCC",31);
#endif

#ifdef __clang__
  strncpy (_compiler, "CLANG", 31);
#endif

#ifdef __GNUC__
  strncpy (_compiler, "GNUC", 31);
#endif

  return _compiler;
}

/* NeedsUpdate */
static bool
NeedsUpdate ()
{
  return _NeedsUpd;
}

/* Get the last error */
static ErrCode_t
GetLastError (void)
{
  return _LastError;
}

/* Set the last error */
static void
SetLastError (ErrCode_t err)
{
  _LastError = err;
}

/* Set the object type */
static void
SetObjectType (Object_t type)
{
  if (!_Active)
    return;

  if (_Initialized)
  {
    _disable (__func__, CGSERR_ALREADY_INITIALIZED);
    return;
  }

  if (_ModuleType != _CHART_OBJECT)
  {
    _disable (__func__, CGSERR_INVALID_MODTYPE);
    return;
  }

  if (type > CONTAINER_OBJECT)
  {
    _disable (__func__, CGSERR_INVALID_ARGUMENT);
    return;
  }

  _ObjectType = type;
}

/*** String ***/
/* StrCpy */
typedef _String_t (*_StrCpy_ptr) (_String_t dst, _String_t src);
_UNIONPTR(StrCpy);

static _String_t
StrCpy (_String_t dst, _String_t src)
{
  return _StrCpy_core (dst, src);
}

/* Cstr2Str */
typedef _String_t (*_Cstr2Str_ptr) (const _ObjectHandler_t objptr, _String_t dst, const char *src);
_UNIONPTR(Cstr2Str);

static _String_t
Cstr2Str (_String_t dst, const char *src)
{
  return _Cstr2Str_core (_THIS, dst, src);
}

/* StrCat */
typedef _String_t (*_StrCat_ptr) (_String_t dst, _String_t src);
_UNIONPTR(StrCat);

static _String_t
StrCat (_String_t dst, _String_t src)
{
  return _StrCat_core (dst, src);
}

/* Str2NCstr */
typedef char * (*_Str2NCstr_ptr) (char *cstr, _String_t str, size_t n);
_UNIONPTR(Str2NCstr);

static char *
Str2NCstr (char *cstr, _String_t str, size_t n)
{
  return _Str2NCstr_core (cstr, str, n);
}

/* StrLen */
typedef size_t (*_StrLen_ptr) (_String_t str);
_UNIONPTR(StrLen);

static size_t
StrLen (_String_t str)
{
  return _StrLen_core (str);
}

/* StrCmp */
typedef int (*_StrCmp_ptr) (_String_t str1, _String_t str2);
_UNIONPTR(StrCmp);

static int
StrCmp (_String_t str1, _String_t str2)
{
  return _StrCmp_core (str1, str2);
}

/* StrCstrCmp */
typedef int (*_StrCstrCmp_ptr) (_String_t str1, const char *str2);
_UNIONPTR(StrCstrCmp);

static int
StrCstrCmp (_String_t str1, const char *str2)
{
  return _StrCstrCmp_core (str1, str2);
}

/* Str2Cstr */
typedef const char * (*_Str2Cstr_ptr) (_String_t str);
_UNIONPTR(Str2Cstr);

static const char *
Str2Cstr (_String_t str)
{
  return _Str2Cstr_core (str);
}

/* StrInit */
typedef _String_t (*_StrInit_ptr) (const _ObjectHandler_t objptr, const char *initstr);
_UNIONPTR(StrInit);

static _String_t
StrInit (const char *initstr)
{
  return _StrInit_core (_THIS, initstr);
}

/*** Arrays ***/
/* ArrayCreate */
typedef _Array_t (*_ArrayCreate_ptr) (_ObjectHandler_t objptr, int elemsize, int maxdim);
_UNIONPTR(ArrayCreate);

static _Array_t
ArrayCreate (int elemsize, int maxdim)
{
  if (!_ArrayChecks (NULL, "ArrayCreate"))
    return NULL;

  /*
  if (_Initialized)
  {
    _disable (__func__, CGSERR_ARRAY_CREATION_AFTER_INIT);
    return NULL;
  }
  */

  if (elemsize > 512)
  {
    _disable (__func__, CGSERR_ARRAY_ELEM_BIG);
    return NULL;
  }

  if (elemsize <= 0)
  {
    _disable (__func__, CGSERR_ARRAY_ELEM_ZERO);
    return NULL;
  }

  if (maxdim < 1 || maxdim > 65536)
    maxdim = 65536;

  _Array_t newarr;
  newarr = _ArrayCreate_core (_THIS, elemsize, maxdim);

  if (newarr == NULL)
    _disable (__func__, CGSERR_ARRAY_CREATION_FAILED);

  return newarr;
}

/* ArrayAppend */
typedef int (*_ArrayAppend_ptr) (_Array_t arr, void *elem);
_UNIONPTR(ArrayAppend);

static int
ArrayAppend (_Array_t arr, void *elem)
{
  if (!_ArrayChecks (arr, "ArrayAppend"))
    return -1;

  return _ArrayAppend_core (arr, elem);
}

/* ArrayPut */
typedef void (*_ArrayPut_ptr) (_Array_t arr, int idx, void *elem);
ModuleLocal _ArrayPut_ptr _ArrayPut_core;
typedef union
{
  _ArrayPut_ptr _ArrayPut;
  void *ptr;
} _ArrayPut_union;

static void
_ArrayPut (_Array_t arr, int idx, void *elem)
{
  _ArrayPut_core (arr, idx, elem);
  return;
}

static void
ArrayPut (_Array_t arr, int idx, void *elem)
{
  if (!_ArrayChecks (arr, "ArrayPut"))
    return;

  _ArrayPut (arr, idx, elem);
  return;
}

/* ArraySize */
typedef int (*_ArraySize_ptr) (_Array_t arr);
_UNIONPTR(ArraySize);

static int
ArraySize (_Array_t arr)
{
  if (!_ArrayChecks (arr, "ArraySize"))
    return -1;

  return _ArraySize_core (arr);
}

/* ArrayGet */
typedef void * (*_ArrayGet_ptr) (_Array_t arr, int index);
_UNIONPTR(ArrayGet);

static void *
ArrayGet (_Array_t arr, int index)
{
  if (!_ArrayChecks (arr, "ArrayGet"))
    return NULL;

  return _ArrayGet_core (arr, index);
}

/* ArrayReset */
typedef void (*_ArrayReset_ptr) (_Array_t arr);
_UNIONPTR(ArrayReset);

static void
ArrayReset (_Array_t arr)
{
  if (!_ArrayChecks (arr, "ArrayReset"))
    return;

  _ArrayReset_core (arr);
}

/* GetDynamicValueSet */
typedef _Array_t (*_GetDynamicValueSet_ptr) (const _ObjectHandler_t objptr);
ModuleLocal _GetDynamicValueSet_ptr _GetDynamicValueSet_core;
typedef union
{
  _GetDynamicValueSet_ptr _GetDynamicValueSet;
  void *ptr;
} _GetDynamicValueSet_union;

static _Array_t
_GetDynamicValueSet (const _ObjectHandler_t objptr)
{
  return _GetDynamicValueSet_core (objptr);
}
/*** End of arrays ***/

/*** Chart ***/
/* ChartWidth */
typedef int (*_ChartWidth_ptr) (const void *chartptr);
_UNIONPTR(ChartWidth);

static int
ChartWidth (void)
{
  if (!_Active)
    return GetLastError ();

  return _ChartWidth_core (_PARENT_CHART);
}

/* ChartHeight */
typedef int (*_ChartHeight_ptr) (const void *chartptr);
_UNIONPTR(ChartHeight);

static int
ChartHeight (void)
{
  if (!_Active)
    return GetLastError ();

  return _ChartHeight_core (_PARENT_CHART);
}

/* ChartForeColor */
typedef Color_t (*_ChartForeColor_ptr) (const void *chartptr);
_UNIONPTR(ChartForeColor);

static Color_t
ChartForeColor (void)
{
  if (!_Active)
    return GetLastError ();

  return _ChartForeColor_core (_PARENT_CHART);
}

/* ChartBackColor */
typedef Color_t (*_ChartBackColor_ptr) (const void *chartptr);
_UNIONPTR(ChartBackColor);

static Color_t
ChartBackColor (void)
{
  if (!_Active)
    return GetLastError ();

  return _ChartBackColor_core (_PARENT_CHART);
}

/* ChartCurrentTF */
typedef TimeFrame_t (*_ChartCurrentTF_ptr) (const void *chartptr);
_UNIONPTR(ChartCurrentTF);

static TimeFrame_t
ChartCurrentTF (void)
{
  if (!_Active)
    return -1;

  return _ChartCurrentTF_core (_PARENT_CHART);
}

/* ChartCurrentBar */
typedef int (*_ChartCurrentBar_ptr) (const void *chartptr);
_UNIONPTR(ChartCurrentBar);

static int
ChartCurrentBar (void)
{
  if (!_Active)
    return -1;

  return _ChartCurrentBar_core (_PARENT_CHART);
}
/*** End of chart ***/

/*** Common ***/
/* GetParentTitle */
typedef _String_t (*_GetParentTitle_ptr) (const void *chartptr);
_UNIONPTR(GetParentTitle);

static _String_t
GetParentTitle (void)
{
  if (!_Active)
    return NULL;

  return _GetParentTitle_core (_PARENT_CHART);
}

/* GetModuleName */
typedef _String_t (*_GetModuleName_ptr) (const void *chartptr);
_UNIONPTR(GetModuleName);

static _String_t
GetModuleName (void)
{
  if (!_Active)
    return NULL;

  return _GetModuleName_core (_THIS);
}

/* GetFractionalDigits */
typedef int (*_GetFractionalDigits_ptr) (double);
_UNIONPTR(GetFractionalDigits);

static int
GetFractionalDigits (double num)
{
  if (!_Active)
    return 0;

  return _GetFractionalDigits_core (num);
}

/*** End of common ***/

/*** Bars ***/
ModuleLocal int _lastshift = -1, _lastshiftHA = -1;
ModuleLocal int _lastnbars = -1, _lastnbarsHA = -1;
ModuleLocal BarData_t _bdata, _bdataHA;

/* NBars */
typedef int (*_NBars_ptr) (const void *chartptr, int, int);
_UNIONPTR(NBars);

static int
NBars (TimeFrame_t tf)
{
  if (!_Active)
    return GetLastError ();

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return GetLastError ();
  }

  return _NBars_core (_PARENT_CHART, CTYPE_CANDLE, tf);
}

/* NBarsHA */
static int
NBarsHA (TimeFrame_t tf)
{
  if (!_Active)
    return GetLastError ();

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return GetLastError ();
  }

  return _NBars_core (_PARENT_CHART, CTYPE_HEIKINASHI, tf);
}

/* Bar */
typedef BarData_t (*_Bar_ptr) (const void *chartptr, int, int, int);
_UNIONPTR(Bar);

static BarData_t
Bar (TimeFrame_t tf, int shift)
{
  if (!_Active)
  {
    memset (&_bdata, 0, sizeof (_bdata));
    return _bdata;
  }

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return _bdata;
  }

  if (shift < 0)
    return _bdata;

  int nbars = _NBars_core (_PARENT_CHART, CTYPE_CANDLE, tf);
  if (_lastshift != shift || _lastnbars != nbars || shift == 0)
  {
    _bdata = _Bar_core (_PARENT_CHART, CTYPE_CANDLE, tf, shift);
    _lastshift = shift;
    _lastnbars = nbars;
  }

  return _bdata;
}

/* BarHA */
static BarData_t
BarHA (TimeFrame_t tf, int shift)
{
  if (!_Active)
  {
    memset (&_bdataHA, 0, sizeof (_bdataHA));
    return _bdataHA;
  }

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return _bdataHA;
  }

  if (shift < 0)
    return _bdata;

  int nbars = _NBars_core (_PARENT_CHART, CTYPE_HEIKINASHI, tf);
  if (_lastshiftHA != shift || _lastnbarsHA != nbars || shift == 0)
  {
    _bdataHA = _Bar_core (_PARENT_CHART, CTYPE_HEIKINASHI, tf, shift);
    _lastshiftHA = shift;
    _lastnbarsHA = nbars;
  }

  return _bdataHA;
}

/* NVisibleBars */
typedef int (*_NVisibleBars_ptr) (const void *chartptr);
_UNIONPTR(NVisibleBars);

static int
NVisibleBars (void)
{
  if (!_Active)
    return GetLastError ();

  return _NVisibleBars_core (_PARENT_CHART);
}

/* NewestVisibleBar */
typedef int (*_NewestVisibleBar_ptr) (const void *chartptr);
_UNIONPTR(NewestVisibleBar);

static int
NewestVisibleBar (void)
{
  if (!_Active)
    return GetLastError ();

  return _NewestVisibleBar_core (_PARENT_CHART);
}

/* OldestVisibleBar */
typedef int (*_OldestVisibleBar_ptr) (const void *chartptr);
_UNIONPTR(OldestVisibleBar);

static int
OldestVisibleBar (void)
{
  if (!_Active)
    return GetLastError ();

  return _OldestVisibleBar_core (_PARENT_CHART);
}
/*** End of bars ***/

/*** Objects ***/
/* AddLabel */
typedef _ObjectHandler_t (*_AddLabel_ptr) (void *chartptr, _ObjectHandler_t objptr);
_UNIONPTR(AddLabel);

static _ObjectHandler_t
AddLabel (void)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddLabel_core (_PARENT_CHART, _THIS);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* AddText */
typedef _ObjectHandler_t (*_AddText_ptr) (void *chartptr, _ObjectHandler_t objptr);
_UNIONPTR(AddText);

static _ObjectHandler_t
AddText (void)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddText_core (_PARENT_CHART, _THIS);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* AddHorizontalLine */
typedef _ObjectHandler_t (*_AddHorizontalLine_ptr) (void *chartptr, _ObjectHandler_t objptr);
_UNIONPTR(AddHorizontalLine);

static _ObjectHandler_t
AddHorizontalLine (void)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddHorizontalLine_core (_PARENT_CHART, _THIS);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* AddVerticalLine */
typedef _ObjectHandler_t (*_AddVerticalLine_ptr) (void *chartptr, _ObjectHandler_t objptr,
                                                 const char *id);
_UNIONPTR(AddVerticalLine);

static _ObjectHandler_t
AddVerticalLine (const char *id)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddVerticalLine_core (_PARENT_CHART, _THIS, id);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* AddCurve */
typedef _ObjectHandler_t (*_AddCurve_ptr) (void *chartptr, _ObjectHandler_t objptr);
_UNIONPTR(AddCurve);

static _ObjectHandler_t
AddCurve (void)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddCurve_core (_PARENT_CHART, _THIS);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* AddVerticalBars */
typedef _ObjectHandler_t (*_AddVerticalBars_ptr) (void *chartptr, _ObjectHandler_t objptr);
_UNIONPTR(AddVerticalBars);

static _ObjectHandler_t
AddVerticalBars (void)
{
  _ObjectHandler_t handler;

  if (!_Active)
    return NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return NULL;
  }

  handler = _AddVerticalBars_core (_PARENT_CHART, _THIS);

  if (handler == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }

  return handler;
}

/* RemoveAllChildren */
typedef void (*_RemoveAllChildren_ptr) (_ObjectHandler_t objptr);
_UNIONPTR(RemoveAllChildren);

static void
RemoveAllChildren (void)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  _RemoveAllChildren_core (_THIS);
}

/* RemoveChild */
typedef void (*_RemoveChild_ptr) (_ObjectHandler_t objptr, _ObjectHandler_t childptr);
_UNIONPTR(RemoveChild);

static void
RemoveChild (_ObjectHandler_t childptr)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  _RemoveChild_core (_THIS, childptr);
}

/* SetText */
typedef void (*_SetText_ptr) (_ObjectHandler_t objptr, _String_t text);
_UNIONPTR(SetText);

static void
SetText (_ObjectHandler_t handler, _String_t text)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetText_core (_THIS, text);
  else
    _SetText_core (handler, text);
}

/* SetPrice */
typedef void (*_SetPrice_ptr) (_ObjectHandler_t objptr, double price);
_UNIONPTR(SetPrice);

static void
SetPrice (_ObjectHandler_t handler, double price)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetPrice_core (_THIS, price);
  else
    _SetPrice_core (handler, price);
}

/* SetColor */
typedef void (*_SetColor_ptr) (_ObjectHandler_t objptr, Color_t color);
_UNIONPTR(SetColor);

static void
SetColor (_ObjectHandler_t handler, Color_t color)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetColor_core (_THIS, color);
  else
    _SetColor_core (handler, color);
}

/* SetColorRGB */
typedef void (*_SetColorRGB_ptr) (_ObjectHandler_t objptr, int r, int g, int b);
_UNIONPTR(SetColorRGB);

static void
SetColorRGB (_ObjectHandler_t handler, int r, int g, int b)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetColorRGB_core (_THIS, r, g, b);
  else
    _SetColorRGB_core (handler, r, g, b);
}

/* SetXY */
typedef void (*_SetXY_ptr) (_ObjectHandler_t objptr, int x, int y);
_UNIONPTR(SetXY);

static void
SetXY (_ObjectHandler_t handler, int x, int y)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetXY_core (_THIS, x, y);
  else
    _SetXY_core (handler, x, y);
}

/* SetFontSize */
typedef void (*_SetFontSize_ptr) (_ObjectHandler_t objptr, int fontsize);
_UNIONPTR(SetFontSize);

static void
SetFontSize (_ObjectHandler_t handler, int fontsize)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetFontSize_core (_THIS, fontsize);
  else
    _SetFontSize_core (handler, fontsize);
}

/* SetFontWeight */
typedef void (*_SetFontWeight_ptr) (_ObjectHandler_t objptr, FontWeight_t fontweight);
_UNIONPTR(SetFontWeight);

static void
SetFontWeight (_ObjectHandler_t handler, FontWeight_t fontweight)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetFontWeight_core (_THIS, fontweight);
  else
    _SetFontWeight_core (handler, fontweight);
}

/* SetPeriod */
typedef void (*_SetPeriod_ptr) (_ObjectHandler_t objptr, int period);
_UNIONPTR(SetPeriod);

static void
SetPeriod (_ObjectHandler_t handler, int period)
{
  if (!_Active)
    return;

  if (handler == NULL)
    _SetPeriod_core (_THIS, period);
  else
    _SetPeriod_core (handler, period);
}

/* GetColor */
typedef Color_t (*_GetColor_ptr) (_ObjectHandler_t objptr);
_UNIONPTR(GetColor);

static Color_t
GetColor (_ObjectHandler_t handler)
{
  if (!_Active)
    return 0;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return 0;
  }

  if (handler == NULL)
    return _GetColor_core (_THIS);
  else
    return _GetColor_core (handler);
}

/* GetPeriod */
typedef int (*_GetPeriod_ptr) (_ObjectHandler_t objptr);
_UNIONPTR(GetPeriod);

static int
GetPeriod (_ObjectHandler_t handler)
{
  if (!_Active)
    return 0;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return 0;
  }

  if (handler == NULL)
    return _GetPeriod_core (_THIS);
  else
    return _GetPeriod_core (handler);
}

/* SetThickness */
typedef void (*_SetThickness_ptr) (_ObjectHandler_t objptr, int thickness);
_UNIONPTR(SetThickness);

static void
SetThickness (_ObjectHandler_t handler, int thickness)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetThickness_core (_THIS, thickness);
  else
    _SetThickness_core (handler, thickness);
}

/* AttachText */
typedef void (*_AttachText_ptr) (_ObjectHandler_t objptr, const char *id, double price);
_UNIONPTR(AttachText);

static void
AttachText (_ObjectHandler_t handler, const char *id, double price)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _AttachText_core (_THIS, id, price);
  else
    _AttachText_core (handler, id, price);
}

/* SetSymbol */
typedef void (*_SetSymbol_ptr) (_ObjectHandler_t objptr, const char *symbol);
_UNIONPTR(SetSymbol);

static void
SetSymbol (_ObjectHandler_t handler, const char *symbol)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetSymbol_core (_THIS, symbol);
  else
    _SetSymbol_core (handler, symbol);
}

/* SetHAdjustment */
typedef void (*_SetHAdjustment_ptr) (_ObjectHandler_t objptr, int adj);
_UNIONPTR(SetHAdjustment);

static void
SetHAdjustment (_ObjectHandler_t handler, int adj)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (adj < TEXT_HADJUST_NORMAL || adj > TEXT_HADJUST_RIGHT)
  {
    _disable (__func__, CGSERR_INVALID_ARGUMENT);
    return;
  }

  if (handler == NULL)
    _SetHAdjustment_core (_THIS, adj);
  else
    _SetHAdjustment_core (handler, adj);
}

/* SetVAdjustment */
typedef void (*_SetVAdjustment_ptr) (_ObjectHandler_t objptr, int adj);
_UNIONPTR(SetVAdjustment);

static void
SetVAdjustment (_ObjectHandler_t handler, int adj)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (adj < TEXT_VADJUST_NORMAL || adj > TEXT_VADJUST_BELOW)
  {
    _disable (__func__, CGSERR_INVALID_ARGUMENT);
    return;
  }

  if (handler == NULL)
    _SetVAdjustment_core (_THIS, adj);
  else
    _SetVAdjustment_core (handler, adj);
}

/* SetSubChartRange */
static void
SetSubChartRange (double min, double max)
{
  if (_Initialized)
  {
    _disable (__func__, CGSERR_ALREADY_INITIALIZED);
    return;
  }

  _rangemin = min;
  _rangemax = max;

  return;
}

/* SetRefresh */
typedef void (*_SetRefresh_ptr) (_ObjectHandler_t objptr);
_UNIONPTR(SetRefresh);

static void
SetRefresh (_ObjectHandler_t handler)
{
  if (!_Active)
    return;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (handler == NULL)
    _SetRefresh_core (_THIS);
  else
    _SetRefresh_core (handler);
}
/*** End of objects ***/

/*** Input variables ***/
/* InputIntegerVariable */
static void
InputIntegerVariable (const char *name, int value, int min, int max, int *var, bool show)
{
  if (_Initialized)
  {
    _disable (__func__, CGSERR_ALREADY_INITIALIZED);
    return;
  }

  _InputVariableRegistryRec ivar;

  strncpy (ivar.name, name, _VAR_NAME_SIZE);
  ivar.name[_VAR_NAME_SIZE] = 0;
  ivar.type = _VAR_INTEGER;
  ivar.defaultValue = (double) value;
  ivar.minValue = (double) min;
  ivar.maxValue = (double) max;
  ivar.intVar = var;
  ivar.show = (show == true? 1 : 0);
  if (ArrayAppend (_InputVariableRegistry, (void *) &ivar) == -1)
    _disable (__func__, CGSERR_INTERNAL_101);

  return;
}

/* InputDoubleVariable */
static void
InputDoubleVariable (const char *name, double value, double min, double max,
                     double *var, bool show)
{
  if (_Initialized)
  {
    _disable (__func__, CGSERR_ALREADY_INITIALIZED);
    return;
  }

  _InputVariableRegistryRec ivar;

  strncpy (ivar.name, name, _VAR_NAME_SIZE);
  ivar.name[_VAR_NAME_SIZE] = 0;
  ivar.type = _VAR_DOUBLE;
  ivar.defaultValue = value;
  ivar.minValue = min;
  ivar.maxValue = max;
  ivar.doubleVar = var;
  ivar.show = (show == true? 1 : 0);
  if (ArrayAppend (_InputVariableRegistry, (void *) &ivar) == -1)
    _disable (__func__, CGSERR_INTERNAL_101);

  return;
}

/* InputColorVariable */
static void
InputColorVariable (const char *name, Color_t value, Color_t *var, bool show)
{
  if (_Initialized)
  {
    _disable (__func__, CGSERR_ALREADY_INITIALIZED);
    return;
  }

  _InputVariableRegistryRec ivar;

  strncpy (ivar.name, name, _VAR_NAME_SIZE);
  ivar.name[_VAR_NAME_SIZE] = 0;
  ivar.type = _VAR_COLOR;
  ivar.defaultValue = (double) value;
  ivar.colorVar = var;
  ivar.show = (show == true? 1 : 0);
  if (ArrayAppend (_InputVariableRegistry, (void *) &ivar) == -1)
    _disable (__func__, CGSERR_INTERNAL_101);

  return;
}
/*** End of input variables ***/

/*** Technical Analysis ***/
/* ADX */
typedef _Array_t (*_fADX_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fADX);

static _Array_t
fADX (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fADX_core (_PARENT_CHART, tf, period);
}

/* ATR */
typedef _Array_t (*_fATR_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fATR);

static _Array_t
fATR (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fATR_core (_PARENT_CHART, tf, period);
}

/* AROONUP */
typedef _Array_t (*_fAROONUP_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fAROONUP);

static _Array_t
fAROONUP (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fAROONUP_core (_PARENT_CHART, tf, period);
}

/* AROONDOWN */
typedef _Array_t (*_fAROONDOWN_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fAROONDOWN);

static _Array_t
fAROONDOWN (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fAROONDOWN_core (_PARENT_CHART, tf, period);
}

/* BBANDSUPPER */
typedef _Array_t (*_fBBANDSUPPER_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fBBANDSUPPER);

static _Array_t
fBBANDSUPPER (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fBBANDSUPPER_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* BBANDSLOWER */
typedef _Array_t (*_fBBANDSLOWER_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fBBANDSLOWER);

static _Array_t
fBBANDSLOWER (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fBBANDSLOWER_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* BBANDSMIDDLE */
typedef _Array_t (*_fBBANDSMIDDLE_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fBBANDSMIDDLE);

static _Array_t
fBBANDSMIDDLE (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fBBANDSMIDDLE_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* CCI */
typedef _Array_t (*_fCCI_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fCCI);

static _Array_t
fCCI (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fCCI_core (_PARENT_CHART, tf, period);
}

/* DMX */
typedef _Array_t (*_fDMX_ptr) (const void *chartptr, TimeFrame_t tf, int period);
_UNIONPTR(fDMX);

static _Array_t
fDMX (TimeFrame_t tf, int period)
{
  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  return _fDMX_core (_PARENT_CHART, tf, period);
}

/* EMA */
typedef _Array_t (*_fEMA_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fEMA);

static _Array_t
fEMA (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr = NULL;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fEMA_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* Generic EMA */
typedef _Array_t (*_gEMA_ptr) (const void *chartptr, int period, _Array_t data);
_UNIONPTR(gEMA);

static _Array_t
gEMA (int period, _Array_t data)
{
  _Array_t arr = NULL;

  if (!_Active)
    return NULL;

  arr = _gEMA_core (_PARENT_CHART, period, data);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* STOCHSLOWK */
typedef _Array_t (*_fSTOCHSLOWK_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fSTOCHSLOWK);

static _Array_t
fSTOCHSLOWK (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSTOCHSLOWK_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* STOCHSLOWD */
typedef _Array_t (*_fSTOCHSLOWD_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fSTOCHSLOWD);

static _Array_t
fSTOCHSLOWD (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSTOCHSLOWD_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* STOCHFASTK */
typedef _Array_t (*_fSTOCHFASTK_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fSTOCHFASTK);

static _Array_t
fSTOCHFASTK (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSTOCHFASTK_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* STOCHFASTD */
typedef _Array_t (*_fSTOCHFASTD_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fSTOCHFASTD);

static _Array_t
fSTOCHFASTD (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSTOCHFASTD_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* MACD */
typedef _Array_t (*_fMACD_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period, Price_t appliedprice);
_UNIONPTR(fMACD);

static _Array_t
fMACD (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fMACD_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* MACDSIGNAL */
typedef _Array_t (*_fMACDSIGNAL_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period, Price_t appliedprice);
_UNIONPTR(fMACDSIGNAL);

static _Array_t
fMACDSIGNAL (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fMACDSIGNAL_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* MACDHIST */
typedef _Array_t (*_fMACDHIST_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period, Price_t appliedprice);
_UNIONPTR(fMACDHIST);

static _Array_t
fMACDHIST (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fMACDHIST_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* MFI */
typedef _Array_t (*_fMFI_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fMFI);

static _Array_t
fMFI (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fMFI_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* MOMENTUM */
typedef _Array_t (*_fMOMENTUM_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fMOMENTUM);

static _Array_t
fMOMENTUM (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fMOMENTUM_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* PSAR */
typedef _Array_t (*_fPSAR_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fPSAR);

static _Array_t
fPSAR (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fPSAR_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* ROC */
typedef _Array_t (*_fROC_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fROC);

static _Array_t
fROC (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fROC_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* RSI */
typedef _Array_t (*_fRSI_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fRSI);

static _Array_t
fRSI (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fRSI_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* SMA */
typedef _Array_t (*_fSMA_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fSMA);

static _Array_t
fSMA (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSMA_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* Generic SMA */
typedef _Array_t (*_gSMA_ptr) (const void *chartptr, int period, _Array_t data);
_UNIONPTR(gSMA);

static _Array_t
gSMA (int period, _Array_t data)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  arr = _gSMA_core (_PARENT_CHART, period, data);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* STDDEV */
typedef _Array_t (*_fSTDDEV_ptr) (const void *chartptr, TimeFrame_t tf, int period,
                              Price_t appliedprice);
_UNIONPTR(fSTDDEV);

static _Array_t
fSTDDEV (TimeFrame_t tf, int period, Price_t appliedprice)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fSTDDEV_core (_PARENT_CHART, tf, period, appliedprice);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/* WILLR */
typedef _Array_t (*_fWILLR_ptr) (const void *chartptr, TimeFrame_t tf,
                                     int period);
_UNIONPTR(fWILLR);

static _Array_t
fWILLR (TimeFrame_t tf, int period)
{
  _Array_t arr;

  if (!_Active)
    return NULL;

  if (!_validtf(tf))
  {
    _disable (__func__, CGSERR_TIMEFRAME);
    return NULL;
  }

  arr = _fWILLR_core (_PARENT_CHART, tf, period);

  if (arr == NULL)
  {
    _disable (__func__, CGSERR_MEMORY);
    return NULL;
  }
  return arr;
}

/*** End of technical analysis */

/* ValueSet */
static void
ValueSet (_ObjectHandler_t objptr, double value, int shift)
{
  _Array_t dynvset = NULL;

  if (!_Initialized)
  {
    _disable (__func__, CGSERR_UNINITIALIZED);
    return;
  }

  if (!_Active)
    return;

  if (shift < 0 || shift > NBars (TF_CURRENT) - 1)
    return;

  if (objptr == NULL)
  {
    _ArrayPut (_ValueSet, shift, (void *) &value);
    return;
  }

  dynvset = _GetDynamicValueSet (objptr);
  _ArrayPut (dynvset, shift, (void *) &value);
}

/* printf (3) hook. redirects output to debug console */
typedef union
{
  _printf_ptr printf;
  void *ptr;
} _printf_union;

/*
 *   function pointers assignments: must remain the last function of bottom.h
 */
static void
_AssignCGScriptFunctionsToPtrs (size_t regsize)
{
  _printf_union printf_u;
  _GetFunctionFromRegistry (regsize, "printf", &(printf_u.ptr));
  printf = printf_u.printf;

#ifdef DEBUG_BUILD
#ifndef __TINYC__
  _SUPPRESS_WARNING(_cprintf);
#endif /* __TINYC__ */
#endif /* DEBUG_BUILD */
  _SUPPRESS_WARNING(_ArrayCleanup);
  _SUPPRESS_WARNING(_StringCleanup);
  _SUPPRESS_WARNING(_ObjectCleanup);
  _SUPPRESS_WARNING(GetFirstRun);
  _SUPPRESS_WARNING(Debug);
  _SUPPRESS_WARNING(ModuleThreadId);
  _SUPPRESS_WARNING(NeedsUpdate);
  _SUPPRESS_WARNING(LoopCounter);
  _SUPPRESS_WARNING(SetObjectType);
  _SUPPRESS_WARNING(InputIntegerVariable);
  _SUPPRESS_WARNING(InputDoubleVariable);
  _SUPPRESS_WARNING(InputColorVariable);


  _ASSIGNFUNC2PTR(GetParentTitle);
  _ASSIGNFUNC2PTR(GetModuleName);

  _InitValueSet_union InitValueSet_u;
  _GetFunctionFromRegistry (regsize, "InitValueSet", &(InitValueSet_u.ptr));
  _InitValueSet_core = InitValueSet_u._InitValueSet;
  _SUPPRESS_WARNING(_InitValueSet);
  _SUPPRESS_WARNING(ValueSet);

  _GetDynamicValueSet_union GetDynamicValueSet_u;
  _GetFunctionFromRegistry (regsize, "GetDynamicValueSet", &(GetDynamicValueSet_u.ptr));
  _GetDynamicValueSet_core = GetDynamicValueSet_u._GetDynamicValueSet;
  _SUPPRESS_WARNING(_GetDynamicValueSet);

  _SetDebugMode_union SetDebugMode_u;
  _GetFunctionFromRegistry (regsize, "SetDebugMode", &(SetDebugMode_u.ptr));
  _SetDebugMode_core = SetDebugMode_u._SetDebugMode;
  _SUPPRESS_WARNING(_SetDebugMode);

  _ASSIGNFUNC2PTR(GetFractionalDigits);

  _ASSIGNFUNC2PTR(StrError2);
  _SUPPRESS_WARNING(StrError);

  _ASSIGNFUNC2PTR(ChartHeight);
  _ASSIGNFUNC2PTR(ChartWidth);
  _ASSIGNFUNC2PTR(ChartForeColor);
  _ASSIGNFUNC2PTR(ChartBackColor);
  _ASSIGNFUNC2PTR(ChartCurrentTF);
  _ASSIGNFUNC2PTR(ChartCurrentBar);

  _ASSIGNFUNC2PTR(Bar);
  _SUPPRESS_WARNING(BarHA);
  _ASSIGNFUNC2PTR(NBars);
  _SUPPRESS_WARNING(NBarsHA);
  _ASSIGNFUNC2PTR(NVisibleBars);
  _ASSIGNFUNC2PTR(NewestVisibleBar);
  _ASSIGNFUNC2PTR(OldestVisibleBar);

  _ASSIGNFUNC2PTR(ArrayCreate);
  _ASSIGNFUNC2PTR(ArrayAppend);

  _ArrayPut_union ArrayPut_u;
  _GetFunctionFromRegistry (regsize, "ArrayPut", &(ArrayPut_u.ptr));
  _ArrayPut_core = ArrayPut_u._ArrayPut;
  _SUPPRESS_WARNING(ArrayPut);

  _ASSIGNFUNC2PTR(ArrayGet);
  _ASSIGNFUNC2PTR(ArraySize);
  _ASSIGNFUNC2PTR(ArrayReset);
  _ASSIGNFUNC2PTR(ArrayExists);

  _ArrayDestroy_union ArrayDestroy_u;
  _GetFunctionFromRegistry (regsize, "ArrayDestroy", &(ArrayDestroy_u.ptr));
  _ArrayDestroy_core = ArrayDestroy_u._ArrayDestroy;
  _SUPPRESS_WARNING(_ArrayDestroy);

  _ArrayDestroyAll_union ArrayDestroyAll_u;
  _GetFunctionFromRegistry (regsize, "ArrayDestroyAll", &(ArrayDestroyAll_u.ptr));
  _ArrayDestroyAll_core = ArrayDestroyAll_u._ArrayDestroyAll;
  _SUPPRESS_WARNING(_ArrayDestroyAll);

  _ObjectDestroy_union ObjectDestroy_u;
  _GetFunctionFromRegistry (regsize, "ObjectDestroy", &(ObjectDestroy_u.ptr));
  _ObjectDestroy_core = ObjectDestroy_u._ObjectDestroy;
  _SUPPRESS_WARNING(_ObjectDestroy);

  _ASSIGNFUNC2PTR(AddText);
  _ASSIGNFUNC2PTR(AddLabel);
  _ASSIGNFUNC2PTR(AddHorizontalLine);
  _ASSIGNFUNC2PTR(AddVerticalLine);
  _ASSIGNFUNC2PTR(AddCurve);
  _ASSIGNFUNC2PTR(AddVerticalBars);
  _ASSIGNFUNC2PTR(RemoveAllChildren);
  _ASSIGNFUNC2PTR(RemoveChild);
  _ASSIGNFUNC2PTR(SetText);
  _ASSIGNFUNC2PTR(SetPrice);
  _ASSIGNFUNC2PTR(SetColor);
  _ASSIGNFUNC2PTR(SetColorRGB);
  _ASSIGNFUNC2PTR(GetColor);
  _ASSIGNFUNC2PTR(SetXY);
  _ASSIGNFUNC2PTR(SetFontSize);
  _ASSIGNFUNC2PTR(SetPeriod);
  _ASSIGNFUNC2PTR(GetPeriod);
  _ASSIGNFUNC2PTR(SetFontWeight);
  _ASSIGNFUNC2PTR(SetThickness);
  _ASSIGNFUNC2PTR(AttachText);
  _ASSIGNFUNC2PTR(SetSymbol);
  _ASSIGNFUNC2PTR(SetHAdjustment);
  _ASSIGNFUNC2PTR(SetVAdjustment);
  _ASSIGNFUNC2PTR(SetInputVariables);
  _ASSIGNFUNC2PTR(SetRange);
  _ASSIGNFUNC2PTR(SetRefresh);

  _StringDestroy_union StringDestroy_u;
  _GetFunctionFromRegistry (regsize, "StringDestroy", &(StringDestroy_u.ptr));
  _StringDestroy_core = StringDestroy_u._StringDestroy;
  _SUPPRESS_WARNING(_StringDestroy);

  _StringDestroyAll_union StringDestroyAll_u;
  _GetFunctionFromRegistry (regsize, "StringDestroyAll", &(StringDestroyAll_u.ptr));
  _StringDestroyAll_core = StringDestroyAll_u._StringDestroyAll;
  _SUPPRESS_WARNING(_StringDestroyAll);

  _ASSIGNFUNC2PTR(StrCpy);
  _ASSIGNFUNC2PTR(StrCat);
  _ASSIGNFUNC2PTR(Cstr2Str);
  _ASSIGNFUNC2PTR(Str2NCstr);
  _ASSIGNFUNC2PTR(StrLen);
  _ASSIGNFUNC2PTR(StrCmp);
  _ASSIGNFUNC2PTR(StrCstrCmp);
  _ASSIGNFUNC2PTR(Str2Cstr);
  _ASSIGNFUNC2PTR(StrInit);

  _ASSIGNFUNC2PTR(fADX);
  _ASSIGNFUNC2PTR(fATR);
  _ASSIGNFUNC2PTR(fAROONUP);
  _ASSIGNFUNC2PTR(fAROONDOWN);
  _ASSIGNFUNC2PTR(fBBANDSUPPER);
  _ASSIGNFUNC2PTR(fBBANDSLOWER);
  _ASSIGNFUNC2PTR(fBBANDSMIDDLE);
  _ASSIGNFUNC2PTR(fCCI);
  _ASSIGNFUNC2PTR(fDMX);
  _ASSIGNFUNC2PTR(fEMA);
  _ASSIGNFUNC2PTR(gEMA);
  _ASSIGNFUNC2PTR(fSTOCHSLOWK);
  _ASSIGNFUNC2PTR(fSTOCHSLOWD);
  _ASSIGNFUNC2PTR(fSTOCHFASTK);
  _ASSIGNFUNC2PTR(fSTOCHFASTD);
  _ASSIGNFUNC2PTR(fMACD);
  _ASSIGNFUNC2PTR(fMACDSIGNAL);
  _ASSIGNFUNC2PTR(fMACDHIST);
  _ASSIGNFUNC2PTR(fMFI);
  _ASSIGNFUNC2PTR(fMOMENTUM);
  _ASSIGNFUNC2PTR(fPSAR);
  _ASSIGNFUNC2PTR(fROC);
  _ASSIGNFUNC2PTR(fRSI);
  _ASSIGNFUNC2PTR(fSMA);
  _ASSIGNFUNC2PTR(gSMA);
  _ASSIGNFUNC2PTR(fSTDDEV);
  _ASSIGNFUNC2PTR(fWILLR);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif

