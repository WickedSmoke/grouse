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

#include "qtachart_core.h"
#include "cgscript.h"

QTACObject *
QTACastFromConstVoid (const void *ptr)
{
  QObject *obj = static_cast <QObject *> (const_cast <void *> (ptr));

  if (obj->metaObject()->className() == QStringLiteral ("QTACObject"))
    return qobject_cast <QTACObject *> (obj);

  return nullptr;
}

// GetParentTitle
extern "C" Q_DECL_EXPORT String_t
GetParentTitle_imp (const void *ptr)
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);
  String_t rslt;

  rslt = StrGetWithId_imp ((ObjectHandler_t) ptr, "ParentTitle");
  if (rslt != nullptr)
    return rslt;

  rslt = StrInit2_imp ((ObjectHandler_t) ptr, "ParentTitle", core->getTabText ().toUtf8().constData());
  return rslt;
}

// GetModuleName
extern "C" Q_DECL_EXPORT String_t
GetModuleName_imp (const ObjectHandler_t objptr)
{
  const QTACObject *obj =  static_cast <const QTACObject *> (objptr);
  String_t rslt;

  rslt = StrGetWithId_imp (objptr, "ModuleName");
  if (rslt != nullptr)
    return rslt;

  rslt = StrInit2_imp (objptr, "ModuleName", obj->moduleName.toUtf8().constData ());

  return rslt;
}

#define ARRAYID(p)  QString (QString::number ((quint64) p)).toStdString().c_str()

// InitValueSet
extern "C" Q_DECL_EXPORT Array_t
InitValueSet_imp (ObjectHandler_t objptr)
{
  QTACObject *obj = static_cast <QTACObject *> (objptr);
  const QTAChartCore *core = static_cast <const QTAChartCore *>
                             ((const void *) obj->chartdata);
  Array_t vset;
  int s = core->TIMEFRAME[0].HLOC.size ();

  vset = ArrayCreate2_imp (objptr, ARRAYID(objptr), sizeof (qreal), s);
  if (vset == nullptr)
    return vset;

  for (int counter = 0; counter < s; counter ++)
  {
    qreal d = core->TIMEFRAME[0].HLOC[0].Close;
    if (ArrayAppend_imp (vset, static_cast <void *> (&d)) == -1)
    {
      ArrayDestroy_imp (vset);
      return nullptr;
    }
  }

  obj->valuesetsize = s;

  return vset;
}

// GetDynamicValueSet
extern "C" Q_DECL_EXPORT Array_t
GetDynamicValueSet_imp (const ObjectHandler_t objptr)
{
  const QTACObject *obj = static_cast <const QTACObject *> (objptr);

  return static_cast <Array_t> (obj->dynvset);
}

// GetFractionalDigits
extern "C" Q_DECL_EXPORT int
GetFractionalDigits_imp (double num)
{
  return fracdig (num);
}

// SetDebugMode
extern "C" Q_DECL_EXPORT void
SetDebugMode_imp (ObjectHandler_t objptr, bool mode)
{
  QTACObject *obj = static_cast <QTACObject *> (objptr);

  obj->setCGScriptDebug (mode);
}
