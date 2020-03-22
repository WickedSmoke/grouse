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

#include <cfloat>
#include <memory>

#include "qtachart_core.h"
#include "cgscript.h"

// ObjDestroy_imp
extern "C" Q_DECL_EXPORT void
ObjDestroy_imp (ObjectHandler_t objptr)
{
  if (objptr == nullptr)
    return;

  QTACObject *pobj = static_cast <QTACObject *> (objptr);

  pobj->setForDelete ();
}


// ObjAddLabel_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddLabel_imp (void *ptr, ObjectHandler_t objptr)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (ptr);
  QTACObject *pobj = static_cast <QTACObject *> (objptr),
             *obj = nullptr;

  if (pobj->type == QTACHART_OBJ_SUBCHART)
    return nullptr;

  obj = new (std::nothrow) QTACObject (ptr, QTACHART_OBJ_LABEL);
  if (obj == nullptr)
    return nullptr;



  QGraphicsTextItem *t = new (std::nothrow) QGraphicsTextItem;
  if (t == nullptr)
  {
    obj->setForDelete ();
    return nullptr;
  }

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);
    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  t->setZValue (1.0);
  t->setDefaultTextColor (core->forecolor);
  core->scene->qtcAddItem (t);

  pobj->Object += obj;
  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setTitle ("Label");
  obj->setText (t, 100, 100);

  return static_cast <ObjectHandler_t> (obj);
}

// ObjAddText_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddText_imp (void *ptr, ObjectHandler_t objptr)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (ptr);
  QTACObject *pobj = static_cast <QTACObject *> (objptr);

  QTACObject *obj = new (std::nothrow) QTACObject (ptr, QTACHART_OBJ_TEXT);
  if (obj == nullptr)
    return nullptr;



  QGraphicsTextItem *t = new (std::nothrow) QGraphicsTextItem;
  if (t == nullptr)
  {
    obj->setForDelete ();
    return nullptr;
  }

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);
    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  t->setZValue (1.0);
  t->setDefaultTextColor (core->forecolor);
  core->scene->qtcAddItem (t);

  pobj->Object += obj;
  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setTitle ("Text");

  obj->setText (t, core->TIMEFRAME.at (0).HLOC.at (0).Text,
                core->TIMEFRAME.at (0).HLOC.at (0).Close);

  return static_cast <ObjectHandler_t> (obj);
}

// ObjAddHorizonalLine_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddHorizonalLine_imp (void *ptr, ObjectHandler_t objptr)
{
  QTACObject *pobj = static_cast <QTACObject *> (objptr),
             *obj = nullptr;

  if (pobj->type == QTACHART_OBJ_SUBCHART)
    obj = new (std::nothrow) QTACObject (pobj, QTACHART_OBJ_HLINE);
  else
    obj = new (std::nothrow) QTACObject (ptr, QTACHART_OBJ_HLINE);

  if (obj == nullptr)
    return nullptr;

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);
    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  pobj->Object += obj;
  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setHLine (nullptr, 0);
  obj->setTitle ("Horizontal Line");

  return static_cast <ObjectHandler_t> (obj);
}

// ObjAddVerticaline_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddVerticalLine_imp (void *ptr, ObjectHandler_t objptr, const char *id)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (ptr);
  QTACObject *pobj = static_cast <QTACObject *> (objptr),
             *obj = nullptr;

  if (pobj->type == QTACHART_OBJ_SUBCHART)
    return nullptr;

  obj = new (std::nothrow) QTACObject (ptr, QTACHART_OBJ_VLINE);

  if (obj == nullptr)
    return nullptr;



  QGraphicsLineItem *vline = new (std::nothrow) QGraphicsLineItem;
  if (vline == nullptr)
  {
    obj->setForDelete ();
    return nullptr;
  }

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);
    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  core->scene->qtcAddItem (vline);

  pobj->Object += obj;
  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setVLine (vline, QString (id));
  obj->setTitle ("Vertical Line");

  return static_cast <ObjectHandler_t> (obj);
}

// ObjAddCurve_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddCurve_imp (void *ptr, ObjectHandler_t objptr)
{
  QTACObject *pobj = static_cast <QTACObject *> (objptr),
             *obj = nullptr;

  if (pobj->type == QTACHART_OBJ_SUBCHART)
    obj = new (std::nothrow) QTACObject (pobj, QTACHART_OBJ_CURVE);
  else
    obj = new (std::nothrow) QTACObject (ptr, QTACHART_OBJ_CURVE);

  if (obj == nullptr)
    return nullptr;

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);
    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setTitle ("Curve");

  if (obj->paramDialog == nullptr) // this is a child
  {
    ParamVector pvector;
    obj->setParamDialog (pvector, "Curve");
    obj->paramDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"),
                           DPT_INT, obj->getPeriod ());
    obj->paramDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"),
                           DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  }

  /* this is needed for curve object as a child in a container object or a subchart */
  obj->setAttributes_common (QTACHART_CLOSE,
                             obj->period,
                             QStringLiteral ("Period"), obj->rmin, obj->rmax,
                             obj->paramDialog->getParam (QStringLiteral ("Color")),
                             QStringLiteral ("Color"));

  obj->dynvset = static_cast <void *> (InitValueSet_imp (obj));
  if (obj->dynvset == nullptr)
  {
    delete obj;
    return nullptr;
  }

  if (pobj->type != QTACHART_OBJ_SUBCHART)
    obj->valueSet ();

  return static_cast <ObjectHandler_t> (obj);
}

// ObjAddVBars_imp
extern "C" Q_DECL_EXPORT ObjectHandler_t
ObjAddVBars_imp (void *ptr, ObjectHandler_t objptr)
{
  Q_UNUSED (ptr)
  QTACObject *pobj = static_cast <QTACObject *> (objptr);
  QTACObject *obj = nullptr;

  if (pobj->type == QTACHART_OBJ_SUBCHART)
    obj = new (std::nothrow) QTACObject (pobj, QTACHART_OBJ_VBARS);
  else
    return nullptr;

  if (obj == nullptr)
    return nullptr;

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj1 = QTACastFromConstVoid (objptr);

    if (obj1 != nullptr)
      obj1->sanitizer->cgoInc ();
  }

  obj->dynamic = true;
  obj->parentModule = pobj;
  obj->setTitle ("Vertical Bars");

  if (obj->paramDialog == nullptr) // this is a child
  {
    ParamVector pvector;
    obj->setParamDialog (pvector, "Vertical Bars");
    obj->paramDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"),
                           DPT_INT, obj->getPeriod ());
    obj->paramDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"),
                           DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  }

  /* this is needed for vbars object as a child in a container object or a subchart */
  obj->setAttributes_common (QTACHART_CLOSE,
                             obj->period,
                             QStringLiteral (""), pobj->rmin, pobj->rmax,
                             obj->paramDialog->getParam (QStringLiteral ("")),
                             QStringLiteral (""));

  obj->dynvset = static_cast <void *> (InitValueSet_imp (obj));
  if (obj->dynvset == nullptr)
  {
    delete obj;
    return nullptr;
  }

  obj->valueSet ();

  return static_cast <ObjectHandler_t> (obj);
}

// ObjRemoveAllChildren_imp
extern "C" Q_DECL_EXPORT void
ObjRemoveAllChildren_imp (ObjectHandler_t objptr)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->removeAllChildren ();
  return;
}

// ObjRemovelChild_imp
extern "C" Q_DECL_EXPORT void
ObjRemoveChild_imp (ObjectHandler_t objptr, void *childptr)
{
  QTACObject *obj = static_cast <QTACObject *> (objptr),
             *child = static_cast <QTACObject *> (childptr);

  obj->removeChild (child);
  return;
}

// ObjSetText_imp
extern "C" Q_DECL_EXPORT void
ObjSetText_imp (ObjectHandler_t objptr, String_t text)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->titlestr = QString (Str2Cstr_imp (text));
  // obj->text->setPlainText (obj->titlestr);
  obj->text->setHtml (obj->titlestr);

  if (obj->type == QTACHART_OBJ_CURVE)
    // obj->title->setPlainText (obj->titlestr % QStringLiteral ("(") % (obj->period > 0?QString::number (obj->period, 'f', 0):QStringLiteral ("")) % QStringLiteral (")") );
    obj->title->setHtml (obj->titlestr % QStringLiteral ("(") % (obj->period > 0?QString::number (obj->period, 'f', 0):QStringLiteral ("")) % QStringLiteral (")") );

  return;
}

// ObjSetPrice_imp
extern "C" Q_DECL_EXPORT void
ObjSetPrice_imp (ObjectHandler_t objptr, double price)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (obj->type == QTACHART_OBJ_HLINE)
    obj->setHLine (obj->hvline, static_cast <qreal> (price));
  else
    obj->price = static_cast <qreal> (price);
  return;
}

// ObjSetColor_imp
extern "C" Q_DECL_EXPORT void
ObjSetColor_imp (ObjectHandler_t objptr, Color_t color)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);

  if (!obj->enabled || obj->deleteit)
    return;

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT ||
      obj->type == QTACHART_OBJ_HLINE || obj->type == QTACHART_OBJ_VLINE)
  {
    obj->text->setDefaultTextColor (QColor ((static_cast <QRgb> (color))));
    obj->title->setVisible (false);

    if (obj->hvline != nullptr)
      obj->hvline->setPen (QPen (QColor (static_cast <QRgb> (color))));
  }
  else
  {
    obj->forecolor = QColor (static_cast <QRgb> (color));

    QTACObject *pobj = obj->parentObject;

    if (pobj == nullptr) // not part of a subchart
    {
      DynParamsDialog *dynparam = obj->getParamDialog ();
      ParamVector pvector = dynparam->getPVector ();

      foreach (DynParam *dparam, pvector)
      {
        if (dparam->paramName == obj->colorParamName)
          dparam->value = dparam->defvalue = static_cast <qreal> (obj->forecolor.rgb ());
      }
    }
  }

  return;
}

// ObjSetColorRGB_imp
extern "C" Q_DECL_EXPORT void
ObjSetColorRGB_imp (ObjectHandler_t objptr, int r, int g, int b)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  QColor color = QColor (r, g, b, 255);

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT)
  {
    obj->text->setDefaultTextColor (color);
    obj->title->setVisible (false);
  }
  else
  {
    obj->forecolor = color;
    QTACObject *pobj = obj->parentObject;

    if (pobj == nullptr) // not part of a subchart
    {
      DynParamsDialog *dynparam = obj->getParamDialog ();
      ParamVector pvector = dynparam->getPVector ();

      foreach (DynParam *dparam, pvector)
      {
        if (dparam->paramName == obj->colorParamName)
          dparam->value = dparam->defvalue = static_cast <qreal> (color.rgb ());
      }
    }
  }

  return;
}

// ObjSetXY_imp
extern "C" Q_DECL_EXPORT void
ObjSetXY_imp (ObjectHandler_t objptr, int x, int y)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT)
    obj->setText (obj->text, x, y);

  return;
}

// ObjSetFontSize_imp
extern "C" Q_DECL_EXPORT void
ObjSetFontSize_imp (ObjectHandler_t objptr, int fontsize)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (fontsize < 7)
    fontsize = 7;

  if (fontsize > 24)
    fontsize = 24;

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT)
  {
    QFont fnt;
    fnt = obj->text->font ();
    fnt.setPointSize (fontsize);
    obj->text->setFont (fnt);
  }

  return;
}

// ObjSetFontWeight_imp
extern "C" Q_DECL_EXPORT void
ObjSetFontWeight_imp (ObjectHandler_t objptr, FontWeight_t fontweight)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT)
  {
    QFont fnt;
    fnt = obj->text->font ();
    fnt.setWeight (fontweight);
    obj->text->setFont (fnt);
  }

  return;
}

// ObjSetPeriod_imp
extern "C" Q_DECL_EXPORT void
ObjSetPeriod_imp (ObjectHandler_t objptr, int period)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (period < 0)
    period = 0;

  obj->setPeriod (period);

  return;
}

// ObjGetColor_imp
extern "C" Q_DECL_EXPORT Color_t
ObjGetColor_imp (ObjectHandler_t objptr)
{
  const QTACObject *obj =  static_cast <QTACObject *> (objptr);

  if (!obj->enabled || obj->deleteit)
    return 0;

  if (obj->type == QTACHART_OBJ_LABEL || obj->type == QTACHART_OBJ_TEXT ||
      obj->type == QTACHART_OBJ_HLINE || obj->type == QTACHART_OBJ_VLINE)
  {
    if (obj->hvline != nullptr)
      return obj->hvline->pen().color().rgb ();

    return obj->text->defaultTextColor().rgb ();
  }

  return obj->forecolor.rgb ();
}

// ObjGetPeriod_imp
extern "C" Q_DECL_EXPORT int
ObjGetPeriod_imp (ObjectHandler_t objptr)
{
  const QTACObject *obj =  static_cast <QTACObject *> (objptr);

  return obj->getPeriod ();
}

// ObjSetThickness_imp
extern "C" Q_DECL_EXPORT void
ObjSetThickness_imp (ObjectHandler_t objptr, int thickness)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->setThickness (thickness);

  return;
}

// ObjAttachText_imp
extern "C" Q_DECL_EXPORT void
ObjAttachText_imp (ObjectHandler_t objptr, const char *id, double price)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->setText (nullptr, QString (id), static_cast <qreal> (price));
}

// ObjSetSymbol_imp
extern "C" Q_DECL_EXPORT void
ObjSetSymbol_imp (ObjectHandler_t objptr, const char *symbol)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->setSymbol (symbol);
}

// ObjSetHAdjustment_imp
extern "C" Q_DECL_EXPORT void
ObjSetHAdjustment_imp (ObjectHandler_t objptr, int adj)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->setHAdjustment (adj);
}

// ObjSetVAdjustment_imp
extern "C" Q_DECL_EXPORT void
ObjSetVAdjustment_imp (ObjectHandler_t objptr, int adj)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->setVAdjustment (adj);
}

// ObjSetInputVariables_imp
extern "C" Q_DECL_EXPORT void
ObjSetInputVariables_imp (ObjectHandler_t objptr, Array_t InputVariableRegistry)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  ParamVector pvector;
  // pvector = obj->getParamDialog ()->getPVector ();

  _InputVariableRegistryRec *var;
  int dim = ArraySize_imp (InputVariableRegistry);

  for (int counter = 0; counter < dim; counter ++)
  {
    int type = 0;

    var = (_InputVariableRegistryRec *) ArrayGet_imp (InputVariableRegistry, counter);
    if (var->type == _VAR_INTEGER) type = DPT_INT;
    if (var->type == _VAR_DOUBLE) type = DPT_REAL;
    if (var->type == _VAR_COLOR) type = DPT_COLOR;
    QString name = QString (var->name);

    DynParam *param = new DynParam (name);
    param->type = type;
    param->show = var->show == 1 ? true : false;

    if (type == DPT_COLOR)
    {
      param->defvalue = QColor ((QRgb) ((Color_t) (var->defaultValue))).rgb ();
      param->callback_var = static_cast <void *> (var->colorVar);
    }
    else if (type == DPT_INT)
    {
      param->defvalue = var->defaultValue;
      param->callback_var = static_cast <void *> (var->intVar);
    }
    else if (type == DPT_REAL)
    {
      param->defvalue = var->defaultValue;
      param->callback_var = static_cast <void *> (var->doubleVar);
    }

    pvector += param;
  }

  // if (obj->type == QTACHART_OBJ_SUBCHART || obj->type == QTACHART_OBJ_CONTAINER)
  if (pvector.size () > 0)
    obj->setParamDialog (pvector, obj->moduleName);

  foreach (DynParam *param, pvector)
    delete param;

  pvector.resize (0);
}

// ObjSetRange_imp
extern "C" Q_DECL_EXPORT void
ObjSetRange_imp (ObjectHandler_t objptr, double min, double max)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  if (min <= DBL_MIN)
    min = static_cast <double> (QREAL_MIN);

  if (max >= DBL_MAX)
    max = static_cast <double> (QREAL_MAX);

  obj->setRange (static_cast <qreal> (min),
                 static_cast <qreal> (max));
}

// ObjRefresh_imp
extern "C" Q_DECL_EXPORT void
ObjRefresh_imp (ObjectHandler_t objptr)
{
  QTACObject *obj =  static_cast <QTACObject *> (objptr);
  if (!obj->enabled || obj->deleteit)
    return;

  obj->needsupdate = true;
  foreach (QTACObject *child, obj->children)
    child->needsupdate = true;
}
