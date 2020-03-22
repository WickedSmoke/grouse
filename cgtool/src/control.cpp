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

#include <cstring>
#include "cgtool.h"

// minimum supported dbversion
static const int MIN_SUPPORTED_DBVERSION = 24;

// flags
static bool verbose_flag = false,
            tostdout_flag = false;

// help
extern int help ();

// data options
extern int list_symbols (const QString &dbfile);

extern int export_data (const QString &tableid,
                        const QString &dbfile);

// module options
extern int compile_module (const QString &cgsfile,
                           const QString &dbfile);

extern int export_module (const QString &moduleid,
                          const QString &dbfile);

extern int list_modules (const QString &dbfile);

extern int decompile_module (const QString &cgmfile,
                             const QString &dbfile);

extern int import_module (const QString &cgmfile,
                          const QString &dbfile);

extern int delete_module (const QString &moduleid,
                          const QString &dbfile);

// info options
extern int cgscript_toolchain (const QString &toolchain,
                               const QString &dbfile);

extern int dbfile_info (const QString &dbfile);

// return verbose flag
bool
verbose ()
{
  return verbose_flag;
}

// return tostdout flag
bool
tostdout ()
{
  return tostdout_flag;
}

// return true when dbversion is supported
bool
checkDBFileVersion (const QString &dbfile)
{
  // check if sqlite file exists
  if (!checkFileExistence (dbfile))
    return false;

  // open sqlite file
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2(dbfile.toUtf8 (), &db, SQLITE_OPEN_READONLY, nullptr);

  if (db == nullptr)
  {
     err << "error: " << "unable to allocate memory" << endl;
     return false;
  }

  if (rc != SQLITE_OK)
  {
    err << "error: " << sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return false;
  }

  // check dbfile version
  int dbversion;
  QString SQL = QStringLiteral ("SELECT * FROM VERSION;");
  rc = sqlite3_exec(db, SQL.toUtf8(), sqlcb_dbversion, (void *) &dbversion, NULL);
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return false;
  }

  if (dbversion < MIN_SUPPORTED_DBVERSION)
  {
    err << "error: invalid database file version: " << dbversion << endl;
    return false;
  }

  sqlite3_close (db);
  return true;
}

// parse module options
static int
parse_module (int argc, char *argv[])
{
  if (argc < 3)
  {
    err << "error: " << "unspecified option" << endl;
    return 1;
  }

  if (std::strcmp (argv[2], "--list") == 0)
  {
    if (argc == 3)
      return list_modules (defaultSqliteFile);
    else
    if (argc == 4)
      return list_modules (argv[3]);
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--compile") == 0 ||
      std::strcmp (argv[2], "--verbose-compile") == 0)
  {
    if (std::strcmp (argv[2], "--verbose-compile") == 0)
      verbose_flag = true;

    if (argc == 4)
      return compile_module (QString (argv[3]), defaultSqliteFile);
    else
    if (argc == 5)
      return compile_module (QString (argv[3]), QString(argv[4]));
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--decompile") == 0)
  {
    if (argc == 4)
      return decompile_module (QString (argv[3]), defaultSqliteFile);
    else
    if (argc == 5)
      return decompile_module (QString (argv[3]), QString(argv[4]));
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--import") == 0)
  {
    if (argc == 4)
      return import_module (QString (argv[3]), defaultSqliteFile);
    else
    if (argc == 5)
      return import_module (QString (argv[3]), QString(argv[4]));
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--delete") == 0)
  {
    if (argc == 4)
      return delete_module (QString (argv[3]), defaultSqliteFile);
    else
    if (argc == 5)
      return delete_module (QString (argv[3]), QString(argv[4]));
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--export") == 0)
  {
    if (argc == 4)
      return export_module (QString (argv[3]), defaultSqliteFile);
    else
    if (argc == 5)
      return export_module (QString (argv[3]), QString(argv[4]));
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  err << "error: " << "unknown option" << endl;
  return 1;
}

// parse data options
static int
parse_data (int argc, char *argv[])
{
  if (argc < 3)
  {
    err << "error: " << "unspecified option" << endl;
    return 1;
  }

  if (std::strcmp (argv[2], "--list") == 0)
  {
    if (argc == 3)
      return list_symbols (defaultSqliteFile);
    else
    if (argc == 4)
      return list_symbols (argv[3]);
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }
  else
  if (std::strcmp (argv[2], "--export") == 0 ||
      std::strcmp (argv[2], "--export-tostdout") == 0)
  {
    if (std::strcmp (argv[2], "--export-tostdout") == 0)
      tostdout_flag = true;

    if (argc == 4)
      return export_data (argv[3], defaultSqliteFile);
    else
    if (argc == 5)
      return export_data (argv[3], argv[4]);
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  err << "error: " << "unknown option" << endl;
  return 1;
}

// parse info options
static int
parse_info (int argc, char *argv[])
{
  if (argc < 3)
  {
    err << "error: " << "unspecified option" << endl;
    return 1;
  }

  if (std::strcmp (argv[2], "--cgscript-toolchain") == 0)
  {
    if (argc == 4)
      return cgscript_toolchain (argv[3], defaultSqliteFile);
    else
    if (argc == 5)
      return cgscript_toolchain (argv[3], argv[4]);
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  if (std::strcmp (argv[2], "--dbfile") == 0)
  {
    if (argc == 3)
      return dbfile_info (defaultSqliteFile);
    else
    if (argc == 4)
      return dbfile_info (argv[3]);
    else
      err << "error: " << "invalid argument" << endl;

    return 1;
  }

  err << "error: " << "unknown option" << endl;
  return 1;
}


// control function
int
control (int argc, char *argv[])
{
  if (argc < 2)
  {
    err << "error: " << "unspecified mode" << endl;
    return 1;
  }

  if (std::strcmp (argv[1], "module") == 0)
    return parse_module (argc, argv);
  else
  if (std::strcmp (argv[1], "help") == 0)
    return help ();
  else
  if (std::strcmp (argv[1], "data") == 0)
    return parse_data (argc, argv);
  else
  if (std::strcmp (argv[1], "info") == 0)
    return parse_info (argc, argv);
  else
    err << "error: " << "invalid mode" << endl;

  return 1;
}
