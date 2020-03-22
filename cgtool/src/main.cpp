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

#include <QDir>
#include <QFileInfo>

#include "cgtool.h"

const QString defaultSqliteFile =
  QDir::homePath () % QDir::separator() % QStringLiteral (".config") %
  QDir::separator() % APPDIR % QDir::separator() % DBNAME;

QString installationPath;
QTextStream in(stdin), out(stdout), err(stderr);

// control
extern int control (int argc, char *argv[]);

static void
atexit_handler ()
{
  out << endl;
}

// help
extern int help ();

// check existence of file
bool
checkFileExistence (const QString &pathname)
{
  if (QFile::exists(pathname))
    return true;

  err << "error: " << pathname << " does not exist" << endl;
  return false;
}

// nullDevice
QString
nullDevice()
{
#ifdef Q_OS_WIN
  return QStringLiteral("\\\\.\\NUL");
#elif defined(_PATH_DEVNULL)
  return QStringLiteral(_PATH_DEVNULL);
#else
  return QStringLiteral("/dev/null");
#endif
}

int
main (int argc, char *argv[])
{
  std::atexit (atexit_handler);

  const QString argv0(argv[0]);
  QFileInfo fi(argv0);
  installationPath = fi.absolutePath();

  if (argc == 1)
  {
    help ();
    return 1;
  }
  else
    return control (argc, argv);
}

