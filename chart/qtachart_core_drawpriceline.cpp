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

//draw the price line
void
QTAChartCore::drawPriceLine (QColor forecolor, qint32 thickness)
{

  QTCGraphicsItem *item;
  QPen pen;
  qreal x1, x2, y1, y2;
  int type = LineItemType, counter = 0;

  pen.setColor (forecolor);
  pen.setWidth (thickness);
  x1 = chartwidth - (framewidth * 1.5) + 1;
  x1 += framewidth / 2;
  x1 += chartleftmost;
  y1 = yOnPrice (CLOSE[*startbar]);

  for (qint32 k = (*startbar) + 1, i = 1,
       maxk = ((*startbar) + nbars_on_chart),
       hlocsize = HLOC->size ();
       k < maxk; k++, i++)
  {
    if (k < hlocsize)
    {
      x2 = chartwidth - (framewidth * 1.5 * (i + 1)) + 1;
      x2 += framewidth / 2;
      x2 += chartleftmost;
      y2 = yOnPrice (CLOSE.at (k));

      if (counter >= ITEMSsize)
      {
        item = new QTCGraphicsItem (type);
        item->setZValue (0.1);
        ITEMS[counter] = item;
        item->init (scene);
      }
      else
      {
        item = ITEMS[counter];
      }

      item->setLine (QLineF (x1, y1, x2, y2));
      item->setPen (pen);
      item->setVisible (true);
      counter ++;
      x1 = x2;
      y1 = y2;
    }
  }
  visibleitems = counter;
  if (counter > ITEMSsize)
    ITEMSsize = counter;
}
