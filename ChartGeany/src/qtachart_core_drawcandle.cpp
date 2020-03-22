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

#define Y_PAD 	0

// draw a candle on the chart
void
QTAChartCore::drawCandle (QTAChartBarData * bardata, qint32 &counter)
{

  QTCGraphicsItem *item;
  QColor color;
  qreal shadow_x, shadow_y1, shadow_y2;
  int type = RectItemType;

  // calculate
  shadow_x = bardata->x + (bardata->width - SHADOW_WIDTH) / 2;
  shadow_y1 = bardata->y - bardata->uporopen;
  shadow_y2 = bardata->uporopen + bardata->downorclose + bardata->height;

  // set candle colors
  color = Qt::red;
  if (bardata->trend == QTACHART_CANDLE_UP)
    color = Qt::green;
    
  QBrush brush (color);  
  QPen pen (bardata->fcolor);

  if (ITEMSsize - counter > 1)
  {
    // candle body
    item = ITEMS[counter];
    item->setRect (bardata->x, bardata->y + Y_PAD,
                   bardata->width, bardata->height);
    item->setPen (pen);
    item->setBrush (brush);
    item->setVisible (true);
    counter ++;

    // up and down
    item = ITEMS[counter];
    item->setRect (shadow_x, shadow_y1 + Y_PAD, SHADOW_WIDTH, shadow_y2);
    item->setPen (pen);
    item->setBrush (brush);
    item->setVisible (true);
    counter ++;

    return;
  }

  // candle body
  if (counter >= ITEMSsize)
  {
    item = new QTCGraphicsItem (type);
    item->setZValue (0.1);
    item->init (scene);
    ITEMS[counter] = item;
  }
  else
  {
    item = ITEMS[counter];
  }

  item->setRect (bardata->x, bardata->y + Y_PAD,
                 bardata->width, bardata->height);
  item->setBrush (brush);
  item->setPen (pen);
  item->setVisible (true);
  counter ++;

  // up and down
  if (counter >= ITEMSsize)
  {
    item = new QTCGraphicsItem (type);
    item->setZValue (0.0);
    item->init (scene);
    ITEMS[counter] = item;
  }
  else
  {
    item = ITEMS[counter];
  }

  item->setRect (shadow_x, shadow_y1 + Y_PAD,
                 SHADOW_WIDTH, shadow_y2);
  item->setPen (pen);
  item->setBrush (brush);
  item->setVisible (true);
  counter ++;
}
