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

#include "cgtool.h"

// list symbols
int
list_symbols (const QString &dbfile)
{
  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return 1;

  // check dbfile version
  if (!checkDBFileVersion (dbfile))
    return 1;

  // open sqlite file
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2(dbfile.toUtf8 (), &db, SQLITE_OPEN_READONLY, nullptr);

  if (db == nullptr)
  {
     err << "error: " << "unable to allocate memory" << endl;
     return 1;
  }

  if (rc != SQLITE_OK)
  {
    err << "error: " << sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  // select from symbol table
  QString SQL =
    QStringLiteral ("SELECT * FROM symbols ORDER BY SYMBOL, \
                     SOURCE, ADJUSTED, TIMEFRAME;");

  SymbolVector symbolVector;

  rc = selectfromdb(db, SQL.toUtf8(), sqlcb_symbol_table, static_cast <void *> (&symbolVector));
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  // print modules to stdout
  QString sym;
  sym.sprintf ("%-40s %-15s %-15s %s",
                "TABLEID",
                "SYMBOL",
                "FEED",
                "NAME");
  out << sym << endl;

  foreach (const SymbolRec *rec, symbolVector)
  {

    sym.sprintf ("%-40s %-15s %-15s %s",
                rec->key.toUtf8 ().data (),
                rec->symbol.toUtf8().data (),
                rec->feed.toUtf8 ().data (),
                rec->name.toUtf8 ().data ());
    out << sym << endl;
  }

  sqlite3_close (db);

  return 0;
}
