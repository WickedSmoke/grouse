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

// constructor, destructor and some methods
// of QTAChartCore

#include <cstring>
#include <QtGlobal>
#include <QtCore/qmath.h>
#include <QApplication>
#include "qtachart_core.h"
#include "defs.h"
#include "top.h"

#ifdef Q_CC_MSVC
#define atoll(x) _atoi64(x)
#endif // Q_CC_MSVC

// constructor
QTAChartCore::QTAChartCore (QWidget * parent)
{
  // initialize
  DrawMutex = new QMutex (QMutex::NonRecursive);
  classError = CG_ERR_OK;
  chart = &View;
  scene = &Scene;
  paramdlgopened = 0;
  visibleitems = 0;
  visiblegriditems = 0;
  ITEMSsize = 0;
  GRIDsize = 0;
  eventcounter = 0;
  currentbar = -1;
  last_x = 0;
  gridstep = 0;
  priceUpdater = NULL;
  tfBtn = NULL;

  ITEMS = NULL;
  GRID = NULL;
  title = NULL;
  subtitle = NULL;
  scaletitle = NULL;
  typetitle = NULL;
  bottom_text = NULL;
  ruller_cursor = NULL;
  expandBtn = NULL;
  propertiesBtn = NULL;
  functionBtn = NULL;
  objectsBtn = NULL;
  drawBtn = NULL;
  helpBtn = NULL;
  dataBtn = NULL;
  zoomInBtn = NULL;
  zoomOutBtn = NULL;
  rightedge = NULL;
  leftedge = NULL;
  topedge = NULL;
  bottomedge = NULL;
  propScr = NULL;
  drawScr = NULL;
  functionScr = NULL;
  objectsScr = NULL;
  helpScr = NULL;
  dataScr = NULL;
  textobjectdialog = NULL;
  lineobjectdialog = NULL;
  chartEventFilter = NULL;
  sceneEventFilter = NULL;

  if (parent != NULL)
    setParent (parent);

  // allocate and set attributes
  ITEMS = new (std::nothrow) QTCGraphicsItem * [MAXITEMS];
  if (!ITEMS) goto constructor_failed;

  GRID = new (std::nothrow) QTCGraphicsItem * [MAXGRIDLINES];
  if (!GRID) goto constructor_failed;

  title = new (std::nothrow) QGraphicsTextItem ;
  if (!title) goto constructor_failed;

  subtitle = new (std::nothrow) QGraphicsTextItem ;
  if (!subtitle) goto constructor_failed;

  scaletitle = new (std::nothrow) QGraphicsTextItem ;
  if (!scaletitle) goto constructor_failed;

  typetitle = new (std::nothrow) QGraphicsTextItem ;
  if (!typetitle) goto constructor_failed;

  bottom_text = new (std::nothrow) QGraphicsTextItem;
  if (!bottom_text) goto constructor_failed;

  ruller_cursor = new (std::nothrow) QTACGraphicsTextItem;
  if (!ruller_cursor) goto constructor_failed;

  expandBtn = new (std::nothrow) QToolButton;
  if (!expandBtn) goto constructor_failed;

  propertiesBtn = new (std::nothrow) QToolButton;
  if (!propertiesBtn) goto constructor_failed;

  functionBtn = new (std::nothrow) QToolButton;
  if (!functionBtn) goto constructor_failed;

  objectsBtn = new (std::nothrow) QToolButton;
  if (!objectsBtn) goto constructor_failed;

  drawBtn = new (std::nothrow) QToolButton;
  if (!drawBtn) goto constructor_failed;

  helpBtn = new (std::nothrow) QToolButton;
  if (!helpBtn) goto constructor_failed;

  dataBtn = new (std::nothrow) QToolButton;
  if (!dataBtn) goto constructor_failed;

  zoomInBtn = new (std::nothrow) QToolButton;
  if (!zoomInBtn) goto constructor_failed;

  zoomOutBtn = new (std::nothrow) QToolButton;
  if (!zoomOutBtn) goto constructor_failed;

  rightedge = new (std::nothrow) QGraphicsLineItem;
  if (!rightedge) goto constructor_failed;

  leftedge = new (std::nothrow) QGraphicsLineItem;
  if (!leftedge) goto constructor_failed;

  topedge = new (std::nothrow) QGraphicsLineItem;
  if (!topedge) goto constructor_failed;

  bottomedge = new (std::nothrow) QGraphicsLineItem;
  if (!bottomedge) goto constructor_failed;

  propScr = new (std::nothrow) QTACProperties;
  if (!propScr) goto constructor_failed;

  drawScr = new (std::nothrow) QTACDraw;
  if (!drawScr) goto constructor_failed;

  functionScr = new (std::nothrow) QTACFunctions;
  if (!functionScr) goto constructor_failed;

  objectsScr = new (std::nothrow) QTACObjects;
  if (!objectsScr) goto constructor_failed;

  helpScr = new (std::nothrow) QTACHelp;
  if (!helpScr) goto constructor_failed;

  dataScr = new (std::nothrow) QTACData;
  if (!dataScr) goto constructor_failed;

  textobjectdialog = new (std::nothrow) TextObjectDialog;
  if (!textobjectdialog) goto constructor_failed;

  lineobjectdialog = new (std::nothrow) LineObjectDialog;
  if (!lineobjectdialog) goto constructor_failed;

  chartEventFilter = new (std::nothrow) QTAChartEventFilter (this);
  if (!chartEventFilter) goto constructor_failed;

  sceneEventFilter = new (std::nothrow) QTAChartSceneEventFilter (this);
  if (!sceneEventFilter) goto constructor_failed;

  lineobjectdialog->setModal (true);
  textobjectdialog->setModal (true);
  expandBtn->setAutoRaise (true);
  propertiesBtn->setAutoRaise (true);
  functionBtn->setAutoRaise (true);
  objectsBtn->setAutoRaise (true);
  drawBtn->setAutoRaise (true);
  helpBtn->setAutoRaise (true);
  dataBtn->setAutoRaise (true);
  zoomInBtn->setAutoRaise (true);
  zoomOutBtn->setAutoRaise (true);

  monthlit << QStringLiteral ("JAN") << QStringLiteral ("FEB") <<
              QStringLiteral ("MAR") << QStringLiteral ("APR") <<
              QStringLiteral ("MAY") << QStringLiteral ("JUN") <<
              QStringLiteral ("JUL") << QStringLiteral ("AUG") <<
              QStringLiteral ("SEP") << QStringLiteral ("OCT") <<
              QStringLiteral ("NOV") << QStringLiteral ("DEC");

  setSizeChanged ();
  return;

constructor_failed:
  if (lineobjectdialog) delete lineobjectdialog;
  if (textobjectdialog) delete textobjectdialog;
  if (rightedge) delete rightedge;
  if (leftedge) delete leftedge;
  if (topedge) delete topedge;
  if (bottomedge) delete bottomedge;
  if (zoomOutBtn) delete zoomOutBtn;
  if (zoomInBtn) delete zoomInBtn;
  if (helpBtn) delete helpBtn;
  if (dataBtn) delete dataBtn;
  if (propertiesBtn) delete propertiesBtn;
  if (drawBtn) delete drawBtn;
  if (functionBtn) delete functionBtn;
  if (expandBtn) delete expandBtn;
  if (ruller_cursor) delete ruller_cursor;
  if (bottom_text) delete bottom_text;
  if (scaletitle) delete scaletitle;
  if (typetitle) delete typetitle;
  if (title) delete title;
  if (subtitle) delete subtitle;
  if (propScr) delete propScr;
  if (drawScr) delete drawScr;
  if (helpScr) delete helpScr;
  if (dataScr) delete dataScr;
  if (functionScr) delete functionScr;
  if (objectsScr) delete objectsScr;
  if (chartEventFilter) delete chartEventFilter;
  if (sceneEventFilter) delete sceneEventFilter;
  if (ITEMS) delete[] ITEMS;
  if (GRID) delete[] GRID;
  classError = CG_ERR_NOMEM;
}

// destructor
QTAChartCore::~QTAChartCore (void)
{
  int nbuttons;

  if (classError == CG_ERR_NOMEM)
    return;

  DrawMutex->lock ();

  deleteOnlinePrice ();

  foreach (const QTACObject *object, Object)
    delete object;

  nbuttons = TIMEFRAME.size ();
  if (nbuttons != 0 && VOLUME.size () != 0 && gridstep > 1)
  {
    for (qint32 counter = 0; counter < nbuttons; counter ++)
      delete tfBtn[counter];

    delete[] prxtfBtn;
    delete[] tfBtn;
  }

  delete lineobjectdialog;
  delete textobjectdialog;
  delete rightedge;
  delete leftedge;
  delete topedge;
  delete bottomedge;
  delete zoomOutBtn;
  delete zoomInBtn;
  delete helpBtn;
  delete dataBtn;
  delete propertiesBtn;
  delete drawBtn;
  delete functionBtn;
  delete expandBtn;
  delete ruller_cursor;
  delete bottom_text;
  delete scaletitle;
  delete typetitle;
  delete title;
  delete subtitle;
  delete propScr;
  delete drawScr;
  delete helpScr;
  delete dataScr;
  delete functionScr;
  delete objectsScr;
  delete chartEventFilter;
  delete sceneEventFilter;

  deleteITEMS ();
  delete[] ITEMS;

  deleteGRID ();
  delete[] GRID;

  DrawMutex->unlock ();
  delete DrawMutex;
}

/// Aa
// add label
QTACObject *
QTAChartCore::addLabel (QGraphicsTextItem *item, qreal x, qreal y)
{
  QTACObject *label, *found = NULL;

  foreach (label, Object)
    if ((label->type == QTACHART_OBJ_LABEL ||
         label->type == QTACHART_OBJ_TEXT) &&
        label->text == item)
      found = label;

  if (found == NULL)
    label = new QTACObject (this, QTACHART_OBJ_LABEL);
  else
  {
    label = found;
    label->type = QTACHART_OBJ_LABEL;
  }

  label->setText (item, x, y);
  label->setTitle ("Label");

  return label;
}

// add subchart button
SubChartButton *
QTAChartCore::addSCB (QTACObject *obj, QString name)
{
  SubChartButton *btn;

  btn = new SubChartButton (obj);
  btn->setObjectName (name);
  connect (btn, SIGNAL (clicked ()), this,
           SLOT (SCBtn_clicked ()));

  CloseSCB += btn;
  return btn;
}

// add horizontal line
void
QTAChartCore::addHLine (QGraphicsLineItem *item, qreal price)
{
  QTACObject *hline, *found = NULL;

  foreach (hline, Object)
    if ( hline->type == QTACHART_OBJ_HLINE &&
         hline->hvline == item)
      found = hline;

  if (found == NULL)
    hline = new QTACObject (this, QTACHART_OBJ_HLINE);
  else
    hline = found;

  hline->setHLine (item, price);
}

// add vertical line
void
QTAChartCore::addVLine (QGraphicsLineItem *item)
{
  QTACObject *vline, *found = NULL;

  foreach (vline, Object)
    if ( vline->type == QTACHART_OBJ_VLINE &&
         vline->hvline == item)
      found = vline;

  if (found == NULL)
    vline = new QTACObject (this, QTACHART_OBJ_VLINE);
  else
    vline = found;

  vline->setVLine (item);
}

void
QTAChartCore::addVLine (QGraphicsLineItem *item, QString trailerCandleText)
{
  QTACObject *vline, *found = NULL;

  foreach (vline, Object)
    if ( vline->type == QTACHART_OBJ_VLINE &&
         vline->hvline == item)
      found = vline;

  if (found == NULL)
    vline = new QTACObject (this, QTACHART_OBJ_VLINE);
  else
    vline = found;

  vline->setVLine (item, trailerCandleText);
}

// add trend line
void
QTAChartCore::addTLine (QGraphicsLineItem *item)
{
  QTACObject *sline, *found = NULL;

  foreach (sline, Object)
    if ( sline->type == QTACHART_OBJ_LINE &&
         sline->hvline == item)
      found = sline;

  if (found == NULL)
    sline = new QTACObject (this, QTACHART_OBJ_LINE);
  else
    sline = found;

  sline->setTLine (item);
}

void
QTAChartCore::addTLine (QGraphicsLineItem *item, LineEdge e1, LineEdge e2)
{
  QTACObject *sline, *found = NULL;

  foreach (sline, Object)
    if ( sline->type == QTACHART_OBJ_LINE &&
         sline->hvline == item)
      found = sline;

  if (found == NULL)
    sline = new QTACObject (this, QTACHART_OBJ_LINE);
  else
    sline = found;

  sline->setTLine (item, e1, e2);
}

// add fibo
void
QTAChartCore::addFibo (QGraphicsLineItem *item)
{
  QTACObject *sline, *found = NULL;

  foreach (sline, Object)
    if ( sline->type == QTACHART_OBJ_FIBO &&
         sline->hvline == item)
      found = sline;

  if (found == NULL)
    sline = new QTACObject (this, QTACHART_OBJ_FIBO);
  else
    sline = found;

  sline->setFibo (item);
}

void
QTAChartCore::addFibo (QGraphicsLineItem *item, LineEdge e1, LineEdge e2)
{
  QTACObject *sline, *found = NULL;

  foreach (sline, Object)
    if ( sline->type == QTACHART_OBJ_FIBO &&
         sline->hvline == item)
      found = sline;

  if (found == NULL)
    sline = new QTACObject (this, QTACHART_OBJ_FIBO);
  else
    sline = found;

  sline->setFibo (item, e1, e2);
}

// add trailing text
void
QTAChartCore::addText(QGraphicsTextItem *item, qreal x, qreal y)
{
  QTACObject *text, *found = NULL;

  // trailing text needs to be attached on a candle
  if (getBottomText (x).trimmed () == QLatin1String (""))
  {
    addLabel (item, x, y);
    return;
  }

  foreach (text, Object)
    if ( text->type == QTACHART_OBJ_TEXT &&
         text->text == item)
      found = text;

  if (found == NULL)
  {
    text = new QTACObject (this, QTACHART_OBJ_TEXT);
    text->setTitle ("Text");
  }
  else
    text = found;

  text->setText (item, x, y);
}

void
QTAChartCore::addText(QGraphicsTextItem *item, QString candleText, qreal price)
{
  QTACObject *text;

  text = new QTACObject (this, QTACHART_OBJ_TEXT);
  text->setTitle ("Text");
  text->setText (item, candleText, price);
}

// add online price
void
QTAChartCore::addOnlinePrice ()
{
  QFont font;
  QGraphicsTextItem *textitem;

  if (Feed.toUpper ().contains ("CSV"))
    return;

  if (onlineprice != NULL)
    return;

  onlineprice = new QTACObject (this, QTACHART_OBJ_LABEL);
  textitem = new QGraphicsTextItem;
  if (events_enabled == false)
    textitem->setVisible (false);
  else
    textitem->setVisible (true);
  textitem->setPlainText ("Please wait...");
  textitem->setDefaultTextColor (forecolor);
  font.setFamily (DEFAULT_FONT_FAMILY );
  font.setWeight (QFont::Bold);
  font.setPixelSize (14);
  font.setPointSize (14);
  textitem->setFont (font);
  onlineprice->setTitle ("Label");
  onlineprice->setText (textitem, 100, 100);
  scene->addItem (textitem);
  priceUpdater = new PriceUpdater (Symbol, Feed, onlineprice);
}

/// Bb
// find the corresponding bar number of x
// returns -1 if not applicable
int
QTAChartCore::barOnX (int x) const
{
  qreal a, b, c;
  int bar;

  if (nbars_on_chart == 0)
    return -1;

  if (x < chartleftmost)
    return -1;

  if (x > chartrightmost)
    return -1;

  b = framewidth * 1.5;
  a = x - chartleftmost;
  c = chartwidth - a;
  c /= b;
  bar = qFloor (c);


  if (bar < 0)
    return -1;

  if (!((bar + (*startbar)) > HLOC->size () - 1))
    return  bar + (*startbar);

  return -1;
}

/// Cc

// change the foreground color
void
QTAChartCore::changeForeColor (QColor color)
{
  QPen pen;

  forecolor = color;
  textcolor = gridcolor = framecolor = forecolor;

  title->setDefaultTextColor (textcolor);
  subtitle->setDefaultTextColor (textcolor);
  scaletitle->setDefaultTextColor (textcolor);
  typetitle->setDefaultTextColor (textcolor);

  pen.setColor (gridcolor);
  topedge->setPen (pen);
  bottomedge->setPen (pen);
  leftedge->setPen (pen);
  rightedge->setPen (pen);

  helpBtn->setStyleSheet(
    QString("background: transparent;color: %1;font: 11px;\
            font-weight: bold;").arg(textcolor.name()));

  foreach (QTACObject *object, Object)
    object->changeForeColor (forecolor);

  bottom_text->setDefaultTextColor (forecolor);

  if (tfBtn != NULL)
    for (qint32 counter = 0; counter < TIMEFRAME.size (); counter ++)
      tfBtn[counter]->setStyleSheet
      (QString ("background: transparent; color: %1;font: 19px;\
          font-weight: bold;border: 1px solid transparent;\
          border-color: darkgray;").arg (forecolor.name ()));
}

// move chart backward by nbars
void
QTAChartCore::chartBackward (int nbars)
{
  int maxstartbar;
  qreal k;

  k = *excess_drag_width;
  if (k > 0)
  {
    k -= (framewidth * 1.5 * nbars);
    if (k < 0)
      *excess_drag_width = 0;
    else
      *excess_drag_width = k;
    draw ();
    return;
  }

  maxstartbar = HLOC->size () - nbars_on_chart;
  if (maxstartbar < 0)
    maxstartbar = 0;

  if ((*startbar) >= maxstartbar)
    return;

  (*startbar) += nbars;

  if ((*startbar) > maxstartbar)
    (*startbar) = maxstartbar;

  draw ();
}

// move chart forward by nbars
void
QTAChartCore::chartForward (int nbars)
{
  if ((*startbar) >= nbars)
  {
    (*startbar) -= nbars;
    draw ();
    return;
  }

  qreal k;
  k = *excess_drag_width;

  if (k < (chartwidth / 2))
  {
    k += (framewidth * 1.5 * nbars);
    *excess_drag_width = k;
    draw ();
  }
}

// move to chart's begin
void
QTAChartCore::chartBegin (void)
{
  *excess_drag_width = 0;
  (*startbar) = HLOC->size () - nbars_on_chart;
  (*startbar) ++;
  if ((*startbar) < 0)
    (*startbar) = 0;

  draw ();
}

// move to chart's end
void
QTAChartCore::chartEnd ()
{
  if (HLOC->size () < nbars_on_chart && *excess_drag_width == 0)
    return;

  *excess_drag_width = 0;
  (*startbar) = 0;

  draw ();
}

// next page
void
QTAChartCore::chartPageNext (void)
{
  if (*excess_drag_width > 0)
    return;

  if (*startbar > nbars_on_chart)
  {
    chartForward (nbars_on_chart - 1);
    return;
  }

  chartForward ((*startbar));
}

// previous page
void
QTAChartCore::chartPagePrevious (void)
{

  if (*excess_drag_width > 0)
  {
    *excess_drag_width = 0;
    draw ();
    return;
  }

  chartBackward (nbars_on_chart - 1);
}

// clear chart's GRID
void
QTAChartCore::clearGRID (void) NOEXCEPT
{
  for (qint32 counter = 0, maxcounter = GRIDsize;
       counter < maxcounter; counter ++)
  {
    GRID[counter]->setSize (0, 0, 0, 0);
    GRID[counter]->setVisible (false);
  }
}

// clear chart's ITEMS
void
QTAChartCore::clearITEMS (void) NOEXCEPT
{
  for (qint32 counter = visibleitems; counter < ITEMSsize; counter ++)
    ITEMS[counter]->setSize (0, 0, 0, 0);
}

// create time frame buttons
void
QTAChartCore::createTFButtons (void)
{
  int nbuttons, pad;

  nbuttons = TIMEFRAME.size ();
  if (nbuttons == 0)
    return;

  pad = 360;
  tfBtn = new QToolButton * [nbuttons];
  prxtfBtn = new QGraphicsProxyWidget * [nbuttons];

  for (qint32 counter = 0; counter < nbuttons; counter ++)
  {
    tfBtn[counter] = new QToolButton;
    tfBtn[counter]->setFixedSize (QSize (32, 28));
    tfBtn[counter]->setText (TIMEFRAME.at (counter).TFSymbol);
    tfBtn[counter]->setStyleSheet
    (QString ("background: transparent; color: %1;font: 19px;\
        font-weight: bold;border: 1px solid transparent;\
        border-color: %1;").arg (forecolor.name ()));
    // tfBtn[counter]->setToolTip (TOOLTIP + TIMEFRAME[counter].TFName  +"</span>");
    tfBtn[counter]->setFocusPolicy (Qt::NoFocus);
    prxtfBtn[counter] = scene->addWidget (tfBtn[counter], Qt::Widget);
    prxtfBtn[counter]->setGeometry (QRectF ((counter * 40) + pad, 5, 32, 28));
    connect (tfBtn[counter], SIGNAL (clicked ()), this,
             SLOT (tfBtn_clicked ()));
  }
}

/// Dd
// draw grid
void
QTAChartCore::drawGRID (void)
{
  QGraphicsTextItem *titem;
  QTCGraphicsItem *litem;
  QTAChartFrame pframe;
  QString stepstr, framestr;
  QPen pen;
  qint64 step, maxstep;
  qreal x, y;
  qint32 width, keepgridstep;
  int type = LineItemType, counter = 0;

  clearGRID ();
  pen.setColor (gridcolor);
#ifdef Q_OS_MAC
  pen.setStyle (Qt::DashLine);
#else
  pen.setStyle (Qt::DotLine);
#endif

  keepgridstep = gridstep;
  maxstep = priceOnY (title_height * 2) / points;

  step = chartbottom;
  if (chartbottom < gridstep || (chartbottom % gridstep) != 0)
  {
    stepstr = QString ("%1").arg (((qreal) step) * points, 10, 'f', fracdig);
    y = yOnPrice (step * points);
    if (counter >= GRIDsize)
    {
      litem = new QTCGraphicsItem (type);
      litem->setZValue (0.0);
      GRID[counter] = litem;
      titem = new QGraphicsTextItem (stepstr, litem);
      titem->setFont (gridFont);
      titem->setDefaultTextColor (textcolor);
      if (counter == 0)
      {
        titem = new QGraphicsTextItem (litem);
        titem->setFont (gridFont);
        titem->setDefaultTextColor (textcolor);
      }
      litem->init (scene);
    }

    litem = GRID[counter];
    litem->setPen (pen);
    litem->setLine (QLineF (chartleftmost, y, chartrightmost, y));
    titem = ((QGraphicsTextItem *)(litem->children ().at(0)));
    titem->setDefaultTextColor (textcolor);
    titem->setPlainText (stepstr);
    titem->setPos (ruller_cursor_x, y - 10);
    litem->setVisible (true);
    titem->setVisible (true);
    counter ++;
    visiblegriditems = counter;
    if (counter > GRIDsize)
      GRIDsize = counter;
  }

  step = gridstep;
  while (step < chartbottom)
    step += gridstep;

// horizontal grid
  for (; step < maxstep; step += gridstep)
  {
    stepstr = QString ("%1").arg (((qreal) step) * points, 10, 'f', fracdig);
    y = yOnPrice (step * points);
    if (counter >= GRIDsize)
    {
      litem = new QTCGraphicsItem (type);
      litem->setZValue (0.0);
      GRID[counter] = litem;
      titem = new QGraphicsTextItem (litem);
      titem->setFont (gridFont);
      if (counter == 0)
      {
        titem = new QGraphicsTextItem (litem);
        titem->setFont (gridFont);
      }
      litem->init (scene);
    }

    litem = GRID[counter];
    litem->setPen (pen);
    litem->setLine (QLineF (chartleftmost, y, chartrightmost, y));
    titem = ((QGraphicsTextItem *)(litem->children ().at(0)));
    titem->setDefaultTextColor (textcolor);
    titem->setPlainText (stepstr);
    titem->setPos (ruller_cursor_x, y - 10);
    litem->setVisible (true);
    titem->setVisible (true);
    counter ++;
    visiblegriditems = counter;
    if (counter > GRIDsize)
      GRIDsize = counter;
  }

  step = charttop;
  stepstr = QString ("%1").arg (((qreal) step) * points, 10, 'f', fracdig);
  y = yOnPrice (step * points);
  titem = ((QGraphicsTextItem *)(GRID[0]->children ().at(1)));
  titem->setDefaultTextColor (textcolor);
  titem->setPlainText (stepstr);
  titem->setPos (ruller_cursor_x, y - 10);
  gridstep = keepgridstep;
// vertical grid
  if (HLOC->size () < 2)
    return;

  width = framewidth;
  pframe = HLOC->at (*startbar);
  for (qint32 xcounter = (*startbar) + 1, i = 1,
       max1 = ((*startbar) + nbars_on_chart), max2 = HLOC->size ();
       xcounter < max1 && xcounter < max2; xcounter ++, i++)
  {
    QRectF rectf;

    framestr = QStringLiteral ("");
    x = -1;

    if (currenttf == QLatin1String ("DAY"))
    {
      if (pframe.month != HLOC->at (xcounter).month)
      {
        x = chartrightmost - ((qreal) ((width * 1.5) * i));
        x -= (*excess_drag_width + 1);
        framestr = monthlit[pframe.month - 1] % QStringLiteral (" ") % QString::number (pframe.year);
      }
    }
    else if (currenttf == QLatin1String ("WEEK"))
    {
      if (pframe.year != HLOC->at (xcounter).year)
      {
        x = chartrightmost - ((qreal) ((width * 1.5) * i));
        x -= (*excess_drag_width + 1);
        framestr = QString::number (pframe.year);
      }
    }
    else if (currenttf == QLatin1String ("MONTH"))
    {
      if ((HLOC->at (xcounter).year + 1) % 5 == 0 && HLOC->at (xcounter).month == 12)
      {
        x = chartrightmost - ((qreal) ((width * 1.5) * i));
        x -= (*excess_drag_width + 1);
        framestr = QString::number (pframe.year);
      }
    }
    else if (currenttf == QLatin1String ("YEAR"))
    {
      if ((HLOC->at (xcounter).year + 1) % 10 == 0)
      {
        x = chartrightmost - ((qreal) ((width * 1.5) * i));
        x -= (*excess_drag_width + 1);
        framestr = QString::number (pframe.year);
      }
    }

    if (x != -1)
    {
      if (counter >= GRIDsize)
      {
        litem = new QTCGraphicsItem (type);
        litem->setZValue (0.0);
        titem = new QGraphicsTextItem (litem);
        titem->setFont (gridFont);
        litem->init (scene);
        GRID[counter] = litem;
      }

      litem = GRID[counter];
      litem->setPen (pen);
      litem->setLine (QLineF (x, charttopmost, x, (height - (bottomline_height + chartframe)) - 5));
      titem = ((QGraphicsTextItem *)(litem->children ().at(0)));
      titem->setDefaultTextColor (textcolor);
      titem->setPlainText (framestr);
      titem->setPos (x, chartbottomost - 15);
      titem->setVisible (true);
      litem->setVisible (true);

      if (counter >= visiblegriditems)
      {
        rectf = titem->boundingRect();
        if (chartrightmost - rectf.width () < x)
          titem->setVisible (false);
      }

      counter ++;
      visiblegriditems = counter;
      if (counter > GRIDsize)
        GRIDsize = counter;
    }
    pframe = HLOC->at (xcounter);
  }
}

// draw volumes
void
QTAChartCore::drawVolumes (void)
{
  QTACObject *vbars;

  if (volumes != NULL)
    return;

  volumes = new QTACObject (this, QTACHART_OBJ_SUBCHART);
  volumes->setTitle ("Volume");
  volumes->setAttributes (QTACHART_VOLUME, 0, "", DUMMY, 0, QREAL_MAX, forecolor, "");
  vbars = new QTACObject (volumes, QTACHART_OBJ_VBARS);
  vbars->setAttributes (QTACHART_VOLUME, 0, "", DUMMY, 0, QREAL_MAX, forecolor, "");
  propScr->setVolumes (true);
  show_volumes = propScr->Volumes ();
}

// delete grid
void
QTAChartCore::deleteGRID (void)
{
  for (qint32 counter = 0; counter < GRIDsize; counter ++)
    delete GRID[counter];

  GRIDsize = 0;
}

// delete volumes
void
QTAChartCore::deleteVolumes (void)
{
  if (volumes != NULL)
  {
    deleteObject (volumes);
    draw ();
  }
}

// delete object
void
QTAChartCore::deleteObject (QTACObject *obj)
{
  if (obj == NULL)
    return;

  if (Object.indexOf (obj) != -1)
  {

    if (obj == volumes)
    {
      propScr->setVolumes (false);
      show_volumes = propScr->Volumes ();
      volumes = NULL;
    }

    if (obj == onlineprice)
    {
      propScr->setOnlinePrice (false);
      show_onlineprice = propScr->OnlinePrice ();
      onlineprice = NULL;
    }

    Object.remove (Object.indexOf(obj, 0));

    if (obj->type == QTACHART_OBJ_SUBCHART)
    {
      obj->subchart_dec = 0;
      nsubcharts --;
    }

    delete obj;
  }
}

// delete online price
void
QTAChartCore::deleteOnlinePrice ()
{
  if (onlineprice != NULL)
  {
    onlineprice->setForDelete ();
    draw ();
  }
}

// delete ITEMS
void
QTAChartCore::deleteITEMS ()
{
  for (qint32 counter = 0; counter < ITEMSsize; counter ++)
    delete ITEMS[counter];

  ITEMSsize = 0;
}

/// Ee
/// Ff
/// Gg
// form the bottom text
QString
QTAChartCore::getBottomText (int x)
{
  QString btext = QStringLiteral (" ");
  static int last_bar = -1;
  int bar = 0;

  bar = barOnX (x);

  if (bar == -1)
    return btext;

  if (bar == last_bar)
    return bottom_text->toPlainText ();

  if (Q_LIKELY (bar != -1))
    btext = (*HLOC).at (bar).Text;

  currentbar = last_bar = bar;

  foreach (QTACObject *obj, Object)
    obj->moduleEvent (EV_MOUSE_ON_BAR);

  return btext;
}

QString
QTAChartCore::getBottomTextHA (int x)
{
  QString btext = QStringLiteral (" ");
  static int last_bar = -1;
  int bar = 0;

  bar = barOnX (x);

  if (bar == -1)
    return btext;

  if (bar == last_bar)
    return bottom_text->toPlainText ();

  if (Q_LIKELY (bar != -1 && bar < (*HEIKINASHI).size ()))
    btext = (*HEIKINASHI).at(bar).Text;

  currentbar = last_bar = bar;

  foreach (QTACObject *obj, Object)
    obj->moduleEvent (EV_MOUSE_ON_BAR);

  return btext;
}

QString
QTAChartCore::getTabText (void) const
{
  QTAChart *chart = qobject_cast <QTAChart *> (parent ());

  return chart->getTabText ();
}

/// Hh
// hide all chart's objects
void
QTAChartCore::hideAllItems (void)
{
  QList < QGraphicsItem * >sceneitems;

  sceneitems = scene->items ();
  if (Q_UNLIKELY(sceneitems.size () == 0))
    return;

  foreach (QGraphicsItem *item, sceneitems)
    item->setVisible (false);
  scene->setBackgroundBrush (Qt::black);
  scene->update ();
}

/// Ii
// initialize the core
void
QTAChartCore::init ()
{
  drawScr->setReferenceChart (static_cast <void*> (this->parent ()));
  functionScr->setReferenceChart (static_cast <void*> (this->parent ()));
  propScr->setReferenceChart (static_cast <void*> (this->parent ()));
  objectsScr->setReferenceChart (static_cast <void*> (this->parent ()));
}
/// Jj
/// Kk
/// Ll
// chart setting callback
static int
sqlcb_chart_setting (void *data, int argc, char **argv, char **column)
{
  QString colname;

  if (argc > 1)
    return 1;

  colname = QString (column[0]).toLower ();
  if (colname == QLatin1String ("timeframe"))
  {
    char *csetting;
    csetting = (char *) data;
    strcpy (csetting, argv[0]);
  }
  else if (colname == QLatin1String ("edw"))
  {
    float *rsetting;
    rsetting = (float *) data;
    *rsetting = (float) strtod (argv[0], NULL);
  }
  else
  {
    qint64 *isetting;
    isetting = (qint64 *) data;
    *isetting = (qint64) atoll (argv[0]);
  }

  return 0;
}

// load settings
void
QTAChartCore::loadSettings ()
{
  QString SQLCommand;
  qint64 isetting = -1;
  float rsetting;
  int rc;
  char csetting[16];

  // volume
  SQLCommand = QStringLiteral ("SELECT VOLUME FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    if (isetting == -1)
    {
      propScr->setVolumes (show_volumes);
      setLinearScale (linear);
      propScr->setOnlinePrice (show_onlineprice);
      propScr->setGrid (show_grid);
      setChartStyle (chart_style);
      propScr->setLineColor (linecolor);
      propScr->setBarColor (barcolor);
      propScr->setForeColor (forecolor);
      propScr->setBackColor (backcolor);
      scene->setBackgroundBrush (backcolor);
      ruller_cursor->setDefaultTextColor (backcolor);
      ruller_cursor->setDefaultBackgroundColor (forecolor);
      changeForeColor (forecolor);
      return;
    }

    if (isetting == 1)
      show_volumes = true;
    else
      show_volumes = false;
    propScr->setVolumes (show_volumes);
  }

  // linear
  SQLCommand = QStringLiteral ("SELECT LINEAR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    if (isetting == 1)
      setLinearScale (true);
    else
      setLinearScale (false);
  }

  // online price
  SQLCommand = QStringLiteral ("SELECT ONLINEPRICE FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    if (isetting == 1)
      show_onlineprice = true;
    else
      show_onlineprice = false;
    propScr->setOnlinePrice (show_onlineprice);
  }

  // grid
  SQLCommand = QStringLiteral ("SELECT GRID FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    if (isetting == 1)
      show_grid = true;
    else
      show_grid = false;
    propScr->setGrid (show_grid);
  }

  // chart style
  SQLCommand = QStringLiteral ("SELECT CHARTSTYLE FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    setChartStyle (isetting);
  }

  // frame width
  SQLCommand = QStringLiteral ("SELECT FRAMEWIDTH FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
    framewidth = isetting;

  // linecolor
  SQLCommand = QStringLiteral ("SELECT LINECOLOR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    linecolor = QColor ((QRgb) isetting);
    propScr->setLineColor (linecolor);
  }

  // barcolor
  SQLCommand = QStringLiteral ("SELECT BARCOLOR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    barcolor = QColor ((QRgb) isetting);
    propScr->setBarColor (barcolor);
  }

  // forecolor
  SQLCommand = QStringLiteral ("SELECT FORECOLOR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    forecolor = QColor ((QRgb) isetting);
    textcolor = gridcolor = framecolor = forecolor;
    propScr->setForeColor (forecolor);
    bottom_text->setDefaultTextColor (textcolor);
  }

  // backcolor
  SQLCommand = QStringLiteral ("SELECT BACKCOLOR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
  {
    backcolor = QColor ((QRgb) isetting);
    propScr->setBackColor (backcolor);
  }

  // startbar
  SQLCommand = QStringLiteral ("SELECT STARTBAR FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&isetting));
  if (rc == SQLITE_OK)
    xstartbar = isetting;

  // excess drag width
  SQLCommand = QStringLiteral ("SELECT EDW FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (&rsetting));
  if (rc == SQLITE_OK)
    xexcess_drag_width =  rsetting;

  // time frame
  SQLCommand = QStringLiteral ("SELECT TIMEFRAME FROM chart_settings WHERE KEY = '") %
               SymbolKey % QStringLiteral ("';");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_chart_setting, static_cast <void *> (csetting));
  if (rc == SQLITE_OK)
    currenttf = QString::fromUtf8 (csetting);

  scene->setBackgroundBrush (backcolor);
  ruller_cursor->setDefaultTextColor (backcolor);
  ruller_cursor->setDefaultBackgroundColor (forecolor);
  changeForeColor (forecolor);
}

/// Mm
// manage objects
void
QTAChartCore::manageObjects (void)
{
  events_enabled = false;
  hideAllItems ();
  expandicon =  QIcon (QString (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  (QStringLiteral ("background: transparent;color: white;font: 11px;"));
  expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  prxobjectsScr->resize (width, height - 40);
  expandBtn->setVisible (true);
  prxobjectsScr->setVisible (true);
}

/// Nn
/// Oo
/// Pp
// find the corresponding price of y
GNUFASTCALL qreal MSVCFASTCALL
QTAChartCore::priceOnY (int y)
{
  // linear chart
  if (linear)
  {
    qreal price, k;
    qint64 diff;
    diff = (y - charttopmost);
    k = (qreal) diff / dots_per_point;
    price = points * ((qreal) charttop - k);
    return price;
  }

  // semilog chart
  qreal relpos, bottom, k, price;
  /*
    bottom = qLog10 ((qreal) chart->chartbottom * chart->points);
    relpos = ((qreal) y - (qreal) chart->chartbottomost) /
             ((qreal) (chart->chartbottomost - chart->charttopmost));
    relpos = qAbs (relpos);
    k =  (relpos *
         ((qLog10 (((qreal) chart->charttop) * chart->points) -
         bottom))) + bottom;

    price = qPow (10,k);
  */
  bottom = qLn ((qreal) chartbottom * points);
  relpos = ((qreal) y - (qreal) chartbottomost) /
           ((qreal) (chartbottomost - charttopmost));
  relpos = qAbs (relpos);
  k =  (relpos *
        ((qLn (((qreal) charttop) * points) - bottom))) + bottom;
  price = qPow (2.718281,k);

  return price;
}

/// Qq
/// Rr
// restore the bottom text
void
QTAChartCore::restoreBottomText ()
{
  QFont font;

  font = bottom_text->font ();
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  bottom_text->setFont (font);
}

/// Ss

// subchart button slot
void
QTAChartCore::SCBtn_clicked ()
{
  SubChartButton *btn = qobject_cast <SubChartButton *> (QObject::sender());

  if (btn->objectName () == QLatin1String ("Close"))
  {
    deleteObject (btn->getOwner ());
    draw ();
    return;
  }
  else if (btn->objectName () == QLatin1String ("Edit"))
  {
    bool modrslt;
    QTACObject *object;
    events_enabled = false;
    object = btn->getOwner ();
    modrslt = object->modifyIndicator ();
    if (modrslt == false)
      deleteObject (object);
    else
      setBottomText (last_x);
  }
  events_enabled = true;
  draw ();
}

// save settings
void
QTAChartCore::saveSettings () const
{
  TemplateManagerDialog *tmpldlg;
  QString SQLCommand;
  int rc;

  tmpldlg = new TemplateManagerDialog;
  tmpldlg->setReferenceChart (parent ());

  SQLCommand =
    QStringLiteral ("DELETE FROM chart_settings WHERE KEY = '") %
    SymbolKey % QStringLiteral ("';") %
    QStringLiteral ("INSERT INTO chart_settings ") %
    QStringLiteral ("(key, volume, linear, chartstyle, timeframe, onlineprice,") %
    QStringLiteral (" linecolor, barcolor, framewidth, startbar, edw, ") %
    QStringLiteral (" forecolor, backcolor, grid) ") %
    QStringLiteral ("VALUES ('") % SymbolKey % QStringLiteral ("',");

  // volume
  if (show_volumes)
    SQLCommand += QStringLiteral ("1, ");
  else
    SQLCommand += QStringLiteral ("0, ");

  // linear
  if (linear)
    SQLCommand += QStringLiteral ("1, ");
  else
    SQLCommand += QStringLiteral ("0, ");

  // chartstyle
  SQLCommand += QString::number (chart_style) % QStringLiteral (", ") %

                // timeframe
                QStringLiteral ("'") % currenttf % QStringLiteral ("', ");

  // online price
  if (show_onlineprice)
    SQLCommand += QStringLiteral ("1, ");
  else
    SQLCommand += QStringLiteral ("0, ");

  // line color
  SQLCommand += QString::number ((qreal) linecolor.rgb (), 'f', 0) % QStringLiteral (", ") %

  // bar color
                QString::number ((qreal) barcolor.rgb (), 'f', 0) % QStringLiteral (", ") %

  // frame width
                QString::number (framewidth) % QStringLiteral (", ") %

  // startbar
                QString::number (*startbar) % QStringLiteral (", ") %

  // excess_drag_width
                QString::number (*excess_drag_width, 'f', 1) % QStringLiteral (", ") %

  // foreground color
                QString::number ((qreal) forecolor.rgb (), 'f', 0) % QStringLiteral (", ") %

  // background color
                QString::number ((qreal) backcolor.rgb (), 'f', 0) % QStringLiteral (", ");

  // grid
  if (show_grid)
    SQLCommand += QStringLiteral ("1); ");
  else
    SQLCommand += QStringLiteral ("0); ");

  SQLCommand +=
    QStringLiteral ("DROP TABLE IF EXISTS template_") % SymbolKey % QStringLiteral (";") %
    QStringLiteral ("CREATE TABLE template_") % SymbolKey % QStringLiteral (" AS ") %
    QStringLiteral ("SELECT * FROM templatemodel;") % tmpldlg->qtachart2sql (SymbolKey);

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_TRANSACTION, __FILE__, __LINE__);
  }

  delete tmpldlg;
}

// set chart's title
void
QTAChartCore::setTitle (QString Title, QString Subtitle)
{
  QString titlestring, subtitlestring;
  QByteArray ba;

  Title = Title.trimmed ();
  Subtitle = Subtitle.trimmed ();
  if (Title == QLatin1String (""))
    Title = QStringLiteral ("Unknown");
  if (Subtitle == QLatin1String (""))
    Subtitle = QStringLiteral ("Unknown");

  std::memset (titletext, 0, STRMAXLEN);
  titlestring = qPrintable (Title);
  ba = titlestring.toLocal8Bit ();
  strncpy (titletext, ba.data (), STRMAXLEN - 1);
  titlestring = QString::fromLocal8Bit (titletext, -1);
  title->setPlainText (TO_UTF8 (titlestring));
  title->setDefaultTextColor (textcolor);

  std::memset (subtitletext, 0, STRMAXLEN);
  subtitlestring = qPrintable (Subtitle);
  subtitlestring = subtitlestring.left (40);
  ba = subtitlestring.toLocal8Bit ();
  strncpy (subtitletext, ba.data (), STRMAXLEN - 1);
  subtitlestring = QString::fromLocal8Bit (subtitletext, -1);
  subtitle->setPlainText (TO_UTF8 (subtitlestring));
  subtitle->setDefaultTextColor (textcolor);
}

// set chart's style
void
QTAChartCore::setChartStyle (int style)
{
  propScr->setChartStyle (style);
  chart_style = propScr->ChartStyle ();

  if (style == QTACHART_CANDLE)
    typetitle->setPlainText ("Candle");
  else if (style == QTACHART_HEIKINASHI)
    typetitle->setPlainText ("Heikin-Ashi");
  else if (style == QTACHART_BAR)
    typetitle->setPlainText ("Bar");
  else if (style == QTACHART_LINE)
    typetitle->setPlainText ("Line");
}

// set chart's style
void
QTAChartCore::setLinearScale (bool scale)
{
  propScr->setLinearScale (scale);
  linear = propScr->LinearScale ();

  if (linear)
    scaletitle->setPlainText ("Linear");
  else
    scaletitle->setPlainText ("Logarithmic");
}

// set the bottom text
void
QTAChartCore::setBottomText (int x)
{
  if (chart_style == QTACHART_HEIKINASHI)
    bottom_text->setPlainText (getBottomTextHA (x));
  else
    bottom_text->setPlainText (getBottomText (x));

  foreach (QTACObject *obj, Object)
  {
    if (obj->type == QTACHART_OBJ_SUBCHART)
      obj->setDataTitle (x);
  }
}

// set the bottom text to custom string
void
QTAChartCore::setCustomBottomText (QString string)
{
  QFont font;

  font = bottom_text->font ();
  font.setPixelSize (10 + CHART_FONT_SIZE_PAD);
  bottom_text->setFont (font);

  bottom_text->setPlainText (string);
  bottom_text->setDefaultTextColor (textcolor);
  foreach (QTACObject *obj, Object)
  {
    if (obj->type == QTACHART_OBJ_SUBCHART)
      obj->setDataTitle (chartrightmost - *excess_drag_width);
  }
}

// set the charts properties
void
QTAChartCore::setChartProperties (void)
{
  events_enabled = false;
  hideAllItems ();
  expandicon = QIcon (QStringLiteral (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  (QStringLiteral ("background: transparent;color: white;font: 11px;"));
  expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  prxpropScr->resize (width, height - 40);
  prxpropScr->setVisible (true);
  expandBtn->setVisible (true);
  propScr->setVolumes (show_volumes);
  propScr->setGrid (show_grid);
  setChartStyle (chart_style);
  setLinearScale (linear);
  propScr->setOnlinePrice (show_onlineprice);
}

// select object to draw
void
QTAChartCore::selectDrawObject (void)
{
  events_enabled = false;
  hideAllItems ();
  prxdrawScr->resize (width, height - 40);
  expandicon = QIcon (QStringLiteral (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  (QStringLiteral ("background: transparent;color: white;font: 11px;"));
  expandBtn->setToolTip  (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  expandBtn->setVisible (true);
  prxdrawScr->setVisible (true);
}

// select function to apply
void
QTAChartCore::selectFunction (void)
{
  events_enabled = false;
  hideAllItems ();
  prxfunctionScr->resize (width, height - 40);
  expandicon = QIcon (QStringLiteral (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  (QStringLiteral ("background: transparent;color: white;font: 11px;"));
  expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  expandBtn->setVisible (true);
  prxfunctionScr->setVisible (true);
}

// set the price cursor
void
QTAChartCore::setRullerCursor (int y)
{
  QString pricestr;
  qreal price;

  if (y <= charttopmost)
    y = charttopmost;

  if (y >= chartbottomost)
    y = chartbottomost;

  price = priceOnY (y);
  if (Q_UNLIKELY (price < 0))
    return;

  pricestr = QString ("%1").arg (price, 10, 'f', fracdig + 1);
  ruller_cursor_y = y;
  ruller_cursor->setPos (ruller_cursor_x, y - 10);
  ruller_cursor->setPlainText (pricestr);
}

// show all chart's objects
void
QTAChartCore::showAllItems (void)
{
  QList < QGraphicsItem * >sceneitems;
  QGraphicsItem *item;

  sceneitems = scene->items ();
  if (Q_UNLIKELY (sceneitems.size () == 0))
    return;
  foreach (item, sceneitems)
    item->setVisible (true);
}

// show help
void
QTAChartCore::showHelp (void)
{
  events_enabled = false;
  hideAllItems ();
  prxhelpScr->resize (width, height - 40);
  expandicon = QIcon (QStringLiteral (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  (QStringLiteral ("background: transparent;color: white;font: 11px;"));
  expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  expandBtn->setVisible (true);
  prxhelpScr->setVisible (true);
}

// show data
void
QTAChartCore::showData (void)
{
  events_enabled = false;
  hideAllItems ();
  prxdataScr->resize (width, height - 40);
  expandicon = QIcon (QString (":/png/images/icons/PNG/Button_Back.png"));
  expandBtn->setIcon (expandicon);
  expandBtn->setStyleSheet
  ("background: transparent;color: white;font: 11px;");
  expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Back (Alt+Z)</span>"));
  expandBtn->setVisible (true);
  prxdataScr->setVisible (true);
}

// sets param dialog to open, returns true on success
bool
QTAChartCore::setPdlgOpened ()
{
  if (paramdlgopened.fetchAndAddAcquire (0) == 1)
    return false;

  paramdlgopened = 1;
  return true;
}

//sets param dialog to closed
void
QTAChartCore::setPdlgClosed ()
{
  paramdlgopened = 0;
}

/// Tt
// time frame button slot
void
QTAChartCore::tfBtn_clicked ()
{
  QString tfsymbol;
  QToolButton *btn;

  btn = (QToolButton *) QTAChartCore::sender ();
  tfsymbol = btn->text ();

  for (qint32 counter = 0, maxcounter = TIMEFRAME.size ();
       counter < maxcounter; counter ++)
  {
    if (TIMEFRAME.at(counter).TFSymbol == tfsymbol)
    {
      QString title;
      HLOC = &TIMEFRAME[counter].HLOC;
      HEIKINASHI = &TIMEFRAME[counter].HEIKINASHI;
      startbar = &(TIMEFRAME[counter].TFStartBar);
      excess_drag_width = &TIMEFRAME[counter].TFExcess_Drag_Width;
      title = Symbol % QStringLiteral (" - ") % TIMEFRAME.at (counter).TFName;
      setTitle (title, QString::fromUtf8 (subtitletext));
      currenttf = TIMEFRAME.at(counter).TFName;
    }
  }

  reloaded = true;
  draw ();
}
/// Uu
/// Vv
/// Ww
/// Xx
/// Yy
/********************************************************/
/** logarithmic price scale:                           **/
/** x'i = (log(xi)-log(xmin)) / (log(xmax)-log(xmin))  **/
/** x'i = relative position (0 <= x'i <=1)             **/
/********************************************************/

// find the corresponding y of a price
GNUFASTCALL qreal MSVCFASTCALL
QTAChartCore::yOnPrice (qreal price) const
{
  if (price == 0)
    return (qreal) chartbottomost;

  // linear chart
  if (linear)
  {
    qreal y, k, xpoints;
    xpoints = (qreal) charttop;
    k = price / points;
    xpoints -= k;
    xpoints = xpoints * dots_per_point;
    y = (qreal) charttopmost;
    y += xpoints;
    return y;
  }

  // semilog chart
  qreal y, relpos, bottom;
  /*
    bottom = qLog10 (((qreal) chart->chartbottom) * chart->points);
    relpos = (qLog10 (price) - bottom) /
             (qLog10 (((qreal) chart->charttop) * chart->points) -
              bottom);
  */
  bottom = qLn (((qreal) chartbottom) * points);
  relpos = (qLn (price) - bottom) /
           (qLn (((qreal) charttop) * points) - bottom);

  y = ((qreal) chartbottomost) -
      ((qreal) (chartbottomost - charttopmost) * relpos);

  return y;
}

/// Zz
