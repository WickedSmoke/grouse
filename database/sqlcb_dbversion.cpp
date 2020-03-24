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

#include <QtGlobal> 
#include <QString>

#ifdef CGTOOL
#include <QLatin1String>
#include "cgtool.h"
#endif

int
sqlcb_dbversion (void *versionptr, int argc, char **argv, char **column)
{
  QString colname;
  int version = -1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("VERSION"))
      version = QString::fromUtf8 (argv[counter]).toInt ();
  }
  *(int *) versionptr = version;
  return 0;
}
