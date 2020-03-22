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

#include <QFileInfo>
#include <QFile>

#include "simplecrypt.h"
#include "cgtool.h"

int
import_module (const QString &cgmfile, const QString &dbfile)
{
  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return 1;

  // check if module file exists
  if (!checkFileExistence (cgmfile))
    return 1;

  // open sqlite file
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2(dbfile.toUtf8 (), &db, SQLITE_OPEN_READWRITE, nullptr);

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

  QFile modscript (cgmfile);
  if (modscript.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    SimpleCrypt crypto(ENCKEY);
    QString encstr;
    QTextStream in (&modscript);
    encstr = in.readLine (0);
    QString SQL = crypto.decryptToString (encstr);
    modscript.close ();
    int rc = updatedb (db, SQL);
    if (rc != SQLITE_OK)
    {
      err << "error: module import failed" << endl;
      return 1;
    }
  }
  
  sqlite3_close (db);
  
  return 0;
}
