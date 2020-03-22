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

// module source callback
static int
sqlcb_module_source(void *data, int argc, char **argv, char **column)
{
  Q_UNUSED (argc)
  Q_UNUSED (column)

  QString *source = static_cast <QString *> (data);
  *source = QString (argv[0]);
  return 0;
}

// decompile a module
int
decompile_module (const QString &cgmfile, const QString &dbfile)
{
  int result = 0;

  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return 1;

  // check dbfile version
  if (!checkDBFileVersion (dbfile))
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

  QFileInfo fi(cgmfile);
  QString modname = fi.baseName ();
  QFile modscript (cgmfile);

  // open cgm file
  if (modscript.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    SimpleCrypt crypto(ENCKEY);
    QString encstr;
    QTextStream in (&modscript);
    encstr = in.readLine (0);
    QString SQL = crypto.decryptToString (encstr);
    SQL.replace ("DELETE FROM modules", "DELETE FROM tempmodules");
    SQL.replace ("INSERT INTO modules", "INSERT INTO tempmodules");
    SQL = "DELETE FROM tempmodules;" + SQL;
    modscript.close ();

    rc = sqlite3_exec(db, SQL.toUtf8(), NULL, NULL, NULL);
    if (rc != SQLITE_OK)
    {
      err << "error: " << sqlite3_errmsg (db) << endl;
      sqlite3_close (db);
      return 1;
    }

    QString source;
    SQL = "SELECT source FROM tempmodules WHERE name = '" + modname +"' LIMIT 1;";
    rc = selectfromdb(db, SQL.toUtf8(), sqlcb_module_source,
                    static_cast <void *> (&source));
    if (rc != SQLITE_OK)
    {
      err << "error: "<< sqlite3_errmsg (db) << endl;
      sqlite3_close (db);
      return 1;
    }

    // open the output file
    QFile file (modname + ".cgs");
    if (!file.open (QFile::WriteOnly|QFile::Text))
    {
      err << "error: cannot open output file" << endl;
      return 1;
    }

    QTextStream textstream (&file);
    textstream << source;
    file.close ();

  }
  else
  {
    err << "error: cannot open module file" << endl;
    return 1;
  }
  
  sqlite3_close (db);
  return result;
}
