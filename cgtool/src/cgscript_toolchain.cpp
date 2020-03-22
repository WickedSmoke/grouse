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

int
cgscript_toolchain (const QString &toolchain, const QString &dbfile)
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

  ToolchainVector tvec;
  QString SQL =
    QStringLiteral ("SELECT * FROM TOOLCHAINS WHERE platform = '") % toolchain % "';";

  rc = sqlite3_exec(db, SQL.toUtf8(), sqlcb_toolchains,
                    static_cast <void *> (&tvec), NULL);
  sqlite3_close (db);

  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    return 1;
  }

  if (tvec.size () == 0)
  {
    err << "toolchain not found" << endl;
    return 1;
  }

  out <<
  "compiler: " << tvec[0]->compiler << endl << endl <<
  "compiler debug flags: " << endl <<
  tvec[0]->compilerdbg.trimmed ().replace ("   ", " ").replace ("  ", " ") << endl << endl <<
  "compiler release flags: " << endl <<
  tvec[0]->compilerrel.trimmed ().replace ("   ", " ").replace ("  ", " ") << endl << endl <<
  "linker debug flags: " << endl <<
  tvec[0]->linkerdbg.replace ("   ", " ").replace ("  ", " ") << endl << endl <<
  "linker release flags: " << endl <<
  tvec[0]->linkerrel.replace ("   ", " ").replace ("  ", " ") << endl;

  return 0;
}
