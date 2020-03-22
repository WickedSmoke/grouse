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

#pragma once

#include <cstdlib>

#include <QString>
#include <QVector>
#include <QtGlobal>
#include <QTextStream>

#include "sqlite3.h"
#include "appdata.h"

// Is nullptr supported?
#if __cplusplus <= 201103L
#ifndef Q_OS_MAC
#define nullptr		NULL
#endif
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#ifndef QStringLiteral
#define QStringLiteral(str) QString::fromUtf8("" str "", sizeof(str) - 1)
#endif
#endif

#define TOOLNAME        "cgtool"

/// types
// toolchain record type
typedef struct
{
  QString platform;
  QString compiler;
  QString compilerdbg;
  QString compilerrel;
  QString linker;
  QString linkerdbg;
  QString linkerrel;
} ToolchainRec;

// toolchain vector type
typedef QVector < ToolchainRec * > ToolchainVector;

// module record type
typedef struct
{
  QString id;
  QString name;
  QString source;
  QString binary;
  QString type;
  QString author;
  QString version;
  QString status;
} ModuleRec;

// module vector type
typedef QVector < ModuleRec * > ModuleVector;

// symbol record type
typedef struct
{
  QString key;
  QString symbol;
  QString name;
  QString feed;
 } SymbolRec;

// symbol vector type
typedef QVector < SymbolRec * > SymbolVector;

/// variables
// stdin, stdout and stderr
extern QTextStream in;
extern QTextStream out;
extern QTextStream err;

// installation path
extern QString installationPath;

// sqlite file
extern const QString defaultSqliteFile;

/// functions
// verbose flag
extern bool
verbose ();

// tostdout flag
extern bool
tostdout ();

// nullDevice
QString nullDevice();

// check file existence
extern bool
checkFileExistence (const QString &pathname);

// check dbfile version
extern bool
checkDBFileVersion (const QString &dbfile);

// returns the platform string (eg linux64-gcc)
extern QString platformString (void);

// datafeeds' callback
extern int
sqlcb_datafeeds (void *dummy, int argc, char **argv, char **column);

// sqlite callback for modules
extern int sqlcb_modules (void *classptr, int argc,
                          char **argv, char **column);

// sqlite callback for tables
extern int sqlcb_symbol_table (void *classptr, int argc,
                          char **argv, char **column);

// sqlite callback for toolchain
extern int sqlcb_toolchain (void *classptr, int argc,
                            char **argv, char **column);

// sqlite callback for ALL toolchains
extern int sqlcb_toolchains (void *classptr, int argc,
                            char **argv, char **column);

// sqlite callback for dbfile version check
extern int sqlcb_dbversion (void *versionptr, int argc,
                            char **argv, char **column);

// select from database
extern int
selectfromdb (sqlite3 *db, const char *sql,
              int (*callback)(void*,int,char**,char**), void *arg1);
              
// insert or update database
int
updatedb (sqlite3 *db, QString &SQL);              
