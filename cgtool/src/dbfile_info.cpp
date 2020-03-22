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

#include <QStringList>

#include "top.h"
#include "cgtool.h"

int
dbfile_info (const QString &dbfile)
{
  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
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

  // check dbfile version
  int dbversion;
  QString SQL = QStringLiteral ("SELECT * FROM VERSION;");
  rc = sqlite3_exec(db, SQL.toUtf8(), sqlcb_dbversion, (void *) &dbversion, NULL);
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  out << "DATABASE FILE VERSION: " << dbversion << endl << endl;

  QStringList feedlist;
  SQL = QStringLiteral ("SELECT * FROM DATAFEEDS ORDER BY FEEDNAME;");

  rc = sqlite3_exec(db, SQL.toUtf8(), sqlcb_datafeeds,
                    static_cast <void *> (&feedlist), NULL);
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  out << "SUPPORTED DATAFEEDS:" << endl;
  foreach (const QString feed, feedlist)
    out << feed << endl;
  out << endl;

  ToolchainVector tvec;
  SQL = QStringLiteral ("SELECT * FROM TOOLCHAINS ORDER BY PLATFORM;");

  rc = sqlite3_exec(db, SQL.toUtf8(), sqlcb_toolchains,
                    static_cast <void *> (&tvec), NULL);
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  out << "SUPPORTED CGSCRIPT TOOLCHAINS:" << endl;
  foreach (const ToolchainRec *rec, tvec)
  {
    if (platformString () == rec->platform)
      out << "* ";
    out << rec->platform << ": " << rec->compiler << endl;
  }

  out << endl << "CGScript version: " << _CGSCRIPT_VERSION_STR << endl;

  sqlite3_close (db);
  return 0;
}
