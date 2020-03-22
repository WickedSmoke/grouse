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

// insert or update database
int
updatedb (sqlite3 *db, QString &SQL)
{
  QString sql;
  char *errmsg = NULL;
  int rc;

  sql = QStringLiteral ("BEGIN;") % SQL % QStringLiteral ("COMMIT;");

  rc = sqlite3_exec(db, sql.toUtf8(), NULL, NULL, &errmsg);

  if (errmsg != NULL)
    sqlite3_free(errmsg);

  return rc;
}
