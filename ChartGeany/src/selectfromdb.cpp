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

#include "sqlite3.h"

#ifdef CGTOOL
#include "cgtool.h"
#else
#include "chartapp.h"
#include "common.h"
#endif // CGTOOL

// select from database
int
#ifdef CGTOOL
selectfromdb (sqlite3 *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg1)
#else
selectfromdb (const char *sql, int (*callback)(void*,int,char**,char**), void *arg1)
#endif
{
  char *errmsg = nullptr;
  int rc;

#ifndef CGTOOL
  setGlobalError(CG_ERR_OK, __FILE__, __LINE__);
  (qobject_cast <ChartApp *> (qApp))->ioLock ();
  rc = sqlite3_exec(Application_Settings->db, sql, callback, arg1, &errmsg);
#else
  rc = sqlite3_exec(db, sql, callback, arg1, &errmsg);
#endif

  if (errmsg != nullptr)
    sqlite3_free(errmsg);

#ifndef CGTOOL
  (qobject_cast <ChartApp *> (qApp))->ioUnlock ();
#endif

  return rc;
}

#ifndef CGTOOL
static int
sqlcb_count (void *counter, int argc, char **argv, char **column)
{
  Q_UNUSED (column)
  Q_UNUSED (argc)

  int *cnt = (int *) counter;

  *cnt = QString (argv[0]).toInt ();

  return 0;
}

int
selectcount (QString &SQL)
{
  int result, rc;

  rc = selectfromdb (SQL.toUtf8 (), sqlcb_count, static_cast <void *> (&result));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_ACCESS_DATA, __FILE__, __LINE__);
    return -1;
  }

  return result;
}
#endif
