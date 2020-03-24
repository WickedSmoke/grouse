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
#include "common.h"

// symbol list poppulate
static void
populate_table (DataManagerDialog *dialog, char *str, int col)
{
  switch (col)
  {
  case 0:
    dialog->symbolList << QString::fromUtf8 (str);
    break;

  case 1:
    dialog->descList << QString::fromUtf8 (str);
    break;

  case 2:
    dialog->marketList << QString::fromUtf8 (str);
    break;

  case 3:
    dialog->sourceList << QString::fromUtf8 (str);
    break;

  case 4:
    dialog->timeframeList << QString::fromUtf8 (str);
    break;

  case 5:
    dialog->datefromList << QString::fromUtf8 (str);
    break;

  case 6:
    dialog->datetoList << QString::fromUtf8 (str);
    break;

  case 7:
    dialog->currencyList << QString::fromUtf8 (str);
    break;

  case 8:
    dialog->keyList << QString::fromUtf8 (str);
    break;

  case 9:
    dialog->adjustedList << QString::fromUtf8 (str);
    break;

  case 10:
    dialog->baseList << QString::fromUtf8 (str);
    break;

  case 11:
    dialog->pathList << QString::fromUtf8 (str);
    break;

  case 12:
    dialog->formatList << QString::fromUtf8 (str);
    break;

  case 13:
    dialog->lastupdateList << QString::fromUtf8 (str);
    break;
  }
}
#endif

// sqlite3_exec callback for retrieving symbol table
int
sqlcb_symbol_table(void *classptr, int argc, char **argv, char **column)
{
#ifdef CGTOOL
  SymbolVector *vec = static_cast <SymbolVector *> (classptr);
  SymbolRec    *srec = new SymbolRec;
#else
  DataManagerDialog *dialog = static_cast <DataManagerDialog *> (classptr);
#endif

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
#ifdef CGTOOL
   if (colname == QLatin1String ("SYMBOL"))
     srec->symbol = QString (argv[counter]);
   else
   if (colname == QLatin1String ("DESCRIPTION"))
     srec->name = QString (argv[counter]);
   else
   if (colname == QLatin1String ("SOURCE"))
     srec->feed = QString (argv[counter]);
   else
   if (colname == QLatin1String ("KEY2"))
     srec->key = QString (argv[counter]);
#else
    if (colname == QLatin1String ("SYMBOL"))
      populate_table (dialog, argv[counter], 0);
    else
    if (colname == QLatin1String ("DESCRIPTION"))
      populate_table (dialog, argv[counter], 1);
    else
    if (colname == QLatin1String ("MARKET"))
      populate_table (dialog, argv[counter], 2);
    else
    if (colname == QLatin1String ("SOURCE"))
      populate_table (dialog, argv[counter], 3);
    else
    if (colname == QLatin1String ("TIMEFRAME"))
      populate_table (dialog, argv[counter], 4);
    else
    if (colname == QLatin1String ("DATEFROM"))
      populate_table (dialog, argv[counter], 5);
    else
    if (colname == QLatin1String ("DATETO"))
      populate_table (dialog, argv[counter], 6);
    else
    if (colname == QLatin1String ("CURRENCY"))
      populate_table (dialog, argv[counter], 7);
    else
    if (colname == QLatin1String ("KEY"))
      populate_table (dialog, argv[counter], 8);
    else
    if (colname == QLatin1String ("ADJUSTED"))
      populate_table (dialog, argv[counter], 9);
    else
    if (colname == QLatin1String ("BASE"))
      populate_table (dialog, argv[counter], 10);
    else
    if (colname == QLatin1String ("DNLSTRING"))
      populate_table (dialog, argv[counter], 11);
    else
    if (colname == QLatin1String ("FORMAT"))
      populate_table (dialog, argv[counter], 12);
    else
    if (colname == QLatin1String ("LASTUPDATE"))
      populate_table (dialog, argv[counter], 13);
#endif
  }

#ifdef CGTOOL
  vec->append (srec);
#endif
  return 0;
}
