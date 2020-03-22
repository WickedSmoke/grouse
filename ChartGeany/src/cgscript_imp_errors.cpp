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

#include <QString>
#include <QVector>
#include "defs.h"
#include "cgscript.h"

// Error registry record
typedef struct alignas (max_align_t)
{
  ErrCode_t err;        // error code
  QString msg;          // error message
} ErrorRegistryRecord;
Q_DECLARE_TYPEINFO (ErrorRegistryRecord, Q_MOVABLE_TYPE);

// Error registry
static QVector <ErrorRegistryRecord> ErrorRegistry;

// Register an error
static void
RegisterError (ErrCode_t code, QString message)
{
  ErrorRegistryRecord rec;
  rec.err = code;
  rec.msg = message;
  ErrorRegistry.append (rec);
}

// Fill the error registry
void
fill_error_registry ()
{
  Q_UNUSED (QTACastFromConstVoid)

  RegisterError (CGSERR_OK, QStringLiteral (" No error "));
  RegisterError (CGSERR_INITIALIZATION_FAILED, QStringLiteral (" Initialization failed "));
  RegisterError (CGSERR_RUNTIME_ERROR, QStringLiteral (" Run-time error "));
  RegisterError (CGSERR_UNINITIALIZED, QStringLiteral (" Uninitialized module "));
  RegisterError (CGSERR_ALREADY_INITIALIZED, QStringLiteral (" Module is initialized "));
  RegisterError (CGSERR_NO_PARENT_CHART, QStringLiteral (" No parent chart"));
  RegisterError (CGSERR_NO_MODULE, QStringLiteral (" No module "));
  RegisterError (CGSERR_NO_ARRAY, QStringLiteral (" No array "));
  RegisterError (CGSERR_ARRAY_CREATION_AFTER_INIT, QStringLiteral (" Array creation outside Init() "));
  RegisterError (CGSERR_ARRAY_ELEM_BIG, QStringLiteral (" Array element too big "));
  RegisterError (CGSERR_ARRAY_ELEM_ZERO, QStringLiteral (" Array element size is zero "));
  RegisterError (CGSERR_ARRAY_CREATION_FAILED, QStringLiteral (" Array creation failed "));
  RegisterError (CGSERR_ARRAY_SUBSCRIPT, QStringLiteral (" Array subscript out of range "));
  RegisterError (CGSERR_MEMORY, QStringLiteral (" Out of memory "));
  RegisterError (CGSERR_TIMEFRAME, QStringLiteral (" Invalid time frame "));
  RegisterError (CGSERR_INVALID_ARGUMENT, QStringLiteral (" Invalid argument "));
  RegisterError (CGSERR_NEGATIVE_SHIFT, QStringLiteral (" Negative shift "));
  RegisterError (CGSERR_INVALID_MODTYPE, QStringLiteral (" Invalid module type "));
  RegisterError (CGSERR_INTERNAL_100, QStringLiteral (" Internal error 100 ")); // function not found in registry
  RegisterError (CGSERR_INTERNAL_101, QStringLiteral (" Internal error 101 ")); // input variable creation failed
  RegisterError (CGSERR_FPE, QStringLiteral (" Floating point exception "));
  RegisterError (CGSERR_SEGV, QStringLiteral (" Segmentation fault "));
  RegisterError (CGSERR_ILL, QStringLiteral (" Illegal instruction "));
  RegisterError (CGSERR_SIGNAL, QStringLiteral (" Signal not handled "));
}

// StrError2: Return string describing error number
extern "C" Q_DECL_EXPORT const char *
StrError2_imp (const ObjectHandler_t objptr, ErrCode_t code)
{
  String_t Serrormsg;
  int maxrec = ErrorRegistry.size (), counter = 0;

  Serrormsg = StrInit2_imp (objptr, "ErrorMessage", " Unknown error ");
  while (counter < maxrec)
  {
    if (ErrorRegistry.at (counter).err == code)
    {
      Cstr2Str_imp (objptr, Serrormsg, ErrorRegistry.at (counter).msg.toStdString().c_str());
      counter = maxrec;
    }
    else
      counter ++;
  }

  return Str2Cstr_imp (Serrormsg);
}
