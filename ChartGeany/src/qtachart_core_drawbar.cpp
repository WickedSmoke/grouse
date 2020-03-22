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

// draw a bar on the chart
void
QTAChartCore::drawBar (QTAChartBarData * bardata, qint32 &counter)
{

  QTCGraphicsItem *item;
  QPen pen;
  int type = LineItemType;

  pen.setColor (barcolor);
  pen.setWidth (bardata->width);

  // bar body
  if (counter >= ITEMSsize)
  {
    item = new QTCGraphicsItem (type);
    item->setZValue (0.1);
    item->setLine (QLineF (bardata->x, bardata->y,
                           bardata->x, bardata->y + bardata->height));
    item->init (scene);
    ITEMS[counter] = item;
  }
  else
  {
    item = ITEMS[counter];
    item->setLine (QLineF (bardata->x, bardata->y,
                           bardata->x, bardata->y + bardata->height));
  }

  item->setPen (pen);
  item->setVisible (true);
  counter ++;

  // open
  if (counter >= ITEMSsize)
  {
    item = new QTCGraphicsItem (type);
    item->setZValue (0.1);
    item->setLine (QLineF (bardata->x - 3, bardata->uporopen,
                           bardata->x, bardata->uporopen));
    item->init (scene);
    ITEMS[counter] = item;
  }
  else
  {
    item = ITEMS[counter];
    item->setLine (QLineF (bardata->x - 3, bardata->uporopen,
                           bardata->x, bardata->uporopen));
  }

  item->setPen (pen);
  item->setVisible (true);
  counter ++;

  // close
  if (counter >= ITEMSsize)
  {
    item = new QTCGraphicsItem (type);
    item->setZValue (0.1);
    item->setLine (QLineF (bardata->x + 3, bardata->downorclose,
                           bardata->x, bardata->downorclose));
    item->init (scene);
    ITEMS[counter] = item;
  }
  else
  {
    item = ITEMS[counter];
    item->setLine (QLineF (bardata->x + 3, bardata->downorclose,
                           bardata->x, bardata->downorclose));
  }

  item->setPen (pen);
  item->setVisible (true);
  counter ++;
}
