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

// ChartWidth
extern "C" Q_DECL_EXPORT int
ChartWidth_imp (const void *ptr) NOEXCEPT
{
  Q_UNUSED (QTACastFromConstVoid)

  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <int> (qAbs (core->chartrightmost - core->chartleftmost));
}

// ChartHeight
extern "C" Q_DECL_EXPORT int
ChartHeight_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <int> (qAbs (core->chartbottomost - core->charttopmost));
}

// ChartForeColor
extern "C" Q_DECL_EXPORT Color_t
ChartForeColor_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <Color_t> (core->forecolor.rgb ());
}

// ChartBackColor
extern "C" Q_DECL_EXPORT Color_t
ChartBackColor_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return static_cast <Color_t> (core->backcolor.rgb ());
}

// ChartCurrentTF
extern "C" Q_DECL_EXPORT TimeFrame_t
ChartCurrentTF_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  if (core->currenttf == QLatin1String ("WEEK")) return TF_WEEK;
  if (core->currenttf == QLatin1String ("MONTH")) return TF_MONTH;
  if (core->currenttf == QLatin1String ("YEAR")) return TF_YEAR;
  return TF_DAY;
}

// ChartCurrentBar
extern "C" Q_DECL_EXPORT int
ChartCurrentBar_imp (const void *ptr) NOEXCEPT
{
  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);

  return core->currentbar;
}
