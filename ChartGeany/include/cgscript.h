/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef CGSCRIPT_H
#define CGSCRIPT_H

#include <QtGlobal>

#ifdef __cplusplus
#include "qtachart_object.h"
#include "qtachart_core.h"
#include "defs.h"

// cast const void * to QTACsomething *
extern QTACObject * QTACastFromConstVoid (const void *ptr);

#else /* standard C */

#include <QtGlobal>

#ifndef Q_OS_WIN
#include <stdbool.h>
#else
#define bool int
#endif

#define NOEXCEPT

/* export */
#ifndef Q_DECL_EXPORT
#ifdef Q_OS_WIN
#define Q_DECL_EXPORT       __declspec(dllexport)
#else
#define Q_DECL_EXPORT
#endif
#endif /* Q_DECL_EXPORT */

#endif /* __cplusplus */

#include <stdarg.h>
#include "top.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FUNCTION_NAME_SIZE      56

/* CGSCript function registry record */
typedef struct
{
  void *ptr;                        /* function's pointer */
  char name[FUNCTION_NAME_SIZE];    /* function name (55 chars + NULL) */
} CGSCriptFunctionRegistryRec;

/* max dimension of CGScriptFunctionRegistry_ptr */
#define REGISTRY_DIM    1024

/* returns the function registry */
/* extern void * CGScriptFunctionRegistry_ptr (); */

/*** Hooks ***/
/* printf implementation */
extern Q_DECL_EXPORT int printf_imp (const char *str, va_list ap);

/*** errors ***/
/* fill the error registry */
extern Q_DECL_EXPORT void fill_error_registry   ();

/* StrError2 implementation */
extern Q_DECL_EXPORT const char * StrError2_imp (const ObjectHandler_t objptr, ErrCode_t code);

/*** Chart ***/
/* ChartWidth implementation */
extern Q_DECL_EXPORT int ChartWidth_imp (const void *ptr) NOEXCEPT;

/* ChartHeight implementation */
extern Q_DECL_EXPORT int ChartHeight_imp (const void *ptr) NOEXCEPT;

/* ChartForeColor */
extern Q_DECL_EXPORT Color_t ChartForeColor_imp (const void *ptr) NOEXCEPT;;

/* ChartBackColor */
extern Q_DECL_EXPORT Color_t ChartBackColor_imp (const void *ptr) NOEXCEPT;;

/* CurrentTF */
extern Q_DECL_EXPORT TimeFrame_t ChartCurrentTF_imp (const void *ptr) NOEXCEPT;

/* CurrentBar */
extern Q_DECL_EXPORT int ChartCurrentBar_imp (const void *ptr) NOEXCEPT;

/*** Common ***/
/* GetParentTitle implementation */
extern Q_DECL_EXPORT String_t GetParentTitle_imp (const void *ptr);

/* GetModuleName implementation */
extern Q_DECL_EXPORT String_t GetModuleName_imp (const ObjectHandler_t objptr);

/* GetFractionalDigits implementation */
extern Q_DECL_EXPORT int GetFractionalDigits_imp (double num);

/* GetDynamicValueSet implementation */
extern Q_DECL_EXPORT Array_t GetDynamicValueSet_imp (const ObjectHandler_t objptr);

/* InitValueSet implementation */
extern Q_DECL_EXPORT Array_t InitValueSet_imp (ObjectHandler_t objptr);

/* SetDebugMode implementation */
extern Q_DECL_EXPORT void SetDebugMode_imp (ObjectHandler_t objptr, bool mode);

/*** Bars ***/
/* Bar implementation */
extern Q_DECL_EXPORT BarData_t Bar_imp (const void *ptr, Candle_t ct, TimeFrame_t tf, int shift);

/* NBars implementation */
extern Q_DECL_EXPORT int NBars_imp (const void *ptr, Candle_t ct, TimeFrame_t tf);

/* NVisibleBars implementation */
extern Q_DECL_EXPORT int NVisibleBars_imp (const void *ptr);

/* NewestVisibleBar implementation */
extern Q_DECL_EXPORT int NewestVisibleBar_imp (const void *ptr) NOEXCEPT;

/* OldestVisibleBar implementation */
extern Q_DECL_EXPORT int OldestVisibleBar_imp (const void *ptr) NOEXCEPT;

/*** Arrays ***/
/* Create array implementation */
extern Q_DECL_EXPORT Array_t ArrayCreate_imp (void *objptr, int elemsize, int maxd);

/* Create array with id implementation */
extern Q_DECL_EXPORT Array_t ArrayCreate2_imp (const void *objptr, const char *id, int elemsize, int maxd);

/* Append element in array implementation */
extern Q_DECL_EXPORT int ArrayAppend_imp (Array_t arr, void *elem);

/* Put element where the index says implementation */
extern Q_DECL_EXPORT void ArrayPut_imp (void *arrptr, int idx, void *elem);

/* Get element from array implementation */
extern Q_DECL_EXPORT void *ArrayGet_imp (Array_t arr, int index);

/* Get array size implementation */
extern Q_DECL_EXPORT int ArraySize_imp (Array_t arr);

/* Reset array implementation */
extern Q_DECL_EXPORT void ArrayReset_imp (Array_t arr);

/* Destroy all arrays of an object implementation */
extern Q_DECL_EXPORT void ArrayDestroyAll_imp (const void *objptr);

/* Destroy an array implementation */
extern Q_DECL_EXPORT void ArrayDestroy_imp (void *arrptr);

/* Check array existense implementation */
extern Q_DECL_EXPORT int ArrayExists_imp (const void *arrptr);

/*** Object ***/
/* Destroy an object implementation */
extern Q_DECL_EXPORT void ObjDestroy_imp (ObjectHandler_t objptr);

/* AddText implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddText_imp (void *ptr, ObjectHandler_t objptr);

/* AddLabel implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddLabel_imp (void *ptr, ObjectHandler_t objptr);

/* AddHorizontalLine implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddHorizonalLine_imp (void *ptr, ObjectHandler_t objptr);

/* AddVerticalLine implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddVerticalLine_imp (void *ptr, ObjectHandler_t objptr, const char *id);

/* AddCurve implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddCurve_imp (void *ptr, ObjectHandler_t objptr);

/* AddVBars implementation */
extern Q_DECL_EXPORT ObjectHandler_t ObjAddVBars_imp (void *ptr, ObjectHandler_t objptr);

/* RemoveAllChildren implementation */
extern Q_DECL_EXPORT void ObjRemoveAllChildren_imp (ObjectHandler_t objptr);

/* RemoveChild implementation */
extern Q_DECL_EXPORT void ObjRemoveChild_imp (ObjectHandler_t objptr, void *child);

/* ObjSetText implementation */
extern Q_DECL_EXPORT void ObjSetText_imp (ObjectHandler_t objptr, String_t text);

/* ObjSetPrice_imp */
extern Q_DECL_EXPORT void ObjSetPrice_imp (ObjectHandler_t objptr, double price);

/* ObjSetColor implementation */
extern Q_DECL_EXPORT void ObjSetColor_imp (ObjectHandler_t objptr, Color_t color);

/* ObjSetColorRGB implementation */
extern Q_DECL_EXPORT void ObjSetColorRGB_imp (ObjectHandler_t objptr, int r, int g, int b);

/* ObjSetXY implementation */
extern Q_DECL_EXPORT void ObjSetXY_imp (ObjectHandler_t objptr, int x, int y);

/* ObjSetFontSize implementation */
extern Q_DECL_EXPORT void ObjSetFontSize_imp (ObjectHandler_t objptr, int fontsize);

/* ObjSetFontWeight implementation */
extern Q_DECL_EXPORT void ObjSetFontWeight_imp (ObjectHandler_t objptr, FontWeight_t fontweight);

/* ObjGetColor implementation */
extern Q_DECL_EXPORT Color_t ObjGetColor_imp (ObjectHandler_t objptr);

/* ObjGetPeriod implementation */
extern Q_DECL_EXPORT int ObjGetPeriod_imp (ObjectHandler_t objptr);

/* ObjSetPeriod implementation */
extern Q_DECL_EXPORT void ObjSetPeriod_imp (ObjectHandler_t objptr, int period);

/* ObjSetThickness implementation */
extern Q_DECL_EXPORT void ObjSetThickness_imp (ObjectHandler_t objptr, int thickness);

/* ObjAttachText implementation */
extern Q_DECL_EXPORT void ObjAttachText_imp (ObjectHandler_t objptr, const char *id, double price);

/* ObjSetSymbol implementation */
extern Q_DECL_EXPORT void ObjSetSymbol_imp (ObjectHandler_t objptr, const char *symbol);

/* ObjSetHAdjustment implementation */
extern Q_DECL_EXPORT void ObjSetHAdjustment_imp (ObjectHandler_t objptr, int adj);

/* ObjSetVAdjustment implementation */
extern Q_DECL_EXPORT void ObjSetVAdjustment_imp (ObjectHandler_t objptr, int adj);

/* ObjSetInputVariables implementation */
extern Q_DECL_EXPORT void
ObjSetInputVariables_imp (ObjectHandler_t objptr, Array_t InputVariableRegistry);

/* ObjSetRange implementation */
extern Q_DECL_EXPORT void
ObjSetRange_imp (ObjectHandler_t objptr, double min, double max);

/* ObjRefresh implementation */
extern Q_DECL_EXPORT void ObjRefresh_imp (ObjectHandler_t objptr);

/*** Strings ***/
/* StringDestroyAll_imp */
extern Q_DECL_EXPORT void StringDestroyAll_imp (const ObjectHandler_t objptr);

/* StrCpy_imp */
extern Q_DECL_EXPORT String_t StrCpy_imp (String_t dst, String_t src);

/* StrCat_imp */
extern Q_DECL_EXPORT String_t StrCat_imp (String_t dst, String_t src);

/* StrCmp_imp */
extern Q_DECL_EXPORT int StrCmp_imp (String_t s1, String_t s2);

/* StrCstrCmp_imp */
extern Q_DECL_EXPORT int StrCstrCmp_imp (String_t s1, const char *s2);

/* StrLen_imp */
extern Q_DECL_EXPORT size_t StrLen_imp (String_t str);

/* Cstr2Str_imp */
extern Q_DECL_EXPORT String_t Cstr2Str_imp (const ObjectHandler_t objptr, String_t dst, const char *src);

/* Str2NCstr_imp */
extern Q_DECL_EXPORT char *Str2NCstr_imp (char *cstr, String_t str, size_t n);

/*  Str2Cstr_imp */
extern Q_DECL_EXPORT const char * Str2Cstr_imp (String_t str);

/* StrInit_imp */
extern Q_DECL_EXPORT String_t StrInit_imp (const ObjectHandler_t objptr, const char *initstr);

/* StrInit2_imp */
extern Q_DECL_EXPORT String_t StrInit2_imp (const ObjectHandler_t objptr, const char *id, const char *initstr);

extern Q_DECL_EXPORT void StringDestroy_imp (const ObjectHandler_t objptr, String_t str);

/* StrGetWithId_imp */
extern Q_DECL_EXPORT String_t StrGetWithId_imp (const  ObjectHandler_t objptr, const char *id);

/*** Technical analysis functions */
/* ADX */
extern Q_DECL_EXPORT Array_t fADX_imp (const void *ptr, TimeFrame_t tf, int period);

/* ATR */
extern Q_DECL_EXPORT Array_t fATR_imp (const void *ptr, TimeFrame_t tf, int period);

/* AROONUP */
extern Q_DECL_EXPORT Array_t fAROONUP_imp (const void *ptr, TimeFrame_t tf, int period);

/* AROONDOWN */
extern Q_DECL_EXPORT Array_t fAROONDOWN_imp (const void *ptr, TimeFrame_t tf, int period);

/* BBANDSUP */
extern Q_DECL_EXPORT Array_t fBBANDSUPPER_imp (const void *ptr, TimeFrame_t tf, int period,
    Price_t appliedprice);

/* BBANDSDOWN */
extern Q_DECL_EXPORT Array_t fBBANDSLOWER_imp (const void *ptr, TimeFrame_t tf, int period,
    Price_t appliedprice);

/* BBANDSMIDDLE */
extern Q_DECL_EXPORT Array_t fBBANDSMIDDLE_imp (const void *ptr, TimeFrame_t tf, int period,
    Price_t appliedprice);

/* CCI */
extern Q_DECL_EXPORT Array_t fCCI_imp (const void *ptr, TimeFrame_t tf, int period);

/* DMX */
extern Q_DECL_EXPORT Array_t fDMX_imp (const void *ptr, TimeFrame_t tf, int period);

/* EMA */
extern Q_DECL_EXPORT Array_t fEMA_imp (const void *ptr, TimeFrame_t tf, int period,
                                       Price_t appliedprice);

/* Generic EMA */
extern Q_DECL_EXPORT Array_t gEMA_imp (const void *ptr, int period, Array_t data);

/* STOCHSLOWK */
extern Q_DECL_EXPORT Array_t fSTOCHSLOWK_imp (const void *ptr, TimeFrame_t tf,
    int period);

/* STOCHSLOWD */
extern Q_DECL_EXPORT Array_t fSTOCHSLOWD_imp (const void *ptr, TimeFrame_t tf,
    int period);

/* STOCHFASTK */
extern Q_DECL_EXPORT Array_t fSTOCHFASTK_imp (const void *ptr, TimeFrame_t tf,
    int period);

/* STOCHFASTD */
extern Q_DECL_EXPORT Array_t fSTOCHFASTD_imp (const void *ptr, TimeFrame_t tf,
    int period);

/* MACD */
extern Q_DECL_EXPORT Array_t fMACD_imp (const void *ptr, TimeFrame_t tf,
                                        int period, Price_t appliedprice);

/* MACDSIGNAL */
extern Q_DECL_EXPORT Array_t fMACDSIGNAL_imp (const void *ptr, TimeFrame_t tf,
    int period, Price_t appliedprice);

/* MACDHIST */
extern Q_DECL_EXPORT Array_t fMACDHIST_imp (const void *ptr, TimeFrame_t tf,
    int period, Price_t appliedprice);

/* MFI */
extern Q_DECL_EXPORT Array_t fMFI_imp (const void *ptr, TimeFrame_t tf,
                                       int period);

/* MOMENTUM */
extern Q_DECL_EXPORT Array_t fMOMENTUM_imp (const void *ptr, TimeFrame_t tf,
    int period, Price_t appliedprice);

/* PSAR */
extern Q_DECL_EXPORT Array_t fPSAR_imp (const void *ptr, TimeFrame_t tf,
                                        int period);

/* ROC */
extern Q_DECL_EXPORT Array_t fROC_imp (const void *ptr, TimeFrame_t tf,
                                       int period, Price_t appliedprice);

/* RSI */
extern Q_DECL_EXPORT Array_t fRSI_imp (const void *ptr, TimeFrame_t tf,
                                       int period, Price_t appliedprice);

/* SMA */
extern Q_DECL_EXPORT Array_t fSMA_imp (const void *ptr, TimeFrame_t tf,
                                       int period, Price_t appliedprice);

/* Generic SMA */
extern Q_DECL_EXPORT Array_t gSMA_imp (const void *ptr, int period, Array_t data);

/* STDDEV */
extern Q_DECL_EXPORT Array_t fSTDDEV_imp (const void *ptr, TimeFrame_t tf,
    int period, Price_t appliedprice);

/* WILLR */
extern Q_DECL_EXPORT Array_t fWILLR_imp (const void *ptr, TimeFrame_t tf,
    int period);

/*** System (cgscript.c) ***/
/* Initialize cgscript: returns the number of registered functions */
extern Q_DECL_EXPORT size_t cgscript_init ();

#ifdef __cplusplus
}
#endif

#endif /* CGSCRIPT_H */
