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

#include <cstring>
#include "cgscript.h"
#include "qtachart_core.h"

// Bar
extern "C" Q_DECL_EXPORT BarData_t
Bar_imp (const void *ptr, Candle_t ct, TimeFrame_t tf, int shift)
{
  QString TF;
  const QTAChartCore *core = static_cast <const QTAChartCore *> (ptr);
  BarData_t rslt;
  QTAChartFrame frame;
  FrameVector fvector;
  int maxshift, counter;

  std::memset (&rslt, 0, sizeof (rslt));
  switch (tf)
  {
    case TF_DAY:
      TF = QStringLiteral ("D");
      break;
    case TF_WEEK:
      TF = QStringLiteral ("W");
      break;
    case TF_MONTH:
      TF = QStringLiteral ("M");
      break;
    case TF_YEAR:
      TF = QStringLiteral ("Y");
      break;
    default:
      return rslt;
  }

  counter = 0;
  while (core->TIMEFRAME.at (counter).TFSymbol != TF) counter ++;
  fvector = core->TIMEFRAME.at (counter).HLOC;
  if (ct == CTYPE_HEIKINASHI)
    fvector = core->TIMEFRAME.at (counter).HEIKINASHI;

  if (fvector.size () == 0)
    return rslt;

  maxshift = fvector.size () - 1;
  if (shift > maxshift)
    shift = maxshift;
  frame = fvector.at (shift);

  rslt.High = frame.High;
  rslt.Low = frame.Low;
  rslt.Open = frame.Open;
  rslt.Close = frame.Close;
  rslt.AdjClose = frame.AdjClose;
  rslt.Volume = frame.Volume;
  rslt.Year = frame.year;
  rslt.Month = frame.month;
  rslt.Day = frame.day;
  rslt.Date[15] = 0;
  memcpy (rslt.Date, frame.Date, 15);
  rslt.Time[15] = 0;
  memcpy (rslt.Time, frame.Time, 15);
  rslt.Id[255] = 0;
  memcpy (rslt.Id, frame.Text.toStdString ().c_str (),255);

  return rslt;
}

// NBars
extern "C" Q_DECL_EXPORT int
NBars_imp (const void *ptr, Candle_t ct, TimeFrame_t tf)
{
  QString TF;
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  switch (tf)
  {
    case TF_DAY:
      TF = QStringLiteral ("D");
      break;
    case TF_WEEK:
      TF = QStringLiteral ("W");
      break;
    case TF_MONTH:
      TF = QStringLiteral ("M");
      break;
    case TF_YEAR:
      TF = QStringLiteral ("Y");
      break;
    default:
      return 0;
  }

  int counter = 0;
  while (core->TIMEFRAME.at (counter).TFSymbol != TF) counter ++;

  if (ct == CTYPE_CANDLE)
    return core->TIMEFRAME.at (counter).HLOC.size ();

  return core->TIMEFRAME.at (counter).HEIKINASHI.size ();
}

// NVisibleBars
extern "C" Q_DECL_EXPORT int
NVisibleBars_imp (const void *ptr)
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);
  FrameVector *hloc;
  int rslt, sb, hls;

  if (core->chart_style == QTACHART_CANDLE)
    hloc = core->HLOC;
  else
    hloc = core->HEIKINASHI;

  sb = static_cast <int> (*core->startbar);
  hls = static_cast <int> (hloc->size ());
  rslt = (core->nbars_on_chart + sb);
  rslt = (rslt < hls ? rslt : hls) - sb;
  return rslt;
}

// NewestVisibleBar
extern "C" Q_DECL_EXPORT int
NewestVisibleBar_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <int> (*core->startbar);
}

// OldestVisibleBar
extern "C" Q_DECL_EXPORT int
OldestVisibleBar_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <int> (*core->startbar + NVisibleBars_imp (ptr) - 1);
}
