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

#ifdef CGTOOL
#include <QLatin1String>
#include "cgtool.h"
#else
#include <QTableWidget>
#include <QTableWidgetItem>
#endif

#include <QString>

// sqlite3_exec callback for retrieving modules
int
sqlcb_modules (void *classptr, int argc, char **argv, char **column)
{
#ifdef CGTOOL
  ModuleVector *vec = static_cast <ModuleVector *> (classptr);
  ModuleRec    *mrec = new ModuleRec;
#else
  QTableWidget *tw = static_cast <QTableWidget *> (classptr);
  int rcount = 0;
  while (tw->item (rcount, 1)->text () != "Empty")
    rcount ++;

#endif
  QString colname;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString (column[counter]).toUpper ();
#ifdef CGTOOL

#else
    QTableWidgetItem *item;
#endif

    if (colname == QLatin1String ("ID"))
    {
#ifdef CGTOOL
      mrec->id = QString (argv[counter]);
#else
      item = tw->item (rcount, 7);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("NAME"))
    {
#ifdef CGTOOL
      mrec->name = QString (argv[counter]);
#else
      item = tw->item (rcount, 0);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("SOURCE"))
    {
#ifdef CGTOOL
      mrec->source = QString (argv[counter]);
#else
      item = tw->item (rcount, 1);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("BINARY"))
    {
#ifdef CGTOOL
      mrec->binary = QString (argv[counter]);
#else
      item = tw->item (rcount, 2);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("TYPE"))
    {
#ifdef CGTOOL
      mrec->type = QString (argv[counter]);
#else
      item = tw->item (rcount, 3);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("AUTHOR"))
    {
#ifdef CGTOOL
      mrec->author = QString (argv[counter]);
#else
      item = tw->item (rcount, 4);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("VERSION"))
    {
#ifdef CGTOOL
      mrec->version = QString (argv[counter]);
#else
      item = tw->item (rcount, 5);
      item->setText (QString (argv[counter]));
#endif
    }

    if (colname == QLatin1String ("STATUS"))
    {
#ifdef CGTOOL
      mrec->status = QString (argv[counter]);
#else
      item = tw->item (rcount, 6);
      item->setText (QString (argv[counter]));
#endif
    }
  }

#ifdef CGTOOL
  vec->append (mrec);
#endif

  return 0;
}
