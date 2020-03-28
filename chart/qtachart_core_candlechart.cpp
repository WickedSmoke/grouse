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
QTAChartCore::drawCandleChart (void)
{
  FrameVector *hloc;
  QTAChartBarData bardata;
  QTAChartFrame frame;
  qint32 counter = 0;

  if (chart_style == QTACHART_CANDLE)
    hloc = HLOC;
  else
    hloc = HEIKINASHI;

  bardata.width = framewidth;
  bardata.fcolor = framecolor;

  for (qint32 k = *startbar, i = 0,
       maxk = ((*startbar) + nbars_on_chart),
       hlocsize = hloc->size ();
       k < maxk; k++, i++)
  {
    if (k < hlocsize)
    {
      frame = hloc->at (k);
      qreal yOpen = yOnPrice (frame.Open),
            yClose = yOnPrice (frame.Close),
            yHigh = yOnPrice (frame.High),
            yLow = 	yOnPrice (frame.Low);

      bardata.x = chartwidth - (bardata.width * 1.5 * (i + 1)) + 1;
      bardata.x += chartleftmost;
      bardata.height = qAbs (yOpen - yClose);
      if (bardata.height == 0)
        bardata.height = 1;

      if (frame.Open >= frame.Close)
      {
        bardata.uporopen = yOpen - yHigh;
        bardata.downorclose = yLow - yClose;
        bardata.y = yOpen;
        bardata.trend = QTACHART_CANDLE_DOWN;
      }
      else
      {
        bardata.downorclose = yLow - yOpen;
        bardata.uporopen = yClose - yHigh;
        bardata.y = yClose;
        bardata.trend = QTACHART_CANDLE_UP;
      }

      bardata.HLOC = &hloc->at (k);
      drawCandle (&bardata, counter);
    }
  }

  visibleitems = counter;
  if (counter > ITEMSsize)
    ITEMSsize = counter;
}

