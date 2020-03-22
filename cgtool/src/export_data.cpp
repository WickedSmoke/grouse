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

#include <QFile>
#include <QIODevice>

#include "cgtool.h"

// sqlite3 callback for tableid->table/view name for symbols
static int
sqlcb_symbol_table_name (void *tname, int argc, char **argv, char **column)
{
  QString *tablename = static_cast <QString *> (tname);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("KEY"))
    {
      *tablename = QString::fromUtf8 (argv[counter]);
      return 0;
    }
  }

  return 0;
}

// sqlite3 callback for data export
static int
sqlcb_export_data (void *tstream, int argc, char **argv, char **column)
{
  QTextStream *textstream = static_cast <QTextStream *> (tstream);
  QString outline = "";

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();

    if (colname != "DATE" && colname != "TIME")
      outline += QString::number(QString (argv[counter]).toDouble (),'f', 4);
    else
      outline += QString (argv[counter]);

    if (counter < argc - 1)
      outline += "|";
  }

  *textstream << outline << endl;

  return 0;
}

// export data
int
export_data (const QString &tableid, const QString &dbfile)
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

  // get the table name
  QString SQL = QStringLiteral ("SELECT key FROM symbols WHERE key2 = '") %
                tableid % QStringLiteral ("';"), tablename;

  rc = selectfromdb(db, SQL.toUtf8(), sqlcb_symbol_table_name,
                    static_cast <void *> (&tablename));
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  SQL =
    QStringLiteral ("SELECT open, high, low, close, volume, date, time, adjclose FROM ") %
    tablename %  QStringLiteral (" ORDER BY date DESC, time DESC;");

  // open the output file
  QFile file;

  if (tostdout ())
  {
    file.open (1, QIODevice::WriteOnly|QIODevice::Text);
  }
  else
  {
    file.setFileName (tablename % QStringLiteral (".csv"));
    if (!file.open (QFile::WriteOnly|QFile::Text) && tostdout ())
    {
      err << "error: cannot open output file" << endl;
      return 1;
    }
  }

  QTextStream textstream (&file);

  textstream << "OPEN|HIGH|LOW|CLOSE|VOLUME|DATE|TIME|ADJCLOSE" << endl;

  rc = selectfromdb(db, SQL.toUtf8(), sqlcb_export_data,
                    static_cast <void *> (&textstream));
  if (rc != SQLITE_OK)
  {
    err << "error: "<< sqlite3_errmsg (db) << endl;
    sqlite3_close (db);
    return 1;
  }

  file.close ();
  sqlite3_close (db);

  return 0;
}
