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

#include <QCryptographicHash>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QProcess>

#include "simplecrypt.h"
#include "errors.h"
#include "cgtool.h"

static ToolchainRec toolchain;

#include "../ChartGeany/src/compile_static.cpp"

// export and module extensions
#ifdef Q_OS_WIN
#define SOEXT       QString (".dll")
#else
#define SOEXT       QString (".so")
#endif

// compile a module
int
compile_module (const QString &cgsfile, const QString &dbfile)
{
  Q_UNUSED (createModule);

  int result = 0;

  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return 1;

  // check if source file exists
  if (!checkFileExistence (cgsfile))
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

  // select from toolchains table
  QString SQL =
    QStringLiteral ("SELECT * FROM toolchains WHERE platform = '") %
                    platformString () % QStringLiteral ("'");

  rc = selectfromdb (db, SQL.toUtf8(), sqlcb_toolchain, static_cast <void *> (&toolchain));
  if (rc != SQLITE_OK)
  {
    err << "error: " << sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }
  sqlite3_close (db);

  QFile sourcefile;

  sourcefile.setFileName (cgsfile);
  if (!sourcefile.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    err << "error: cannot open source file" << endl;
    return 1;
  }

  QTextStream readsourcefile (&sourcefile);
  QString source;
  source = readsourcefile.readAll();
  sourcefile.close ();

  QFileInfo fi(cgsfile);
  QString compiletmpfname = tempSourcePath (),
          programname = fi.baseName ();

  // create the final script with top.h and bottom.h prepended and
  // appended respectively
  result = createFinalScript (source, programname, compiletmpfname);
  if (result != CG_ERR_OK)
    return result;

  QString compileoutputfname = compiletmpfname + SOEXT,
          pstring = platformString (), output;

  bool native = (toolchain.compiler.trimmed () == "");

  if (native) // native compile
  {
    result = nativeCompile (compiletmpfname, compileoutputfname,
                            pstring, output, true);
  }
  else      // external compiler
  {
    result = externalCompile (compiletmpfname, compileoutputfname,
                              NULL, false);
  }

  if (result == CG_ERR_NO_COMPILER)
  {
    err << endl << "compiler not found" << endl;
    return 1;
  }

  if (result != CG_ERR_OK || output.size () > 0)
  {
    err << endl << "compilation failed" << endl;
    err << output;
    return 1;
  }

  // create module's sql script
  QString modsource, modid, modauthor, modversion,
          modtype, modname, modplatform;
  modsource = source.replace ("'", "`");
  modsource.replace (";", ";" + QChar ('\n'));
  QStringList lines = modsource.split("\n");
  modid = QStringLiteral ("");
  foreach (QString line, lines)
  {
    if (line.contains ("Property ") && line.contains ("MODID"))
      modid = line.split("\"").at (1);
    if (line.contains ("Property ") && line.contains ("MODAUTHOR"))
      modauthor = line.split("\"").at (1);
    if (line.contains ("Property ") && line.contains ("MODVERSION"))
      modversion = line.split("\"").at (1);
    if (line.contains ("SetObjectType") && line.contains ("_OBJECT"))
    {
      modtype = line.split("(").at (1).split (")").at (0).trimmed ();
      modtype.replace ("_OBJECT", "");
    }
  }

  modname = programname;
  modplatform = pstring;

  // delete the previous version if exists
  SQL = QStringLiteral ("DELETE FROM modules WHERE id = '") % modid % QStringLiteral ("';");

  // insert the new version
  SQL += QStringLiteral ("INSERT INTO modules (id, name, status, source, binary, platform, author, version, type) \
     VALUES ('") % modid % QStringLiteral ("','") %
     modname % QStringLiteral ("','") %
     QStringLiteral ("DISABLED") % QStringLiteral ("','") %
     modsource % QStringLiteral ("','") %
     QStringLiteral ("','") %
     modplatform % QStringLiteral ("','") %
     modauthor % QStringLiteral ("','") %
     modversion % QStringLiteral ("','") %
     modtype % QStringLiteral ("');");

  // export cgm
  SimpleCrypt crypto(ENCKEY);
  modname += ".cgm";
  QFile modscript (modname);
  if (!modscript.open (QIODevice::WriteOnly | QIODevice::Text))
  {
    err << "error: cannot create cgm file" << endl;
    return 1;
  }
  QString encstr = crypto.encryptToString (SQL);
  modscript.write(encstr.toUtf8 ());
  modscript.close();

  return 0;
}
