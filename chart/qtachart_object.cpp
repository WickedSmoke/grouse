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

#include <iostream>
#include <stdexcept>
#include <limits>
#include <QCoreApplication>
#include <QtCore/qmath.h>
#include <QApplication>
#include <QtGlobal>
#include "qtachart_core.h"
#include "qtachart.h"
#include "cgscript.h"
#include "debug.h"

// constructor of line edge class
LineEdge::LineEdge () NOEXCEPT
{
  pad = 0;
  txtdirection = 0;
  sequence = 1;
  pricetxt = nullptr;
}

// destructor of line edge class
LineEdge::~LineEdge ()
{

}

// constructor of close button on subcharts
SubChartButton::SubChartButton (QTACObject *object) NOEXCEPT
{
  owner = object;
  this->setAutoRaise (true);
  chartdata = owner->chartdata;
}

// destructor
SubChartButton::~SubChartButton ()
{

}

// get owner object
QTACObject *
SubChartButton::getOwner () NOEXCEPT
{
  return owner;
}

// constructor for modules
QTACObject::QTACObject (void *data, QString modpath, QString modname)
{
  sanitizer = new ObjectSanitizer (static_cast <const void *> (this));
  int objtype;

  period = 10;
  cgscriptdebug = false;
  modulePath = modpath;
  moduleName = modname.left(25);
  module.setFileName (modpath);
  if (!module.isLoaded ())
  {
    module.setLoadHints (QLibrary::ResolveAllSymbolsHint|
                         QLibrary::ExportExternalSymbolsHint);
    if (!module.load ())
    {
      enabled = false;
      debugdialog->appendText ("Cannot load module: " % modname % " failed with message '" % module.errorString() % "'");
      return;
    }
  }

  modinit = reinterpret_cast <ModuleInit> (module.resolve("_CGModuleInitObject"));
  if (modinit == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot initialize module: " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  modloop = reinterpret_cast <ModuleLoop> (module.resolve ("_CGModuleLoop"));
  if (modloop == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot use module's Loop(): " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  modevent = reinterpret_cast <ModuleEvent> (module.resolve ("_CGModuleEvent"));
  if (modevent == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot use module's Event(): " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  modfinish = reinterpret_cast <ModuleFinish> (module.resolve ("_CGModuleFinish"));
  if (modfinish == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot unload module: " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  modvset = reinterpret_cast <ModuleValueSet> (module.resolve ("_CGModuleValueSet"));
  if (modvset == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot use module's ValueSet(): " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  modcompiler = reinterpret_cast <ModuleCompiler> (module.resolve ("_CGModuleCompiler"));
  if (modcompiler == nullptr)
  {
    enabled = false;
    debugdialog->appendText ("Cannot determin compiler used for module: " % modname % " failed with message '" % module.errorString() % "'");
    return;
  }

  // run module's Init() function
  if (!moduleInit (data, &objtype))
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (data);
  chartdata = data;
  type = objtype;
  parentObject = nullptr;
  core->Object += this;

  QTACObject_constructor_common ();
  setTitle (moduleName);
  valueSet ();

  ParamVector Param;
  switch (type)
  {
    case QTACHART_OBJ_CURVE:
      setParamDialog (Param, moduleName, parent ());
      break;
    default:
      setParamDialog (Param, moduleName);
  }

  if (type == QTACHART_OBJ_LABEL || type == QTACHART_OBJ_TEXT)
  {
    text = new QGraphicsTextItem;
    text->setDefaultTextColor (core->forecolor);
    text->setVisible (false);
    text->setParent (this);
    scene->qtcAddItem (text);
    if (filter == nullptr)
    {
      filter = new QTACObjectEventFilter (text);
      text->installEventFilter (filter);
      text->setAcceptHoverEvents (true);
    }
  }
  else if (type == QTACHART_OBJ_CURVE)
  {
    paramDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"),
                           DPT_INT, getPeriod ());
    paramDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"),
                           DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
    paramDialog->setObjectName (QStringLiteral ("ParamDialog"));

    setAttributes_common (QTACHART_CLOSE,
                          period,
                          QStringLiteral ("Period"), 0, 0,
                          (QColor) paramDialog->getParam (QStringLiteral ("Color")),
                          QStringLiteral ("Color"));
  }
  else if (type == QTACHART_OBJ_SUBCHART)
  {
    setAttributes_common (QTACHART_CLOSE,
                          period,
                          QStringLiteral (""), rmin, rmax,
                          forecolor, QStringLiteral ("Color"));

  }

  // run module's Loop() function
  moduleLoop ();

  if (core->events_enabled == true)
    core->draw ();
}

// constuctor for objects on chart or subcharts
QTACObject::QTACObject (QTAChartCore *core, QTAChartObjectType objtype)
{
  sanitizer = new ObjectSanitizer (static_cast <const void *> (this));

  modinit = nullptr;
  modloop = nullptr;
  modevent = nullptr;
  modfinish = nullptr;
  modvset = nullptr;
  cgscriptdebug = false;
  chartdata = core;
  type = objtype;
  period = 10;
  parentObject = nullptr;
  core->Object += this;
  moduleName = QStringLiteral ("");
  QTACObject_constructor_common ();
}

// constructor
QTACObject::QTACObject (QTACObject *parentsubchart, QTAChartObjectType objtype)
{
  sanitizer = new ObjectSanitizer (static_cast <const void *> (this));
  modinit = nullptr;
  modloop = nullptr;
  modevent = nullptr;
  modfinish = nullptr;
  modvset = nullptr;
  cgscriptdebug = false;
  parentObject = parentsubchart;
  parentObject->children += this;
  chartdata = parentObject->chartdata;
  type = objtype;
  period = 10;
  moduleName = QStringLiteral ("");
  QTACObject_constructor_common ();
}

// constructor's common function
void
QTACObject::QTACObject_constructor_common ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QFont font;

  parentModule = nullptr;

  if (type == QTACHART_OBJ_LABEL)
  {
    qRegisterMetaType<RTPrice> ("RTPrice");
    connect(this ,SIGNAL(updateOnlinePrice (RTPrice)),
            this, SLOT (updateOnlinePriceSlot (RTPrice)));
  }

  core->reloaded = true;
  enabled = true;
  dynamic = false;
  subchart_dec = 1;
  quantum = 0;
  scene = core->scene;
  deleteit = false;
  onlineprice = false;
  needsupdate = true;
  dataset_type = QTACHART_CLOSE;
  filter = nullptr;
  dataset = nullptr;
  valueset = nullptr;
  TAfunc = nullptr;
  TAfunc2 = nullptr;
  dynvset = nullptr;
  // paramDialog = nullptr;
  ITEMS = nullptr;
  visibleitems = 0;
  ITEMSsize = 0;
  valuesetsize = 0;
  lastperiod = 0;
  thickness = 1;
  rmin = QREAL_MIN;
  rmax = QREAL_MAX;
  X1 = Y1 = X2 = Y2 = 0;
  trailerCandleText = QStringLiteral ("");
  price = 0;
  vadjust = QTACHART_OBJ_VADJUST_NORMAL;
  hadjust = QTACHART_OBJ_HADJUST_NORMAL;

  if (parentObject == nullptr)
    setParent (core);
  else
    setParent (parentObject);

  text = nullptr;
  bottomline = nullptr;
  hvline = nullptr;
  titlestr = QStringLiteral ("");
  datastr = QStringLiteral ("");

  title = new QGraphicsTextItem;

  if (type == QTACHART_OBJ_CURVE || type == QTACHART_OBJ_DOT)
  {
    if (filter == nullptr && parentObject == nullptr)
    {
      title->setParent (this);
      filter = new QTACObjectEventFilter (title);
      title->installEventFilter (filter);
      title->setAcceptHoverEvents (true);
    }
  }

  font = title->font ();
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setWeight (QFont::DemiBold);
  title->setFont (font);

  scene->qtcAddItem (title);
  closeBtn = nullptr;
  // editBtn = nullptr;

  if (type == QTACHART_OBJ_CONTAINER)
    title->setVisible (false);

  if (type == QTACHART_OBJ_SUBCHART)
  {
    core->setSizeChanged ();
    core->nsubcharts ++;
    closeBtn = core->addSCB (this, QStringLiteral ("Close"));
    closeBtn->setFixedSize (QSize (22, 22));
    closeicon =
      QIcon (QString (":/png/images/icons/PNG/Error_Symbol.png"));
    closeBtn->setIcon (closeicon);
    closeBtn->setIconSize (QSize (16, 16));
    closeBtn->setFocusPolicy (Qt::NoFocus);
    closeBtn->setStyleSheet (QStringLiteral ("background: transparent;color: white;font: 11px;"));
    prxcloseBtn = scene->qtcAddWidget (closeBtn, Qt::Widget);

    editBtn = core->addSCB (this, QStringLiteral ("Edit"));
    editBtn->setFixedSize (QSize (30, 16));
    editBtn->setText (QStringLiteral ("EDIT"));
    editBtn->setFocusPolicy (Qt::NoFocus);
    editBtn->setStyleSheet (QStringLiteral ("background: transparent; border: 1px solid transparent;border-color: darkgray;color: white; font: 9px;"));
    prxeditBtn = scene->qtcAddWidget (editBtn, Qt::Widget);

    if (paramDialog.isNull ())
      editBtn->setVisible (false);

    // no tooltip for title
    title->setToolTip (QStringLiteral (""));
  }

  if (type == QTACHART_OBJ_LINE ||
      type == QTACHART_OBJ_FIBO)
  {
    Edge += new LineEdge;
    Edge += new LineEdge;
    Edge[0]->pricetxt = new QGraphicsTextItem;
    Edge[1]->pricetxt = new QGraphicsTextItem;
    Edge[0]->pricetxt->setParent (this);
    Edge[1]->pricetxt->setParent (this);
    Edge[0]->pricetxt->installEventFilter (new QTACObjectEventFilter (Edge[0]->pricetxt));
    Edge[1]->pricetxt->installEventFilter (new QTACObjectEventFilter (Edge[1]->pricetxt));
    Edge[0]->pricetxt->setAcceptHoverEvents (true);
    Edge[1]->pricetxt->setAcceptHoverEvents (true);
    Edge[0]->pricetxt->setFont (font);
    Edge[1]->pricetxt->setFont (font);
    Edge[0]->pricetxt->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));
    Edge[1]->pricetxt->setToolTip (QStringLiteral ("<span style=\"font: 9px;color:black;\">Double click to edit</span>"));
  }

  if (type == QTACHART_OBJ_FIBO)
  {
    FiboLevelPrc << 0 << 0.236 << 0.382 << 0.5 << 0.618 << 0.764 << 1;
    for (qint32 counter = 0, maxcounter = FiboLevelPrc.size ();
         counter < maxcounter; counter ++)
    {
      // QGraphicsTextItem *textitem;
      FiboLevel += scene->qtcAddLine (0, 0, 0, 0);
      // textitem = new QGraphicsTextItem;
      // textitem->setParent (this);
      // FiboLevelLbl += textitem;
      // scene->qtcAddItem (textitem);
      scene->qtcAddItem (&FiboLevelLbl[counter]);
      // textitem = new QGraphicsTextItem;
      // textitem->setParent (this);
      // FiboLevelPrcLbl += textitem;
      scene->qtcAddItem (&FiboLevelPrcLbl[counter]);
    }
  }

  if (type != QTACHART_OBJ_LABEL && type != QTACHART_OBJ_TEXT)
  {
    text = title;
  }
}

// destructor's common function
void
QTACObject::QTACObject_destructor_common ()
{

  QPen pen;
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (type == QTACHART_OBJ_SUBCHART)
  {
    closeBtn->setVisible (false);
    scene->removeItem (prxcloseBtn);
    core->garbageQWidget += closeBtn;
    core->CloseSCB.remove (core->CloseSCB.indexOf (closeBtn));
    editBtn->setVisible (false);
    scene->removeItem (prxeditBtn);
    core->garbageQWidget += editBtn;
    core->CloseSCB.remove (core->CloseSCB.indexOf (editBtn));
    core->setSizeChanged ();
  }

  if (type == QTACHART_OBJ_FIBO)
  {
    for (qint32 counter = 0; counter < FiboLevelPrc.size (); counter ++)
    {
      FiboLevel[counter]->setVisible (false);
      scene->removeItem (FiboLevel[counter]);
      delete FiboLevel[counter];
    }
    FiboLevel.clear ();
  }

  foreach (const LineEdge *edge, Edge)
    delete edge;

  if (hvline != nullptr)
  {
    scene->removeItem (hvline);
    delete hvline;
    hvline = nullptr;
  }

  if (bottomline != nullptr)
  {
    scene->removeItem (bottomline);
    delete bottomline;
    bottomline = nullptr;
  }

  if (scene->items().contains (title))
    scene->removeItem (title);
  delete title;

  if (text == title)
    text = nullptr;
  title = nullptr;

  if (text != nullptr)
  {
    scene->removeItem (text);
    delete text;
    text = nullptr;
  }

  deleteITEMS ();
  delete[] ITEMS;

  if (children.size () != 0)
    foreach (const QTACObject *child, children)
    {
      delete child;
      child = nullptr;
    }

  if (type == QTACHART_OBJ_SUBCHART)
    core->nsubcharts -= subchart_dec;

  if (valueset != nullptr)
    delete valueset;

  if (!paramDialog.isNull ())
    delete paramDialog;

  core->clearITEMS ();

  removeAllChildren ();

  if (dynvset != nullptr)
    ArrayDestroy_imp (dynvset);

  removeAllChildren ();

  if (moduleName != "")
  {
    modfinish ();
  }

  if (moduleName != "")
  {
    if (module.isLoaded ())
    {
      if (module.unload ())
      {
        QDir dir;
        dir.remove (module.fileName ());
      }
    }
  }
}

// destructor
QTACObject::~QTACObject ()
{
  QTACObject_destructor_common ();

  if (CGSCRIPT_SANITIZER)
  {
    if (parentModule)
      parentModule->sanitizer->cgoDec ();
  }

  delete sanitizer;
}

// remove all children
void
QTACObject::removeAllChildren ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  foreach (QTACObject *obj1, Object)
  {
    foreach (QTACObject *obj2, core->Object)
    {
      if (obj1 == obj2)
        obj2->setForDelete ();
    }
  }
}

// remove child
void
QTACObject::removeChild (QTACObject *child)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QTACObject *obj1 = nullptr;
  int nobjects = Object.size (), counter = 0;

  while (counter < nobjects && child != Object[counter])
    counter++;

  if (counter < nobjects)
    obj1 = Object[counter];
  else
    return;

  foreach (QTACObject *obj2, core->Object)
    if (obj1 == obj2) obj2->setForDelete ();
}

// real time updates
static void
realTimeUpdates (RTPrice rtprice, QString tablename, QTAChart *chart)
{
  QString csvline, SQLCommand, xtablename;
  int rc;

  xtablename = tablename;
  xtablename = xtablename.replace (QStringLiteral ("_ADJUSTED"), QStringLiteral (""));
  csvline = rtprice.date % QStringLiteral (",") % rtprice.open % QStringLiteral (",") % rtprice.high % QStringLiteral (",") %
            rtprice.low % QStringLiteral (",") % rtprice.price % QStringLiteral (",") % rtprice.volume % QStringLiteral (",") %
            rtprice.price % QStringLiteral (",") % rtprice.time;
  SQLCommand  = QStringLiteral ("DELETE FROM ") % xtablename %
                QStringLiteral (" WHERE DATE = '") % rtprice.date % QStringLiteral ("';");
  SQLCommand += csvline2SQL (csvline, xtablename);

  rc = updatedb (SQLCommand);
  if (rc == SQLITE_OK)
  {
    chart->loadFrames (tablename);
    chart->loadFrames (tablename % QStringLiteral ("_WEEK"));
    chart->loadFrames (tablename % QStringLiteral ("_MONTH"));
    chart->loadFrames (tablename % QStringLiteral ("_YEAR"));
  }
}

// update online price signal emittion and slot
void
QTACObject::emitUpdateOnlinePrice (RTPrice rtprice)
{
  if (type != QTACHART_OBJ_LABEL)
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QTAChart *chart = qobject_cast <QTAChart *> (core->parent ());

  realTimeUpdates (rtprice, core->SymbolKey, chart);
  emit updateOnlinePrice (rtprice);
}

void
QTACObject::updateOnlinePriceSlot (RTPrice rtprice)
{
  if (type != QTACHART_OBJ_LABEL)
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  QString str = rtprice.price % QStringLiteral (" ") % rtprice.change %
                QStringLiteral (" ") % rtprice.prcchange;

  if (str == text->toPlainText ())
    return; // no update

  rtprice.prcchange.replace (QStringLiteral ("%"), QStringLiteral (" "));
  if (rtprice.prcchange.toFloat () > 0)
    text->setDefaultTextColor (QColor (0x00, 0xaa, 0x00, 255));
  else if (rtprice.prcchange.toFloat () < 0)
    text->setDefaultTextColor (Qt::red);
  else
    text->setDefaultTextColor (forecolor);

  text->setPlainText (str);
  if (core->visibleLastBar ())
  {
    if (core->events_enabled)
      core->draw ();
  }
  else
    drawObject (this);
}

// change foreground color
void
QTACObject::changeForeColor (QColor color)
{
  if (enabled == false)
    return;

  if (type != QTACHART_OBJ_SUBCHART)
    return;

  forecolor = color;
  title->setDefaultTextColor (forecolor);

  foreach (QTACObject *child, children)
  {
    if (child->type == QTACHART_OBJ_VBARS && type == QTACHART_VOLUME)
      child->forecolor = forecolor;

    if (child->type == QTACHART_OBJ_HLINE)
    {
      child->forecolor = forecolor;
      child->hvline->setPen (forecolor);
      child->title->setDefaultTextColor (forecolor);
    }
  }

  if (!editBtn.isNull ())
    editBtn->setStyleSheet (
      QString ("background: transparent; border: 1px solid transparent; \
                border-color: %1;color: %1; font: 9px;").arg (forecolor.name ()));
}

// clear chart's ITEMS
void
QTACObject::clearITEMS () NOEXCEPT
{
  for (qint32 counter = 0; counter < ITEMSsize; counter ++)
    ITEMS[counter]->setSize (0, 0, 0, 0);
}

// delete ITEMS
void
QTACObject::deleteITEMS ()
{
  for (qint32 counter = 0; counter < ITEMSsize; counter ++)
  {
    ITEMS[counter]->setSize (0, 0, 0, 0);
    delete ITEMS[counter];
  }

  ITEMSsize = 0;
}

// get parent
QTACObject *
QTACObject::getParentObject () NOEXCEPT
{
  return parentObject;
}

// get title
QString
QTACObject::getTitle () NOEXCEPT
{
  return titlestr;
}

// get trailer candle title
QString
QTACObject::getTrailerCandleText (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[0]->trailerCandleText;
    else
      return QStringLiteral ("");
  }
  return trailerCandleText;
}

QString
QTACObject::getTrailerCandleText2 (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[1]->trailerCandleText;
  }
  return QStringLiteral ("");
}

// get color
QColor
QTACObject::getColor (void) const
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
    return hvline->pen ().color ();

  return text->defaultTextColor ();
}

// get the price
qreal
QTACObject::getPrice (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[0]->price;
    else
      return 0.0;
  }
  return price;
}

qreal
QTACObject::getPrice2 (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[1]->price;
  }
  return 0.0;
}

// get the edge's pad
qreal
QTACObject::getPad (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[0]->pad;
  }
  return 0.0;
}

qreal
QTACObject::getPad2 (void)
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[1]->pad;
  }
  return 0.0;
}

// get text's direction of label
qint16
QTACObject::getTxtDirection ()
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[0]->txtdirection;
  }
  return 0.0;
}

qint16
QTACObject::getTxtDirection2 ()
{
  if (type == QTACHART_OBJ_LINE || type == QTACHART_OBJ_FIBO)
  {
    if (Edge.size () > 0)
      return Edge[1]->txtdirection;
  }
  return 0.0;
}

// get module's value set
DataSet
QTACObject::getModVSet (int *sz)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  PriceVector *result = nullptr;
  Array_t mvset = nullptr;
  qreal d = 0;
  int s = core->OPEN.size ();

  *sz = s;
  result = new PriceVector;

  if (dynamic)
    mvset = (Array_t) dynvset;
  else
  if (moduleName != QLatin1String (""))
    mvset = modvset ();

  if (mvset == nullptr)
    goto getModVSet_lbl10;

  for (int counter = 0; counter < s; counter ++)
  {
    d = *(double *) ArrayGet_imp (mvset, counter);
    result->append ((qreal) d);
  }

  return result;

getModVSet_lbl10:
  for (int counter = 0; counter < s; counter ++)
    result->append (d);

  return result;
}

// calculate and return value set
DataSet
QTACObject::valueSet ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  // calculate the dataset using tafunc
  if (TAfunc != nullptr)
  {
    if (Q_UNLIKELY (valueset == nullptr))
    {
      valueset = TAfunc (dataset, period);
      valuesetsize = dataset->size ();
    }
    else
    {
      if (Q_UNLIKELY (valuesetsize != dataset->size ()) ||
          lastperiod != period || core->reloaded)
      {
        delete valueset;
        valueset = nullptr;
        valueset = TAfunc (dataset, period);
        valuesetsize = dataset->size ();
        lastperiod = period;
      }
    }
  }
  else if (TAfunc2 != nullptr)
  {
    if (Q_UNLIKELY (valueset == nullptr))
    {
      valueset = TAfunc2 (core->HLOC, period);
      valuesetsize = core->HLOC->size ();
    }
    else
    {
      if (Q_UNLIKELY (valuesetsize != dataset->size ())
          || lastperiod != period || core->reloaded)
      {
        delete valueset;
        valueset = nullptr;
        valueset = TAfunc2 (core->HLOC, period);
        valuesetsize = core->HLOC->size ();
        lastperiod = period;
      }
    }
  }
  else
  {
    if (type == QTACHART_OBJ_CURVE || type == QTACHART_OBJ_CONTAINER ||
        type == QTACHART_OBJ_SUBCHART || type == QTACHART_OBJ_VBARS)
    {
      if (Q_UNLIKELY (valueset == nullptr))
      {
        valuesetsize = core->HLOC->size ();
        valueset = getModVSet (&valuesetsize);
      }
      else
      {
        if (dataset != nullptr)
        {
          if (Q_UNLIKELY (valuesetsize != dataset->size ())
              || lastperiod != period || core->reloaded || needsupdate)
          {
            delete valueset;
            valuesetsize = core->HLOC->size ();
            valueset = getModVSet (&valuesetsize);
            lastperiod = period;
            needsupdate = false;
          }
        }
      }
    }
  }

  if (children.size () > 0)
  {
    foreach (QTACObject *child, children)
    {
      child->setPeriod (period);
      child->valueSet ();
    }
  }

  return valueset;
}

// set the debug mode of a CGScript module
void
QTACObject::setCGScriptDebug (bool mode)
{
  cgscriptdebug = mode;
}

// set object's deleteit flag to true
void
QTACObject::setForDelete (void)
{
  if (deleteit == true)
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  deleteit = true;

  if (type == QTACHART_OBJ_LABEL || type == QTACHART_OBJ_TEXT)
  {
    if (text != nullptr)
    {
      text->setPlainText ("");
      text->setVisible (false);
    }
  }

  // set for delete all children
  foreach (QTACObject *obj1, Object)
  {
    foreach (QTACObject *obj2, core->Object)
      if (obj1 == obj2)
        obj1->setForDelete ();
  }

  // set for delete the parent
  if (dynamic)
  {
    foreach (QTACObject *obj1, core->Object)
    {
      if (obj1->modinit != nullptr)
      {
        if (obj1->deleteit != true && obj1->type == QTACHART_OBJ_CONTAINER)
        {
          bool remove = true;
          foreach (QTACObject *obj2, obj1->Object)
            if (obj2->deleteit == false)
              remove = false;

          if (remove == true)
            obj1->setForDelete ();
        }
      }
    }
  }
}

// Return DynParam value of the given name.  Returns 0.0 if name is not found.
qreal QTACObject::paramValue( const QString& name ) const
{
    if( name == periodParamName )
        return (qreal) getPeriod();
    if( name == colorParamName )
        return (qreal) getColor().rgb();

    const QTACObject *child;
    foreach (child, children)
    {
        if( name == child->periodParamName )
            return (qreal) child->getPeriod();
        if( name == child->colorParamName )
            return (qreal) child->getColor().rgb();
    }

    return 0.0;
}

#include "ObjectParameters.cpp"

// modify technical indicator;
bool
QTACObject::modifyIndicator ()
{
  if (enabled == false)
    return false;

  int xperiod = period;

  if( paramDialog.isNull() )
  {
      // Create temporary dialog on the fly.
      ParamVector pvector;
      _objectParameters(this, pvector);
      DynParamsDialog* pd = new DynParamsDialog(pvector, objectName());
      if( pd )
      {
          pd->setColorDialog (new appColorDialog);
          if( pd->exec() == QDialog::Accepted )
          {
             modifyFromDialog( pd );
          }
          delete pd;
      }
  }
  else
  {
      paramDialog->setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
      paramDialog->exec ();
      QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
  }

  if (deleteit == true)
  {
    return false;
  }

  if (xperiod != period)
    valueSet ();

  return true;
}

// set the price level for a horizontal line
void
QTACObject::setHLine (QGraphicsLineItem *line, qreal value)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QRectF rectf;
  qreal y;

  if (deleteit)
    return;

  if (parentObject == nullptr)
    title->setToolTip (QStringLiteral ( "<span style=\"font: 9px;color: black;\">Double click to edit</span>"));

  if (line == nullptr)
  {
    line = new QGraphicsLineItem ();
    line->setVisible (true);
    line->setLine (0, 0, 0, 0);
    line->setPen (QPen (core->gridcolor));
    scene->qtcAddItem (line);
  }

  price = value;
  hvline = line;
  if (parentObject == nullptr)
  {
    title->setParent (this);

    if (filter == nullptr)
    {
      filter = new QTACObjectEventFilter (title);
      title->installEventFilter (filter);
      title->setAcceptHoverEvents (true);
    }

    titlestr = QString ("%1").arg (price, 10, 'f', core->fracdig + 1);
    title->setPlainText (titlestr);
    title->setDefaultTextColor (hvline->pen().color ());
    titlestr = QStringLiteral ("Horizontal line ") % QString ("%1").arg (price, 10, 'f', core->fracdig + 1);

    y = core->yOnPrice (price);
    rectf = title->boundingRect();
    hvline->setLine (core->chartleftmost, y, core->chartrightmost - rectf.width (), y);
    title->setPos (core->chartrightmost - rectf.width (), y - 10);
    return;
  }

  titlestr = QString ("%1").arg (value, 10, 'f', core->fracdig + 1);
  title->setPlainText (titlestr);
  title->setDefaultTextColor (core->forecolor);
  titlestr = QStringLiteral ("Horizontal line ") % QString ("%1").arg (price, 10, 'f', core->fracdig + 1);
}

// set the price level for a horizontal line
void
QTACObject::setVLine (QGraphicsLineItem *line)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QRectF rectf;
  qreal x;

  if (deleteit)
    return;

  hvline = line;
  title->setParent (this);

  if (parentObject == nullptr)
    title->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));

  if (filter == nullptr)
  {
    filter = new QTACObjectEventFilter (title);
    title->installEventFilter (filter);
    title->setAcceptHoverEvents (true);
  }

  x = line->boundingRect().x ();
  trailerCandleText = core->getBottomText ((int) x);
  titlestr = trailerCandleText.mid (3, 20);
  title->setPlainText (titlestr);
  title->setDefaultTextColor (hvline->pen().color ());
  titlestr = QStringLiteral ("Vertical line ") % titlestr;

  rectf = title->boundingRect();
  hvline->setLine (x, core->charttopmost, x, core->chartbottomost - rectf.height () + 3);
  title->setPos (x - (rectf.width () / 2), core->chartbottomost - 12);
}

void
QTACObject::setVLine (QGraphicsLineItem *line, QString text)
{
  if (deleteit)
    return;

  hvline = line;
  title->setParent (this);

  if (parentObject == nullptr)
    title->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));

  if (filter == nullptr)
  {
    filter = new QTACObjectEventFilter (title);
    title->installEventFilter (filter);
    title->setAcceptHoverEvents (true);
  }

  trailerCandleText = text;
  titlestr = trailerCandleText.mid (3, 20);
  title->setPlainText (titlestr);
  title->setDefaultTextColor (hvline->pen().color ());
  titlestr = QStringLiteral ("Vertical line ") % titlestr;
}

// set the edges of a trend line
void
QTACObject::setTLine (QGraphicsLineItem *sline)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QLineF line;
  QRectF rectf;
  QString prcstr;
  LineEdge *edge;

  if (Q_UNLIKELY (deleteit))
    return;

  title->setParent (this);
  hvline = sline;
  line = hvline->line ();

  // edge 1
  edge = Edge[0];
  edge->price = core->priceOnY ((int) line.y1 ());
  edge->trailerCandleText = core->getBottomText ((int) line.x1 ());
  if (line.x1 () < line.x2 ())
    edge->txtdirection = 0;
  else
    edge->txtdirection = 1;
  edge->sequence = 1;
  prcstr = QString::number (edge->price, 'f', core->fracdig + 1);
  edge->pricetxt->setFont (title->font ());
  edge->pricetxt->setPlainText (prcstr);
  edge->pricetxt->setDefaultTextColor (hvline->pen().color ());
  titlestr = QStringLiteral ("Trend line ") % prcstr % QStringLiteral (" - ");

  // edge 2
  edge = Edge[1];
  edge->price = core->priceOnY ((int) line.y2 ());
  edge->trailerCandleText = core->getBottomText ((int) line.x2 ());
  if (line.x1 () < line.x2 ())
    edge->txtdirection = 1;
  else
    edge->txtdirection = 0;

  edge->sequence = 2;
  prcstr = QString::number (edge->price, 'f', core->fracdig + 1);
  edge->pricetxt->setFont (title->font ());
  edge->pricetxt->setPlainText (prcstr);
  edge->pricetxt->setDefaultTextColor (hvline->pen().color ());
  titlestr += prcstr;

  hvline->setLine (line.x1 (), line.y1 (), line.x2 (), line.y2 ());
  foreach (edge, Edge)
  {
    qreal x = 0, y = 0;
    rectf = edge->pricetxt->boundingRect();

    if (edge->sequence == 1)
    {
      X1 = x = line.x1 ();
      Y1 = y = line.y1 ();
    }
    else if (edge->sequence == 2)
    {
      X2 = x = line.x2 ();
      Y2 = y = line.y2 ();
    }

    if (edge->trailerCandleText.size () < 4)
    {
      edge->pad = core->HLOC->size () - *core->startbar;
      edge->pad = (core->framewidth * 1.5 * edge->pad);
      if (edge->txtdirection == 1)
      {
        edge->pad = core->chartrightmost - *core->excess_drag_width;
      }
      else
      {
        edge->pad =  edge->pad + *core->excess_drag_width;
        edge->pad =  core->chartrightmost - edge->pad;
      }
      edge->pad -= x;
      edge->pad = qAbs (edge->pad) / (core->framewidth * 1.5);
    }

    if (edge->txtdirection == 0)
      x -=  (rectf.width () + 2);
    else
      x += 2;

    if (!scene->items ().contains (edge->pricetxt))
      scene->qtcAddItem (edge->pricetxt);
    if (x > core->chartleftmost &&
        x < (core->chartrightmost - rectf.width ()) &&
        y > core->charttopmost &&
        y < (core->chartbottomost - rectf.height ()))
    {
      edge->pricetxt->setPos ( x, y - 12);
      edge->pricetxt->setVisible (true);
    }
    else
      edge->pricetxt->setVisible (false);
  }
}

void
QTACObject::setTLine (QGraphicsLineItem *sline, LineEdge e1, LineEdge e2)
{
  if (Q_UNLIKELY (deleteit))
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  title->setParent (this);
  hvline = sline;

  Edge[0]->price = e1.price;
  Edge[0]->pad = e1.pad;
  Edge[0]->txtdirection = e1.txtdirection;
  Edge[0]->trailerCandleText = e1.trailerCandleText;
  Edge[0]->sequence = e1.sequence;
  Edge[0]->pricetxt->setPlainText (QString::number (e1.price, 'f', core->fracdig + 1));
  Edge[0]->pricetxt->setDefaultTextColor (hvline->pen().color ());
  scene->qtcAddItem (Edge[0]->pricetxt);

  Edge[1]->price = e2.price;
  Edge[1]->pad = e2.pad;
  Edge[1]->txtdirection = e2.txtdirection;
  Edge[1]->trailerCandleText = e2.trailerCandleText;
  Edge[1]->sequence = e2.sequence;
  Edge[1]->pricetxt->setPlainText (QString::number (e2.price, 'f', core->fracdig + 1));
  Edge[1]->pricetxt->setDefaultTextColor (hvline->pen().color ());
  scene->qtcAddItem (Edge[1]->pricetxt);

  titlestr = QStringLiteral ("Trend line ") % QString::number (e1.price, 'f', core->fracdig + 1) % QStringLiteral (" - ");
  titlestr += QString::number (e2.price, 'f', core->fracdig + 1);
}

// set the edges of a fibo line
void
QTACObject::setFibo (QGraphicsLineItem *sline)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QLineF line;
  QRectF rectf;
  QString prcstr;
  LineEdge *edge;

  if (Q_UNLIKELY (deleteit))
    return;

  title->setParent (this);
  hvline = sline;
  line = hvline->line ();

  // edge 1
  edge = Edge[0];
  edge->price = core->priceOnY ((int) line.y1 ());
  edge->trailerCandleText = core->getBottomText ((int) line.x1 ());
  edge->txtdirection = 0;
  edge->sequence = 1;
  prcstr = line.y1 () < line.y2 () ? QStringLiteral ("1.0") : QStringLiteral ("0.0");
  edge->pricetxt->setFont (title->font ());
  edge->pricetxt->setPlainText (prcstr);
  edge->pricetxt->setDefaultTextColor (hvline->pen().color ());

  // edge 2
  edge = Edge[1];
  edge->price = core->priceOnY ((int) line.y2 ());
  edge->trailerCandleText = core->getBottomText ((int) line.x2 ());
  edge->txtdirection = 0;
  edge->sequence = 2;
  prcstr = line.y1 () < line.y2 () ? QStringLiteral ("0.0") : QStringLiteral ("1.0");
  edge->pricetxt->setFont (title->font ());
  edge->pricetxt->setPlainText (prcstr);
  edge->pricetxt->setDefaultTextColor (hvline->pen().color ());

  hvline->setLine (line.x1 (), line.y1 (), line.x2 (), line.y2 ());
  foreach (edge, Edge)
  {
    qreal x = 0, y = 0;
    rectf = edge->pricetxt->boundingRect();

    if (edge->sequence == 1)
    {
      X1 = x = line.x1 ();
      Y1 = y = line.y1 ();
    }
    else if (edge->sequence == 2)
    {
      X2 = x = line.x2 ();
      Y2 = y = line.y2 ();
    }

    if (edge->trailerCandleText.size () < 4)
    {
      edge->pad = core->HLOC->size () - *core->startbar;
      edge->pad = (core->framewidth * 1.5 * edge->pad);
      edge->pad = core->chartrightmost - *core->excess_drag_width;
      edge->pad -= x;
      edge->pad = qAbs (edge->pad) / (core->framewidth * 1.5);
    }

    if (edge->txtdirection == 0)
      x -=  (rectf.width () + 2);
    else
      x += 2;

    if (!scene->items ().contains (edge->pricetxt))
      scene->qtcAddItem (edge->pricetxt);

    edge->pricetxt->setPos ( x, y - 12);
    edge->pricetxt->setVisible (true);
  }

  FiboLevelPrice.clear ();
  for (qint32 counter = 0, maxcounter = FiboLevelPrc.size ();
       counter < maxcounter; counter ++)
  {
    qreal price, x;
    price = qAbs (Edge[0]->price - Edge[1]->price);
    price *= FiboLevelPrc[counter];
    if (Edge[0]->price < Edge[1]->price)
      price += Edge[0]->price;
    else
      price += Edge[1]->price;
    FiboLevelPrice += price;

    prcstr = QString::number (price, 'f', core->fracdig + 2);
    FiboLevelPrcLbl[counter].setFont (title->font ());
    FiboLevelPrcLbl[counter].setPlainText (prcstr);
    FiboLevelPrcLbl[counter].setDefaultTextColor (hvline->pen().color ());
    rectf = FiboLevelPrcLbl[counter].boundingRect();
    x =  core->chartrightmost - (rectf.width () + 2);
    ((QGraphicsLineItem *)FiboLevel[counter])->setPen (hvline->pen ());
    ((QGraphicsLineItem *)FiboLevel[counter])->setLine (line.x1 (),
        core->yOnPrice (price), x,
        core->yOnPrice (price));
    FiboLevelPrcLbl[counter].setPos (x + 1, core->yOnPrice (price) - 12);
    FiboLevelLbl[counter].setVisible (true);

    if (counter > 0 && counter < FiboLevelPrc.size () - 1)
    {
      prcstr = QString::number (FiboLevelPrc[counter], 'f', core->fracdig + 2);
      FiboLevelLbl[counter].setFont (title->font ());
      FiboLevelLbl[counter].setPlainText (prcstr);
      FiboLevelLbl[counter].setDefaultTextColor (hvline->pen().color ());
      rectf = FiboLevelLbl[counter].boundingRect();
      x =  line.x1 () - (rectf.width () + 2);
      FiboLevelLbl[counter].setPos (x, core->yOnPrice (price) - 12);
      FiboLevelLbl[counter].setVisible (true);
    }
  }
  titlestr = QStringLiteral ("Fibonacci");
}

void
QTACObject::setFibo (QGraphicsLineItem *sline, LineEdge e1, LineEdge e2)
{
  if (Q_UNLIKELY (deleteit))
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QString prcstr;

  title->setParent (this);
  hvline = sline;

  Edge[0]->price = e1.price;
  Edge[0]->pad = e1.pad;
  Edge[0]->txtdirection = e1.txtdirection;
  Edge[0]->trailerCandleText = e1.trailerCandleText;
  Edge[0]->sequence = e1.sequence;
  Edge[0]->pricetxt->setPlainText (QString (hvline->line ().y1 () < hvline->line ().y2 () ? QStringLiteral ("1.0") : QStringLiteral ("0.0")));
  Edge[0]->pricetxt->setDefaultTextColor (hvline->pen().color ());
  scene->qtcAddItem (Edge[0]->pricetxt);

  Edge[1]->price = e2.price;
  Edge[1]->pad = e2.pad;
  Edge[1]->txtdirection = e2.txtdirection;
  Edge[1]->trailerCandleText = e2.trailerCandleText;
  Edge[1]->sequence = e2.sequence;
  Edge[1]->pricetxt->setPlainText (QString(hvline->line ().y1 () < hvline->line ().y2 () ? QStringLiteral ("0.0") : QStringLiteral ("1.0")));
  Edge[1]->pricetxt->setDefaultTextColor (hvline->pen().color ());
  scene->qtcAddItem (Edge[1]->pricetxt);

  FiboLevelPrice.clear ();
  for (qint32 counter = 0, maxcounter = FiboLevelPrc.size ();
       counter < maxcounter; counter ++)
  {
    qreal price;
    price = qAbs (Edge[0]->price - Edge[1]->price);
    price *= FiboLevelPrc[counter];
    if (Edge[0]->price < Edge[1]->price)
      price += Edge[0]->price;
    else
      price += Edge[1]->price;
    FiboLevelPrice += price;

    prcstr = QString::number (price, 'f', core->fracdig + 2);
    FiboLevelPrcLbl[counter].setFont (title->font ());
    FiboLevelPrcLbl[counter].setPlainText (prcstr);
    FiboLevelPrcLbl[counter].setDefaultTextColor (hvline->pen().color ());
    (static_cast <QGraphicsLineItem *> (FiboLevel[counter]))->setPen (hvline->pen ());

    if (counter > 0 && counter < FiboLevelPrc.size () - 1)
    {
      prcstr = QString::number (FiboLevelPrc[counter], 'f', core->fracdig + 2);
      FiboLevelLbl[counter].setFont (title->font ());
      FiboLevelLbl[counter].setPlainText (prcstr);
      FiboLevelLbl[counter].setDefaultTextColor (hvline->pen().color ());
    }
  }

  titlestr = QStringLiteral ("Fibonacci");
}

// set text of label or text objects
void
QTACObject::setText (QGraphicsTextItem *textitem, qreal x, qreal y)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  qreal height, width, kx = x;

  if (core->object_drag == true && type == QTACHART_OBJ_TEXT)
  {
    if (moduleName != "" || dynamic == true)
    {
      QRectF rect = textitem->boundingRect ();

      if (hadjust == QTACHART_OBJ_HADJUST_CENTER)
        x += rect.width () / 2;
      else if (hadjust == QTACHART_OBJ_HADJUST_LEFT)
        x += (rect.width () + 2);
      else if (hadjust == QTACHART_OBJ_HADJUST_RIGHT)
        x -= 2;

      if (vadjust == QTACHART_OBJ_VADJUST_CENTER)
        y += rect.height () / 2;
      else if (vadjust == QTACHART_OBJ_VADJUST_ABOVE)
        y += (rect.height () + 2);
      else if (vadjust == QTACHART_OBJ_VADJUST_BELOW)
        y -= 2;

      kx = x;
    }
  }

  X1 = x;
  Y1 = y;

  text = textitem;
  forecolor = text->defaultTextColor ();
  text->setParent (this);
  text->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));

  if (filter == nullptr)
  {
    filter = new QTACObjectEventFilter (text);
    text->installEventFilter (filter);
    text->setAcceptHoverEvents (true);
  }

  height = qAbs (core->chartbottomost - core->charttopmost);
  width =  qAbs (core->chartrightmost - core->chartleftmost);
  price = core->priceOnY ((int) y);
  x -= core->chartleftmost;
  y -= core->charttopmost;
  relx = x / width;
  rely = y / height;

  if (type == QTACHART_OBJ_LABEL)
    drawLabel ();
  else if (type == QTACHART_OBJ_TEXT)
  {
    trailerCandleText = core->getBottomText (kx);
    drawText ();
  }
}

void
QTACObject::setText(QGraphicsTextItem *textitem, QString candleText, qreal prc)
{
  if (type != QTACHART_OBJ_TEXT)
    return;

  price = prc;
  trailerCandleText = candleText;

  if (textitem != nullptr)
  {
    text = textitem;
    text->setParent (this);
    text->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));
    text->setVisible (false);

    if (filter == nullptr)
    {
      filter = new QTACObjectEventFilter (text);
      text->installEventFilter (filter);
      text->setAcceptHoverEvents (true);
    }

    forecolor = text->defaultTextColor ();
  }
  drawText ();
}

// set indicator's parameter dialog
void
QTACObject::setParamDialog (ParamVector pvector, QString title, QObject *parent)
{
  if (!paramDialog.isNull ())
    delete paramDialog;

  paramDialog = new DynParamsDialog (pvector, title);
  appColorDialog *colorDialog = new appColorDialog;
  paramDialog->setColorDialog (colorDialog);
  paramDialog->setVisible (false);

  connect(paramDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(modification_accepted()));
  connect(paramDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(modification_rejected()));

  if (!editBtn.isNull ())
    editBtn->setVisible (true);

  if (parent != nullptr)
    paramDialog->setParent (qobject_cast <QWidget *> (parent)) ;
}

// set objects title
void
QTACObject::setTitle (QString str)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  titlestr = str;
  setObjectName (titlestr);

  if (type == QTACHART_OBJ_CURVE)
    title->setPlainText (titlestr % QStringLiteral ("(") % (period > 0?QString::number (period, 'f', 0):QStringLiteral ("")) % QStringLiteral (")") );
  else
    title->setPlainText (titlestr);

  title->setDefaultTextColor (core->forecolor);
}

// set symbol for text or label object
void
QTACObject::setSymbol (const char *symbol)
{
  if (type == QTACHART_OBJ_TEXT || type == QTACHART_OBJ_LABEL)
  {
    QString s(symbol);
    text->setHtml (s);
  }

  /*
    hadjust = QTACHART_OBJ_HADJUST_CENTER;

    if (s == QTACHART_OBJ_UPWARDS_ARROW ||
        s == QTACHART_OBJ_UPWARDS_WHITE_ARROW ||
        s == QTACHART_OBJ_WHITE_UP_POINTING_INDEX)
      vadjust = QTACHART_OBJ_VADJUST_BELOW;


    if (s == QTACHART_OBJ_DOWNWARDS_ARROW ||
        s == QTACHART_OBJ_DOWNWARDS_WHITE_ARROW ||
        s == QTACHART_OBJ_WHITE_DOWN_POINTING_INDEX)
      vadjust = QTACHART_OBJ_VADJUST_ABOVE;
  */
}

// set objects attributes
void
QTACObject::setAttributes_common (QTAChartDataSet dstype,
                                  int per, QString perParamName,
                                  qreal xmin, qreal xmax,
                                  QColor color, QString colParamName)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  dataset_type = dstype;
  forecolor = color;
  rmin = xmin;
  rmax = xmax;
  period = per;
  periodParamName = perParamName;
  colorParamName = colParamName;

  // set dataset to HLOC
  if (dataset_type == QTACHART_OPEN)
    dataset = &core->OPEN;

  if (dataset_type == QTACHART_CLOSE)
    dataset = &core->CLOSE;

  if (dataset_type == QTACHART_HIGH)
    dataset = &core->HIGH;

  if (dataset_type == QTACHART_LOW)
    dataset = &core->LOW;

  if (dataset_type == QTACHART_VOLUME)
    dataset = &core->VOLUME;

  if (type == QTACHART_OBJ_CURVE ||
      type == QTACHART_OBJ_DOT ||
      type == QTACHART_OBJ_VBARS)
  {
    visibleitems = 0;
    ITEMSsize = 0;
    ITEMS = new QTCGraphicsItem * [MAXITEMS];

    if (parentObject != nullptr)
      scene->removeItem (title);
  }

  if (parentObject == nullptr)
    title->setToolTip (QStringLiteral ("<span style=\"font: 9px;color: black;\">Double click to edit</span>"));

  valueSet ();
}

void
QTACObject::setAttributes (QTAChartDataSet dstype,
                           int per, QString perParamName,
                           DataSet (*tafunc) (const DataSet, int),
                           qreal xmin,
                           qreal xmax,
                           QColor color, QString colParamName)
{
  TAfunc = tafunc;
  setAttributes_common (dstype, per, perParamName, xmin, xmax, color, colParamName);
}

void
QTACObject::setAttributes (QTAChartDataSet dstype,
                           int per, QString perParamName,
                           DataSet (*tafunc) (const FrameVector *, int),
                           qreal xmin,
                           qreal xmax,
                           QColor color, QString colParamName)
{
  TAfunc2 = tafunc;
  setAttributes_common (dstype, per, perParamName, xmin, xmax, color, colParamName);
}

// set subchart's title to display indicator's data
void
QTACObject::setDataTitle (int x)
{
  if (!enabled)
    return;

  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  datastr = QStringLiteral ("");
  if (type == QTACHART_OBJ_SUBCHART)
  {
    const QTACObject *child;
    int bar;

    foreach (child, children)
    {
      if ((child->type == QTACHART_OBJ_CURVE || child->type == QTACHART_OBJ_VBARS) &&
          child->valueset != nullptr)
      {
        qreal data = 0;
        bar = core->barOnX (x);

        if (bar >= 0 && bar < child->valueset->size ())
        {
          data = child->valueset->at(bar);
          if (data > 1000 && data < 1000000)
          {
            data /= 1000;
            datastr += QStringLiteral (" ") % QString::number (data, 'f', 2) % QStringLiteral ("K");
          }
          else if (data >= 1000000)
          {
            data /= 1000000;
            datastr += QStringLiteral (" ") % QString::number (data, 'f', 2) % QStringLiteral ("M");
          }
          else
            datastr += QStringLiteral (" ") % QString::number (data, 'f', 2);
        }
      }
    }

    if (moduleName == QLatin1String (""))
    {
      title->setPlainText (titlestr % QStringLiteral ("(") %
                           (period > 0 ? QString::number (period, 'f', 0) : QStringLiteral ("")) % \
                           QStringLiteral ("):") % datastr);
    }
    else
    {
      ParamVector pv = getParamDialog ()->getPVector ();
      QString args = QStringLiteral ("(");
      int counter, s;
      bool first;

      for (counter = 0, s = pv.size (), first = true; counter < s; counter ++)
      {
        DynParam *p = pv.at(counter);
        if (p->show)
        {
          if (!first)
            args += QStringLiteral (",");
          else
            first = false;

          args += QString::number (p->defvalue, 'f', 0);
        }
      }
      args += QStringLiteral ("):");
      title->setPlainText (titlestr % args % datastr);
    }
  }
}

// find dataset's min and max on chart
void
QTACObject::minmax ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  qreal ritem;

  if (valueset == nullptr)
    return;

  if (parentObject != nullptr && type == QTACHART_OBJ_HLINE)
  {
    rangemin = rangemax = price;
    return;
  }

  rangemin = rmin;
  if (rmin == QREAL_MIN)
    rangemin = QREAL_MAX;

  rangemax = rmax;
  if (rmax == QREAL_MAX)
    rangemax = QREAL_MIN;

  for (qint32 k = *core->startbar,
       maxk = *core->startbar + core->nbars_on_chart,
       vsetsize = valueset->size ();
       k < maxk; k++)
  {
    if (k < vsetsize)
    {
      ritem = valueset->at(k);

      // find dataset's minimum
      if (rmin == QREAL_MIN)
        if (rangemin > ritem)
          rangemin = ritem;

      // find dataset's maximum
      if (rmax == QREAL_MAX)
        if (rangemax < ritem)
          rangemax = ritem;
    }
  }

  if (type == QTACHART_OBJ_VBARS && parentObject != nullptr)
  {
    if (rangemin > 0 && rangemax > 0)
      rangemin = 0;
    if (rangemin < 0 && rangemax < 0)
      rangemax = 0;
  }

  rangemax = ((rangemax >= 0)?rangemax * 1.05:(qAbs (rangemax) * 1.05) * -1);
  rangemin = ((rangemin >= 0)?rangemin * 1.05:(qAbs (rangemin) * 1.05) * -1);
}

void QTACObject::modifyFromDialog( DynParamsDialog* pd )
{
  QTACObject *child;

  pd->setVisible (false);
  if (pd->removeCheckBox->checkState () == Qt::Checked)
  {
    clearITEMS ();
    title->setVisible (false);
    setForDelete ();
    if (type == QTACHART_OBJ_CURVE || children.size () > 0)
    {
      foreach (child, children)
        child->clearITEMS ();
    }
    return;
  }

  if (! colorParamName.isEmpty())
    forecolor = pd->getParam (colorParamName);

  if (! periodParamName.isEmpty())
    period = (int) pd->getParam (periodParamName);

  if (children.size () > 0)
  {
    foreach (child, children)
    {
      if (child->type == QTACHART_OBJ_HLINE)
        child->setHLine (child->hvline, pd->getParam (child->periodParamName));
      else if (! child->colorParamName.isEmpty())
        child->forecolor = pd->getParam (child->colorParamName);
      else if (! child->periodParamName.isEmpty())
        child->period = (int) pd->getParam (child->periodParamName);
    }
  }

  if (type == QTACHART_OBJ_CURVE)
    title->setPlainText (titlestr % QStringLiteral ("(") % (period > 0?QString::number (period, 'f', 0):QStringLiteral ("")) % QStringLiteral (")") );

  if (moduleEvent (EV_INPUT_VAR) && !deleteit)
    valueSet ();

  drawObject (this);
}

// slots
void
QTACObject::modification_accepted()
{
  modifyFromDialog( paramDialog );
}

void
QTACObject::modification_rejected()
{
  paramDialog->setVisible (false);
}
