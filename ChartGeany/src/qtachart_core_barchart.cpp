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

// draw a candle chart
void
QTAChartCore::drawBarChart (void)
{
  QTAChartBarData bardata;
  QTAChartFrame frame;
  int counter = 0;

  bardata.width = framewidth * 0.2;
  for (qint32 k = *startbar, i = 0,
       maxk = ((*startbar) + nbars_on_chart),
       hlocsize = HLOC->size ();
       k < maxk; k++, i++)
  {
    if (k < hlocsize)
    {
      frame = HLOC->at (k);
      bardata.x = chartwidth - (framewidth * 1.5 * (i + 1)) + 1;
      bardata.x += (framewidth - bardata.width) / 2;
      bardata.x += chartleftmost;
      bardata.height =
        qAbs (yOnPrice (frame.High) - yOnPrice (frame.Low));
      if (bardata.height <= 0)
        bardata.height = 1;
      bardata.y = yOnPrice (frame.High);
      bardata.uporopen = yOnPrice (frame.Open);
      bardata.downorclose = yOnPrice (frame.Close);
      bardata.HLOC = &HLOC->at (k);
      drawBar (&bardata, counter);
    }
  }

  visibleitems = counter;
  if (counter > ITEMSsize)
    ITEMSsize = counter;
}
