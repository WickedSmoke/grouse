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

#include <QString>

#ifdef CGTOOL
#include <QStringList>
#include "cgtool.h"
#else
#include "common.h"
#endif

// datafeeds' callback
int
sqlcb_datafeeds (void *dummy, int argc, char **argv, char **column)
{
#ifdef CGTOOL
  QStringList *datafeedsList = static_cast <QStringList *> (dummy);
#else
  if (dummy != NULL)
    return 1;
#endif

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();
    if (colname == QLatin1String ("FEEDNAME"))
#ifdef CGTOOL
      *datafeedsList << QString (argv[counter]);
#else
      ComboItems->datafeedsList << QString (argv[counter]);
#endif

#ifndef CGTOOL
    if (colname == QLatin1String ("REALTIME"))
      ComboItems->realtimeList << QString (argv[counter]);
    if (colname == QLatin1String ("SYMLIST"))
      ComboItems->symlistList << QString (argv[counter]);
    if (colname == QLatin1String ("SYMLISTURL"))
      ComboItems->symlisturlList << QString (argv[counter]);
#endif
  }
  return 0;
}