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

// draw/refresh the object
void
drawObject (QTACObject *object)
{
  Q_UNUSED (QTACastFromConstVoid)

  QTAChartCore *core = static_cast <QTAChartCore *> ((void *) object->chartdata);
  QTACObject *child;
  QString timeframe = QStringLiteral ("");
  qint32 max;

  if (object->enabled == false)
  {
    object->text->setVisible (false);
    object->title->setVisible (false);
    object->editBtn->setVisible (false);
    object->closeBtn->setVisible (false);
    object->setForDelete ();
    return;
  }

  if (core->events_enabled == false)
    return;

  object->basex = core->chartleftmost;
  object->width = core->chartwidth;

  if (object->moduleName != QLatin1String (""))
  {
    if (!object->moduleLoop ())
      return;
  }

  if (object->type == QTACHART_OBJ_CONTAINER)
  {
    object->title->setVisible (false);
    return;
  }

  if (object->type != QTACHART_OBJ_SUBCHART) // object is not a subchart
  {
    if (object->parentObject == NULL) // object belongs to main chart
    {
      if (core->reloaded)
        object->valueSet ();

      object->basey = core->charttopmost;
      object->height = core->chartheight;
    }
    else                // object belongs to a subchart
    {
      object->basey = object->parentObject->basey;
      object->height = object->parentObject->height;
    }

    switch (object->type)
    {
      case QTACHART_OBJ_LABEL:
        object->drawLabel ();
        return;
      case QTACHART_OBJ_TEXT:
        object->drawText ();
        return;
      case QTACHART_OBJ_HLINE:
      case QTACHART_OBJ_VLINE:
        object->drawHVLine ();
        return;
      case QTACHART_OBJ_LINE:
        object->drawTLine ();
        return;
      case QTACHART_OBJ_FIBO:
        object->drawFibo ();
        return;
    }

    object->X.clear ();

    if (object->valueset == NULL)
      return;

    qint32 vsetsize = object->valueset->size ();
    max = core->nbars_on_chart + *core->startbar;
    object->X.reserve (core->nbars_on_chart);
    for (qint32 counter = *core->startbar, i = 0;
         counter < max && counter < vsetsize;
         counter ++, i ++)
    {
      qreal x;
      x = object->width - (core->framewidth * 1.5 * (i + 1)) + 1;
      x += (core->framewidth * 0.8) / 2;
      x += object->basex;
      object->X += x;
    }

    object->Y.clear ();
    object->Y.reserve (object->X.size ());

    for (qint32 counter = *core->startbar;
         counter < max && counter < vsetsize;
         counter ++)
    {
      if (object->valueset->at(counter) == 0 &&
          object->parentObject == NULL)
        object->Y += 0;
      else
      {
        qreal y;
        if (object->parentObject == NULL ||
            object->parentObject->type == QTACHART_OBJ_CURVE)
          y = core->yOnPrice (object->valueset->at(counter));
        else
          y = object->subYonPrice (object->valueset->at(counter));
        object->Y += y;
      }
    }

    if (object->type == QTACHART_OBJ_VBARS)
    {
      object->drawVBars ();
      return;
    }

    if (object->type == QTACHART_OBJ_CURVE ||
        object->type == QTACHART_OBJ_DOT)
    {
      object->title->setVisible (false);

      if (*core->startbar == 0 && object->dataset->size () > object->period)
      {
        if (object->parentObject == NULL)
        {
          QRectF rectf;
          rectf = object->title->boundingRect();
          if (rectf.width () + (object->X.at (0) + 5) < core->chartrightmost)
          {
            object->title->setVisible (true);
            object->title->setDefaultTextColor (object->forecolor);
            object->title->setPos (object->X.at (0) + 5, object->Y.at (0) - 12);
          }
        }
      }

      if (object->type == QTACHART_OBJ_CURVE)
        object->drawCurve ();
      else
        object->drawDot ();
    }
  }
  else // object is a subchart
  {
    QPen pen;
    quint32 chart_sequence = 0, nobjects;

    if (core->reloaded)
      object->valueSet ();

    // no tooltip for title
    object->title->setToolTip (QStringLiteral (""));

    // determine subchart's height
    if (core->size_changed)
    {
      object->height = core->height -
                       (core->chartheight + core->bottomline_height +
                        core->title_height + (core->chartframe * 2));
      object->height /= core->nsubcharts;
    }

    // initialize rangemin and range max
    if (object->rmin == QREAL_MIN)
      object->rangemin = QREAL_MAX;
    else
      object->rangemin = object->rmin;

    if (object->rmax == QREAL_MAX)
      object->rangemax = QREAL_MIN;
    else
      object->rangemax = object->rmax;

    foreach (child, object->children)
      child->minmax ();

    foreach (child, object->children)
    {
      if (child->type == QTACHART_OBJ_CURVE ||
          child->type == QTACHART_OBJ_VBARS)
      {
        if (object->rangemax < child->rangemax)
          object->rangemax = child->rangemax;
        if (object->rangemin > child->rangemin)
          object->rangemin = child->rangemin;
      }
    }

    object->quantum = (qAbs (object->rangemax - object->rangemin) /
                       (object->height - 35));
    if (object->quantum == 0)
      object->quantum = 1;

    foreach (child, object->children)
    {
      child->quantum = object->quantum;
      child->rangemin = object->rangemin;
      child->rangemax = object->rangemax;
    }

    // set title
    /*
    if (object->period > 0)
      object->title->setPlainText (object->titlestr % "(" %
      (object->period > 0?QString::number (object->period, 'f', 0):"") % "):" % object->datastr);
    */

    // hide edit button if there is no parameters' dialog
    if (object->paramDialog.isNull ())
      object->editBtn->setVisible (false);

    // bottom line's pen
    pen.setColor (Qt::darkYellow);
    pen.setStyle (Qt::SolidLine);
    pen.setWidth (2);

    // determine subchart's sequence
    nobjects = core->Object.size ();
    for (quint32 counter = 0; counter < nobjects &&
         core->Object.at (counter) != object;
         counter ++)
    {
      const QTACObject *objcursor = core->Object.at (counter);
      if (objcursor->type == QTACHART_OBJ_SUBCHART &&
          objcursor->deleteit == false)
        chart_sequence ++;
    }

    if (core->size_changed)
    {
      // determine subchart's y
      object->basey = core->chartbottomost + (chart_sequence * object->height);

      object->title->setPos (object->basex , object->basey + 5);
      if (object->bottomline != NULL)
      {
        object->scene->removeItem (object->bottomline);
        delete object->bottomline;
        object->bottomline = NULL;
      }

      // add bottom line
      object->bottomline = object->scene->qtcAddLine (object->basex + 1,
                           object->basey + object->height,
                           core->width,
                           object->basey + object->height, pen);

      // add close and edit buttons
      object->prxcloseBtn->setGeometry (QRectF (core->chartrightmost - 24, object->basey + 4, 22, 22));
      object->prxeditBtn->setGeometry (QRectF (core->chartrightmost - 64, object->basey + 7, 30, 16));
    }
  }

  // draw children objects
  foreach (child, object->children)
    drawObject (child);
}

// draw label
void
QTACObject::drawLabel ()
{
  if (text == NULL)
    return;

  QRectF rect;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  qreal x, y, width, height;

  text->setVisible (false);
  title->setVisible (false);

  if (deleteit)
    return;

  height = qAbs (core->chartbottomost - core->charttopmost);
  width =  qAbs (core->chartrightmost - core->chartleftmost);
  x = width * relx;
  y = height * rely;
  x += core->chartleftmost;
  y += core->charttopmost;

  rect = text->boundingRect ();

  if (x < core->chartleftmost)
    x = core->chartleftmost;
  if (y < core->charttopmost)
    y = core->charttopmost;
  if (x + rect.width () > core->chartrightmost)
    x = core->chartrightmost - rect.width ();
  if (y + rect.height () > core->chartbottomost)
    y =  (core->chartbottomost - rect.height ());

  text->setPos (x, y);
  text->setVisible (true);

  objx = x;
  objy = y;
}

// draw text
void
QTACObject::drawText ()
{
  QRectF rect;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  qreal x = -1, y = 0;
  int found = -1;

  text->setVisible (false);
  title->setVisible (false);
  if (deleteit)
    return;

  for (qint32 counter = 0; counter < core->nbars_on_chart; counter ++)
  {
    if ( counter + *core->startbar < core->HLOC->size ())
      if (trailerCandleText == (*core->HLOC).at (counter + *core->startbar).Text)
        found = counter;
  }

  if (found == -1)
    return;

  rect = text->boundingRect ();

  y = core->yOnPrice (price);

  if (vadjust == QTACHART_OBJ_VADJUST_CENTER)
    y -= rect.height () / 2;
  else if (vadjust == QTACHART_OBJ_VADJUST_ABOVE)
    y -= (rect.height () + 2);
  else if (vadjust == QTACHART_OBJ_VADJUST_BELOW)
    y += 2;

  x =  core->chartwidth - (core->framewidth * 1.5 * (found + 1)) + 1;
  x += (core->framewidth * 0.8) / 2;
  x += core->chartleftmost + 1;

  if (hadjust == QTACHART_OBJ_HADJUST_CENTER)
    x -= rect.width () / 2;
  else if (hadjust == QTACHART_OBJ_HADJUST_LEFT)
    x -= (rect.width () + 2);
  else if (hadjust == QTACHART_OBJ_HADJUST_RIGHT)
    x += 2;

  text->setPos (x, y);

  if (core->chartrightmost - x < rect.width ())
    return;

  if (y < core->charttopmost)
    return;

  if (x < core->chartleftmost)
    x = core->chartleftmost;
  if (y < core->charttopmost)
    y = core->charttopmost;
  if (x + rect.width () > core->chartrightmost)
    x = core->chartrightmost - rect.width ();
  if (y + rect.height () > core->chartbottomost)
    y =  (core->chartbottomost - rect.height ());

  text->setVisible (true);
  objx = x;
  objy = y;
}

// draw vertical bars
void
QTACObject::drawVBars ()
{
  QPen pen;
  QTCGraphicsItem *item;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  qreal x, y, w, h, y0;
  int type = RectItemType, icounter = 0;

  pen.setColor (forecolor);
  clearITEMS ();

  y0 = subYonPrice (0);
  w = core->framewidth / ((3 * 0.85) / (thickness + 0.1));
  pen.setWidthF (w);

  const qint32 Ys = Y.size ();
  for (qint32 counter = 0; counter < Ys; counter ++)
  {
    x = X.at (counter);

    if (y0 > Y.at (counter))
      y = Y.at (counter);
    else
      y = y0;

    h = qAbs (Y.at (counter) - y0);
    if (Q_UNLIKELY (icounter >= ITEMSsize))
    {
      item = new QTCGraphicsItem (type);
      item->setRect (QRectF (x, y, w, h));
      ITEMS[icounter] = item;
      item->init (scene);
      item->setZValue (0.5);
    }
    else
    {
      item = ITEMS[icounter];
      item->setRect (QRectF (x, y, w, h));
    }

    item->setPen (pen);
    item->setVisible (true);
    icounter ++;
  }

  visibleitems = icounter;
  if (icounter > ITEMSsize)
    ITEMSsize = icounter;
}

// draw curve
void
QTACObject::drawCurve ()
{
  QPen pen;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  QTCGraphicsItem *item;
  qreal x1, x2, y1, y2;
  int type = LineItemType, icounter = 0;

  clearITEMS ();

  if (Q_UNLIKELY(Y.size () < 2))
    return;

  x1 = X.at (0);
  y1 = Y.at (0);

  if (Q_UNLIKELY(y1 == 0))
    return;

  pen.setColor (forecolor);
  pen.setWidth (thickness);

  for (qint32 counter = 1, maxcounter = Y.size ();
       counter < maxcounter; counter ++)
  {
    x2 = X.at (counter);
    y2 = Y.at (counter);
    if (Q_UNLIKELY(y2 == 0))
      break;

    if (Q_LIKELY(x1 > core->chartleftmost && x2 > core->chartleftmost))
      if (Q_LIKELY(y1 > basey && y1 < (basey + height)))
        if (Q_LIKELY(y2 > basey && y2 < (basey + height)))
        {
          /*
              if (Q_UNLIKELY (icounter >= ITEMSsize))
                {
                  item = new QTCGraphicsItem (type);
                  item->setLine (QLineF (x1, y1, x2, y2));
                  ITEMS[icounter] = item;
                  item->init (scene);
                  item->setZValue (0.9);
                }
                else
                {
                item = ITEMS[icounter];
                item->setLine (QLineF (x1, y1, x2, y2));
                }
          */
          if (Q_LIKELY (icounter < ITEMSsize))
          {
            item = ITEMS[icounter];
            item->setLine (QLineF (x1, y1, x2, y2));
          }
          else
          {
            item = new QTCGraphicsItem (type);
            item->setLine (QLineF (x1, y1, x2, y2));
            ITEMS[icounter] = item;
            item->init (scene);
            item->setZValue (0.9);
          }

          item->setPen (pen);
          item->setVisible (true);
          icounter ++;
        }
    x1 = x2;
    y1 = y2;
  }

  visibleitems = icounter;
  if (icounter > ITEMSsize)
    ITEMSsize = icounter;
}

// draw dot
void
QTACObject::drawDot ()
{
  QPen pen;
  QTCGraphicsItem *item;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  qreal x1, y1;
  int type = EllipseItemType, icounter = 0;

  pen.setColor (forecolor);
  pen.setWidth (thickness);
  clearITEMS ();

  if (Y.size () < 2)
    return;

  if (Y.at (0) == 0)
    return;

  for (qint32 counter = 0, maxcounter = Y.size ();
       counter < maxcounter; counter ++)
  {
    x1 = X.at (counter);
    y1 = Y.at (counter);
    if (y1 == 0)
      break;

    if (x1 > core->chartleftmost && x1 < core->chartrightmost)
      if (y1 > core->charttopmost && y1 < core->chartbottomost)
      {
        if (Q_UNLIKELY (icounter >= ITEMSsize))
        {
          item = new QTCGraphicsItem (type);
          item->setRect (QRectF (x1, y1, 3, 3));
          if (item->brush ().style () != Qt::SolidPattern)
            item->setBrush (QBrush(Qt::SolidPattern));
          ITEMS[icounter] = item;
          item->init (scene);
          item->setZValue (0.9);
        }
        else
        {
          item = ITEMS[icounter];
          item->setRect (QRectF (x1, y1, 3, 3));
          if (item->brush ().style () != Qt::SolidPattern)
            item->setBrush (QBrush(Qt::SolidPattern));
        }
        item->setPen (pen);
        item->setVisible (true);
        icounter ++;
      }
  }

  visibleitems = icounter;
  if (icounter > ITEMSsize)
    ITEMSsize = icounter;
}

// draw trend line
void
QTACObject::drawTLine ()
{
  QRectF rectf;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  LineEdge *edge;
  qreal x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  int found = -1;

  if (hvline->isVisible ())
  {
    hvline->setVisible (false);
    Edge[0]->pricetxt->setVisible (false);
    Edge[1]->pricetxt->setVisible (false);
  }

  if (Q_UNLIKELY(deleteit))
    return;

  if (Edge[0]->trailerCandleText.size () < 4)
    found = 0;
  else
    for (qint32 counter = 0; counter < core->nbars_on_chart; counter ++)
    {
      if ( counter + *core->startbar < core->HLOC->size ())
        if (Edge[0]->trailerCandleText == (*core->HLOC).at (counter + *core->startbar).Text)
          found = counter;
    }

  if (found == -1)
    return;

  y1 = core->yOnPrice (Edge[0]->price);
  if (Edge[0]->trailerCandleText.size () > 3)
  {
    x1 =  core->chartwidth - (core->framewidth * 1.5 * (found + 1)) + 1;
    x1 += (core->framewidth * 0.8) / 2;
    x1 += core->chartleftmost;
  }
  else
  {
    if (Edge[0]->txtdirection == 1)
    {
      x1 = core->chartrightmost - *core->excess_drag_width;
      x1 += (Edge[0]->pad * (core->framewidth * 1.5));
    }
    else
    {
      x1 = core->HLOC->size () - *core->startbar;
      x1 = core->framewidth * 1.5 * x1;
      x1 = x1 + *core->excess_drag_width;
      x1 = core->chartrightmost - x1;
      x1 -= (Edge[0]->pad * (core->framewidth * 1.5));
    }
  }

  found = -1;
  if (Edge[1]->trailerCandleText.size () < 4)
    found = 0;
  else
    for (qint32 counter = 0; counter < core->nbars_on_chart; counter ++)
    {
      if ( counter + *core->startbar < core->HLOC->size ())
        if (Edge[1]->trailerCandleText == (*core->HLOC).at (counter + *core->startbar).Text)
          found = counter;
    }

  if (found == -1)
    return;

  y2 = core->yOnPrice (Edge[1]->price);
  if (Edge[1]->trailerCandleText.size () > 3)
  {
    x2 =  core->chartwidth - (core->framewidth * 1.5 * (found + 1)) + 1;
    x2 += (core->framewidth * 0.8) / 2;
    x2 += core->chartleftmost;
  }
  else
  {
    if (Edge[1]->txtdirection == 1)
    {
      x2 = core->chartrightmost - *core->excess_drag_width;
      x2 += (Edge[1]->pad * (core->framewidth * 1.5));
    }
    else
    {
      x2 = core->HLOC->size () - *core->startbar;
      x2 = core->framewidth * 1.5 * x2;
      x2 =  x1 + *core->excess_drag_width;
      x2 =  core->chartrightmost - x2;
      x2 -= (Edge[1]->pad * (core->framewidth * 1.5));
    }
  }

  if (x1 < core->chartleftmost)
    return;
  if (x1 > core->chartrightmost)
    return;
  if (y1 < core->charttopmost)
    return;
  if (y1 > core->chartbottomost)
    return;

  if (x2 < core->chartleftmost)
    return;
  if (x2 > core->chartrightmost)
    return;
  if (y2 < core->charttopmost)
    return;
  if (y2 > core->chartbottomost)
    return;

  hvline->setLine (x1, y1, x2, y2);
  hvline->setVisible (true);
  foreach (edge, Edge)
  {
    qreal x = 0, y = 0;
    rectf = edge->pricetxt->boundingRect();

    if (edge->sequence == 1)
    {
      x = x1;
      y = y1;
    }
    else if (edge->sequence == 2)
    {
      x = x2;
      y = y2;
    }

    if (edge->txtdirection == 0)
      x -=  (rectf.width () + 2);
    else
      x += 2;

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

// draw fibo line
void
QTACObject::drawFibo ()
{
  QRectF rectf;
  QLineF line;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  LineEdge *edge;
  qreal x1 = 0, x2 = 0, y1 = 0, y2 = 0;

  if (hvline->isVisible ())
  {
    hvline->setVisible (false);
    Edge[0]->pricetxt->setVisible (false);
    Edge[1]->pricetxt->setVisible (false);
    for (qint32 counter = 0; counter < FiboLevelPrc.size (); counter ++)
    {
      FiboLevel[counter]->setVisible (false);
      FiboLevelLbl[counter].setVisible (false);
      FiboLevelPrcLbl[counter].setVisible (false);
    }
  }

  if (deleteit)
    return;

  line = hvline->line ();

  y1 = core->yOnPrice (Edge[0]->price);
  x1 = line.x1 ();

  y2 = core->yOnPrice (Edge[1]->price);
  x2 = x1;

  if (x1 < core->chartleftmost)
    return;
  if (x1 > (core->chartrightmost - 75))
    return;
  if (y1 < core->charttopmost)
    return;
  if (y1 > core->chartbottomost)
    return;

  if (x2 < core->chartleftmost)
    return;
  if (x2 > (core->chartrightmost - 75))
    return;
  if (y2 < core->charttopmost)
    return;
  if (y2 > core->chartbottomost)
    return;

  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
  hvline->setLine (x1, y1, x2, y2);
  hvline->setVisible (true);
  foreach (edge, Edge)
  {
    qreal x = 0, y = 0;
    rectf = edge->pricetxt->boundingRect();

    if (edge->sequence == 1)
    {
      x = x1;
      y = y1;
    }
    else if (edge->sequence == 2)
    {
      x = x2;
      y = y2;
    }

    if (edge->txtdirection == 0)
      x -=  (rectf.width () + 2);
    else
      x += 2;

    edge->pricetxt->setPos ( x, y - 12);
    edge->pricetxt->setVisible (true);
  }

  FiboLevelPrice.clear ();
  for (qint32 counter = 0; counter < FiboLevelPrc.size (); counter ++)
  {

    // QString prcstr;
    qreal price, x;
    price = qAbs (Edge[0]->price - Edge[1]->price);
    price *= FiboLevelPrc[counter];
    if (Edge[0]->price < Edge[1]->price)
      price += Edge[0]->price;
    else
      price += Edge[1]->price;
    FiboLevelPrice += price;
    // prcstr = QString::number (price, 'f', 2);

    // FiboLevelPrcLbl[counter].setPlainText (prcstr);
    rectf = FiboLevelPrcLbl[counter].boundingRect();
    x =  core->chartrightmost - (rectf.width () + 2);
    (static_cast <QGraphicsLineItem *> (FiboLevel[counter]))->setPen (hvline->pen ());
    (static_cast <QGraphicsLineItem *> (FiboLevel[counter]))->setLine (line.x1 (),
        core->yOnPrice (price), x,
        core->yOnPrice (price));
    FiboLevelPrcLbl[counter].setPos (x + 1, core->yOnPrice (price) - 12);
    FiboLevelPrcLbl[counter].setDefaultTextColor (hvline->pen().color ());
    FiboLevelLbl[counter].setVisible (true);
    FiboLevelPrcLbl[counter].setVisible (true);
    (static_cast <QGraphicsLineItem *> (FiboLevel[counter]))->setVisible (true);

    if (counter > 0 && counter < FiboLevelPrc.size () - 1)
    {
      FiboLevelLbl[counter].setDefaultTextColor (hvline->pen().color ());
      rectf = FiboLevelLbl[counter].boundingRect();
      x =  line.x1 () - (rectf.width () + 2);
      FiboLevelLbl[counter].setPos (x, core->yOnPrice (price) - 12);
      FiboLevelLbl[counter].setVisible (true);
    }
  }
}

// draw horizontal or verical line
void
QTACObject::drawHVLine ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QRectF rectf;
  qreal x, y, top, bottom;

  if (type == QTACHART_OBJ_HLINE)
  {
    if (parentObject == NULL)
    {
      top = core->charttop * core->points;
      bottom = core->chartbottom * core->points;

      if (price >= top || price <= bottom)
      {
        hvline->setVisible (false);
        title->setVisible (false);
      }
      else
      {
        hvline->setVisible (true);
        title->setVisible (true);
      }

      y = core->yOnPrice (price);
      rectf = title->boundingRect();
      hvline->setLine (core->chartleftmost, y, core->chartrightmost - rectf.width (), y);
      title->setPos (core->chartrightmost - rectf.width (), y - 10);
      return;
    }

    y = subYonPrice (price);
    if (y < basey || y > basey + height)
    {
      hvline->setVisible (false);
      title->setVisible (false);
      return;
    }

    hvline->setVisible (true);
    title->setVisible (true);
    hvline->setLine (core->chartleftmost, y, core->chartrightmost, y);
    title->setPos (core->ruller_cursor_x, y - 10);
    return;
  }

  if (type == QTACHART_OBJ_VLINE)
  {
    int found = -1;

    hvline->setVisible (false);
    title->setVisible (false);
    if (deleteit)
      return;

    for (qint32 counter = 0; counter < core->nbars_on_chart; counter ++)
    {
      if ( counter + *core->startbar < core->HLOC->size ())
        if (trailerCandleText == (*core->HLOC).at (counter + *core->startbar).Text)
          found = counter;
    }

    if (found == -1)
      return;

    x =  core->chartwidth - (core->framewidth * 1.5 * (found + 1)) + 1;
    x += (core->framewidth * 0.8) / 2;
    x += core->chartleftmost + 2;

    if (x + (title->boundingRect().width () / 2) > core->chartrightmost)
      return;

    if (x - (title->boundingRect().width () / 2) < core->chartleftmost)
      return;

    rectf = title->boundingRect();
    hvline->setLine (x, core->charttopmost, x, core->chartbottomost - rectf.height () + 3);
    title->setPos (x - (rectf.width () / 2), core->chartbottomost - 12);
    hvline->setVisible (true);
    title->setVisible (true);
  }
}
