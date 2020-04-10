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
#include "idb.h"
#endif

#ifdef _WIN32
#define stringEqualI(A,B)  (_stricmp(A,B) == 0)
#else
#define stringEqualI(A,B)  (strcasecmp(A,B) == 0)
#endif

// sqlite3_exec callback for retrieving symbol table
int
sqlcb_symbol_table(void *classptr, int argc, char **argv, char **column)
{
#ifdef CGTOOL
  SymbolVector *vec = static_cast <SymbolVector *> (classptr);
  SymbolRec    *srec = new SymbolRec;
#else
  SymbolSummary *summ = static_cast <SymbolSummary *> (classptr);
  QStringList* list;
#endif

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    const char* colname = column[counter];
#ifdef CGTOOL
   if (stringEqualI(colname, "SYMBOL"))
     srec->symbol = QString (argv[counter]);
   else if (stringEqualI(colname, "DESCRIPTION"))
     srec->name = QString (argv[counter]);
   else if (stringEqualI(colname, "SOURCE"))
     srec->feed = QString (argv[counter]);
   else if (stringEqualI(colname, "KEY2"))
     srec->key = QString (argv[counter]);
#else
    if (stringEqualI(colname, "SYMBOL"))
      list = &summ->symbolList;
    else if (stringEqualI(colname, "DESCRIPTION"))
      list = &summ->descList;
    else if (stringEqualI(colname, "MARKET"))
      list = &summ->marketList;
    else if (stringEqualI(colname, "SOURCE"))
      list = &summ->sourceList;
    else if (stringEqualI(colname, "TIMEFRAME"))
      list = &summ->timeframeList;
    else if (stringEqualI(colname, "DATEFROM"))
      list = &summ->datefromList;
    else if (stringEqualI(colname, "DATETO"))
      list = &summ->datetoList;
    else if (stringEqualI(colname, "CURRENCY"))
      list = &summ->currencyList;
    else if (stringEqualI(colname, "KEY"))
      list = &summ->keyList;
    else if (stringEqualI(colname, "ADJUSTED"))
      list = &summ->adjustedList;
    else if (stringEqualI(colname, "BASE"))
      list = &summ->baseList;
    else if (stringEqualI(colname, "DNLSTRING"))
      list = &summ->pathList;
    else if (stringEqualI(colname, "FORMAT"))
      list = &summ->formatList;
    else if (stringEqualI(colname, "LASTUPDATE"))
      list = &summ->lastupdateList;
    else
      list = nullptr;

    if (list)
      list->append(QString::fromUtf8(argv[counter]));
#endif
  }

#ifdef CGTOOL
  vec->append (srec);
#endif
  return 0;
}
