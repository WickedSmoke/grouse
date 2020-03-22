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

#include <stdlib.h>
#include <string.h>
#include "cgscript.h"

/*** stdio.h ***/
/* printf_cgs: printf (3) hook wrapper */
int printf_cgs (const char *str, ...)
{
  int rslt;

  va_list ap;
  va_start(ap, str);
  rslt = printf_imp (str, ap);
  va_end(ap);

  return (int) rslt;
}

/*** CGSCript infrastructure ***/
/* fill CGScriptFunctionRegistry */
static CGSCriptFunctionRegistryRec CGScriptFunctionRegistry[REGISTRY_DIM];

/* returns CGScriptFunctionRegistry_ptr */
void *
CGScriptFunctionRegistry_ptr ()
{
  return (void *) CGScriptFunctionRegistry;
}

/* compare function for qsort */
static int
RegistryRecCompar (const void *v1, const void *v2)
{
  const CGSCriptFunctionRegistryRec
   *r1 = (const CGSCriptFunctionRegistryRec *) v1,
   *r2 = (const CGSCriptFunctionRegistryRec *) v2;
  int rslt;

  rslt = strncmp (r1->name, r2->name, FUNCTION_NAME_SIZE);
  return rslt;
}

/* add function to list */
int
addFunctionToRegistry (const char *name, void *ptr)
{
  int counter = 0;

  while (counter < REGISTRY_DIM && CGScriptFunctionRegistry[counter].name[0] != 0)
    counter ++;

  if (counter == (REGISTRY_DIM - 1))
    return -1; /* registry full */

  CGScriptFunctionRegistry[counter].name[FUNCTION_NAME_SIZE - 1] = 0;
  strncpy (CGScriptFunctionRegistry[counter].name, name, FUNCTION_NAME_SIZE - 1);
  CGScriptFunctionRegistry[counter].ptr = ptr;

  return 0;  /* function added */
}

/* initialize */
extern size_t
cgscript_init ()
{
  size_t counter, DIM = 0;

  fill_error_registry ();

  for (counter = 0; counter < REGISTRY_DIM; counter ++)
  {
    CGScriptFunctionRegistry[counter].name[0] = 0;
    CGScriptFunctionRegistry[counter].ptr = NULL;
  }

  /* stdio.h */
  addFunctionToRegistry ("printf", (void *) (printf_cgs));

  /* error functions */
  addFunctionToRegistry ("StrError2", (void *) (StrError2_imp));

  /* chart functions */
  addFunctionToRegistry ("ChartWidth", (void *) (ChartWidth_imp));
  addFunctionToRegistry ("ChartHeight", (void *) (ChartHeight_imp));
  addFunctionToRegistry ("ChartForeColor", (void *) (ChartForeColor_imp));
  addFunctionToRegistry ("ChartBackColor", (void *) (ChartBackColor_imp));
  addFunctionToRegistry ("ChartCurrentTF", (void *) (ChartCurrentTF_imp));
  addFunctionToRegistry ("ChartCurrentBar", (void *) (ChartCurrentBar_imp));

  /* common functions */
  addFunctionToRegistry ("GetParentTitle", (void *) (GetParentTitle_imp));
  addFunctionToRegistry ("GetModuleName", (void *) (GetModuleName_imp));
  addFunctionToRegistry ("InitValueSet", (void *) (InitValueSet_imp));
  addFunctionToRegistry ("GetFractionalDigits", (void *) (GetFractionalDigits_imp));
  addFunctionToRegistry ("GetDynamicValueSet", (void *) (GetDynamicValueSet_imp));
  addFunctionToRegistry ("SetDebugMode", (void *) (SetDebugMode_imp));

  /* bar functions */
  addFunctionToRegistry ("Bar", (void *) (Bar_imp));
  addFunctionToRegistry ("NBars", (void *) (NBars_imp));
  addFunctionToRegistry ("NVisibleBars", (void *) (NVisibleBars_imp));
  addFunctionToRegistry ("NewestVisibleBar", (void *) (NewestVisibleBar_imp));
  addFunctionToRegistry ("OldestVisibleBar", (void *) (OldestVisibleBar_imp));

  /* array functions */
  addFunctionToRegistry ("ArrayCreate", (void *) (ArrayCreate_imp));
  addFunctionToRegistry ("ArrayAppend", (void *) (ArrayAppend_imp));
  addFunctionToRegistry ("ArrayPut", (void *) (ArrayPut_imp));
  addFunctionToRegistry ("ArrayGet", (void *) (ArrayGet_imp));
  addFunctionToRegistry ("ArraySize", (void *) (ArraySize_imp));
  addFunctionToRegistry ("ArrayReset", (void *) (ArrayReset_imp));
  addFunctionToRegistry ("ArrayDestroy", (void *) (ArrayDestroy_imp));
  addFunctionToRegistry ("ArrayDestroyAll", (void *) (ArrayDestroyAll_imp));
  addFunctionToRegistry ("ArrayExists", (void *) (ArrayExists_imp));

  /* object functions */
  addFunctionToRegistry ("AddLabel", (void *) (ObjAddLabel_imp));
  addFunctionToRegistry ("AddText", (void *) (ObjAddText_imp));
  addFunctionToRegistry ("AddHorizontalLine", (void *) (ObjAddHorizonalLine_imp));
  addFunctionToRegistry ("AddVerticalLine", (void *) (ObjAddVerticalLine_imp));
  addFunctionToRegistry ("AddCurve", (void *) (ObjAddCurve_imp));
  addFunctionToRegistry ("AddVerticalBars", (void *) (ObjAddVBars_imp));
  addFunctionToRegistry ("AttachText", (void *) (ObjAttachText_imp));
  addFunctionToRegistry ("ObjectDestroy", (void *) (ObjDestroy_imp));
  addFunctionToRegistry ("RemoveAllChildren", (void *) (ObjRemoveAllChildren_imp));
  addFunctionToRegistry ("RemoveChild", (void *) (ObjRemoveChild_imp));
  addFunctionToRegistry ("SetText", (void *) (ObjSetText_imp));
  addFunctionToRegistry ("SetPrice", (void *) (ObjSetPrice_imp));
  addFunctionToRegistry ("SetColor", (void *) (ObjSetColor_imp));
  addFunctionToRegistry ("SetColorRGB", (void *) (ObjSetColorRGB_imp));
  addFunctionToRegistry ("GetColor", (void *) (ObjGetColor_imp));
  addFunctionToRegistry ("SetXY", (void *) (ObjSetXY_imp));
  addFunctionToRegistry ("SetFontSize", (void *) (ObjSetFontSize_imp));
  addFunctionToRegistry ("SetFontWeight", (void *) (ObjSetFontWeight_imp));
  addFunctionToRegistry ("SetPeriod", (void *) (ObjSetPeriod_imp));
  addFunctionToRegistry ("GetPeriod", (void *) (ObjGetPeriod_imp));
  addFunctionToRegistry ("SetThickness", (void *) (ObjSetThickness_imp));
  addFunctionToRegistry ("SetSymbol", (void *) (ObjSetSymbol_imp));
  addFunctionToRegistry ("SetHAdjustment", (void *) (ObjSetHAdjustment_imp));
  addFunctionToRegistry ("SetVAdjustment", (void *) (ObjSetVAdjustment_imp));
  addFunctionToRegistry ("SetInputVariables", (void *) (ObjSetInputVariables_imp));
  addFunctionToRegistry ("SetRange", (void *) (ObjSetRange_imp));
  addFunctionToRegistry ("SetRefresh", (void *) (ObjRefresh_imp));

  /* string functions */
  addFunctionToRegistry ("StringDestroy", (void *) (StringDestroy_imp));
  addFunctionToRegistry ("StringDestroyAll", (void *) (StringDestroyAll_imp));
  addFunctionToRegistry ("StrCpy", (void *) (StrCpy_imp));
  addFunctionToRegistry ("StrCat", (void *) (StrCat_imp));
  addFunctionToRegistry ("StrCmp", (void *) (StrCmp_imp));
  addFunctionToRegistry ("StrCstrCmp", (void *) (StrCstrCmp_imp));
  addFunctionToRegistry ("StrLen", (void *) (StrLen_imp));
  addFunctionToRegistry ("Cstr2Str", (void *) (Cstr2Str_imp));
  addFunctionToRegistry ("Str2NCstr", (void *) (Str2NCstr_imp));
  addFunctionToRegistry ("Str2Cstr", (void *) (Str2Cstr_imp));
  addFunctionToRegistry ("StrInit", (void *) (StrInit_imp));

  /* technical analysis functions */
  addFunctionToRegistry ("fADX", (void *) (fADX_imp));
  addFunctionToRegistry ("fATR", (void *) (fATR_imp));
  addFunctionToRegistry ("fAROONUP", (void *) (fAROONUP_imp));
  addFunctionToRegistry ("fAROONDOWN", (void *) (fAROONDOWN_imp));
  addFunctionToRegistry ("fBBANDSUPPER", (void *) (fBBANDSUPPER_imp));
  addFunctionToRegistry ("fBBANDSLOWER", (void *) (fBBANDSLOWER_imp));
  addFunctionToRegistry ("fBBANDSMIDDLE", (void *) (fBBANDSMIDDLE_imp));
  addFunctionToRegistry ("fCCI", (void *) (fCCI_imp));
  addFunctionToRegistry ("fDMX", (void *) (fDMX_imp));
  addFunctionToRegistry ("fEMA", (void *) (fEMA_imp));
  addFunctionToRegistry ("gEMA", (void *) (gEMA_imp));
  addFunctionToRegistry ("fSTOCHSLOWK", (void *) (fSTOCHSLOWK_imp));
  addFunctionToRegistry ("fSTOCHSLOWD", (void *) (fSTOCHSLOWD_imp));
  addFunctionToRegistry ("fSTOCHFASTK", (void *) (fSTOCHFASTK_imp));
  addFunctionToRegistry ("fSTOCHFASTD", (void *) (fSTOCHFASTD_imp));
  addFunctionToRegistry ("fMACD", (void *) (fMACD_imp));
  addFunctionToRegistry ("fMACDSIGNAL", (void *) (fMACDSIGNAL_imp));
  addFunctionToRegistry ("fMACDHIST", (void *) (fMACDHIST_imp));
  addFunctionToRegistry ("fMFI", (void *) (fMFI_imp));
  addFunctionToRegistry ("fMOMENTUM", (void *) (fMOMENTUM_imp));
  addFunctionToRegistry ("fPSAR", (void *) (fPSAR_imp));
  addFunctionToRegistry ("fROC", (void *) (fROC_imp));
  addFunctionToRegistry ("fRSI", (void *) (fRSI_imp));
  addFunctionToRegistry ("fSMA", (void *) (fSMA_imp));
  addFunctionToRegistry ("gSMA", (void *) (gSMA_imp));
  addFunctionToRegistry ("fSTDDEV", (void *) (fSTDDEV_imp));
  addFunctionToRegistry ("fWILLR", (void *) (fWILLR_imp));

  /* sort the registry */
  while (CGScriptFunctionRegistry[DIM].ptr != NULL) DIM++;

  qsort (CGScriptFunctionRegistry_ptr (), DIM,
         sizeof (CGSCriptFunctionRegistryRec), RegistryRecCompar);

  return DIM;
}
