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
#endif

// sqlite3_exec callback for retrieving single toolchain path and options
int
sqlcb_toolchain (void *classptr, int argc, char **argv, char **column)
{
#ifdef CGTOOL
  ToolchainRec *options = static_cast <ToolchainRec *> (classptr);
#else
  AppOptions *options = static_cast <AppOptions *> (classptr);
#endif

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == QLatin1String ("PLATFORM"))
      options->platform = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILER"))
      options->compiler = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILERDBG"))
      options->compilerdbg = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILERREL"))
      options->compilerrel = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKER"))
      options->linker = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKERDBG"))
      options->linkerdbg = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKERREL"))
      options->linkerrel = QString (argv[counter]);
  }
  return 0;
}

// sqlite3_exec callback for retrieving all toolchains
int
sqlcb_toolchains (void *classptr, int argc, char **argv, char **column)
{
  ToolchainVector *vec = static_cast <ToolchainVector *> (classptr);
  ToolchainRec *rec = new ToolchainRec;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == QLatin1String ("PLATFORM"))
      rec->platform = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILER"))
      rec->compiler = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILERDBG"))
      rec->compilerdbg = QString (argv[counter]);
    else
    if (colname == QLatin1String ("COMPILERREL"))
      rec->compilerrel = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKER"))
      rec->linker = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKERDBG"))
      rec->linkerdbg = QString (argv[counter]);
    else
    if (colname == QLatin1String ("LINKERREL"))
      rec->linkerrel = QString (argv[counter]);
  }

  vec->append (rec);

  return 0;
}
