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

#include "sqlite3.h"
#include "cgtool.h"

// select from database
int selectfromdb(sqlite3 *db, const char *sql,
                 int (*callback)(void*,int,char**,char**), void *arg1)
{
  char *errmsg = nullptr;
  int rc;

  rc = sqlite3_exec(db, sql, callback, arg1, &errmsg);
  if (errmsg != nullptr)
    sqlite3_free(errmsg);
  return rc;
}

#else   // not CGTOOL

#include <QTime>
#include "idb.h"
#include "chartapp.h"
#include "common.h"

extern CG_ERR_RESULT dbman(int dbversion, sqlite3*);
extern int sqlcb_dbversion(void *versionptr, int argc,
                           char **argv, char **column);

QAtomicInt GlobalError;
static InstrumentDatabase* gDatabase = nullptr;


// select from database
int selectfromdb(const char *sql,
                 int (*callback)(void*,int,char**,char**), void *arg1)
{
  char *errmsg = nullptr;
  int rc;

  setGlobalError(CG_ERR_OK, __FILE__, __LINE__);
  gDatabase->_iomutex.lock();

  rc = sqlite3_exec(gDatabase->_db, sql, callback, arg1, &errmsg);
  if (errmsg != nullptr)
    sqlite3_free(errmsg);

  gDatabase->_iomutex.unlock();
  return rc;
}

static int sqlcb_count (void *counter, int argc, char **argv, char **column)
{
  Q_UNUSED (column)
  Q_UNUSED (argc)

  int *cnt = (int *) counter;
  *cnt = QString (argv[0]).toInt ();

  return 0;
}

int selectcount (QString &SQL)
{
  int result, rc;

  rc = selectfromdb(SQL.toUtf8(), sqlcb_count, static_cast <void *> (&result));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_ACCESS_DATA, __FILE__, __LINE__);
    return -1;
  }

  return result;
}

// insert or update database
int updatedb(QString &SQL, bool trylock)
{
  QString sql;
  char *errmsg = nullptr;
  static int updcounter = 0;
  int rc;

  sql = QStringLiteral ("BEGIN;") % SQL % QStringLiteral ("COMMIT;");

  updcounter ++;
  if (updcounter == 1000)
  {
    sql += "PRAGMA shrink_memory;";
    updcounter = 0;
  }

  setGlobalError(CG_ERR_OK, __FILE__, __LINE__);

  if (!trylock)
    gDatabase->_iomutex.lock();
  else if (gDatabase->_iomutex.tryLock() == false)
    return SQLITE_OK;

  rc = sqlite3_exec(gDatabase->_db, sql.toUtf8(), nullptr, nullptr, &errmsg);

  if (errmsg != nullptr)
    sqlite3_free(errmsg);

  if (rc != SQLITE_OK)
  {
    gDatabase->reset();
  }

  gDatabase->_iomutex.unlock();
  return rc;
}


int updatedb(QString &SQL)
{
  return updatedb (SQL, false);
}


// update price table
void updatePrice(RTPrice rtprice)
{
  QString SQL = QStringLiteral ("");
  int rc;

  if (rtprice.price.toDouble () <= 0)
    rtprice.price = QStringLiteral ("0");

  SQL += QStringLiteral ("DELETE FROM prices WHERE symbol = '") % rtprice.symbol %
         QStringLiteral ("' AND feed = '") % rtprice.feed % QStringLiteral ("'; ") %
         QStringLiteral ("INSERT INTO prices (symbol, feed, date, price, volume, time, change, \
          prcchange, timestamp) VALUES ('") %
         rtprice.symbol % QStringLiteral ("','") %
         rtprice.feed % QStringLiteral ("','") %
         rtprice.date % QStringLiteral ("',") %
         rtprice.price % QStringLiteral (",'") %
         rtprice.volume % QStringLiteral ("','") %
         rtprice.time % QStringLiteral ("','") %
         rtprice.change % QStringLiteral ("','") %
         rtprice.prcchange % QStringLiteral ("', (datetime('now','localtime')));");
  rc = updatedb (SQL, true);
  if (rc != CG_ERR_OK)
    setGlobalError (CG_ERR_TRANSACTION, __FILE__, __LINE__);
}


// reset the database
void InstrumentDatabase::reset()
{
  int rc;

  // close
  sqlite3_close (_db);

  // open sqlite db
  rc = sqlite3_open(_dbPath.toUtf8 (), &_db);
  if (rc != SQLITE_OK) // if open failed, quit application
      goto fail;
  sqlite3_extended_result_codes(_db, 1);

  // execute pragma
  rc = sqlite3_exec(_db, _pragma.toUtf8(), nullptr, nullptr, nullptr);
  if (rc != SQLITE_OK) // if open failed, quit application
      goto fail;
  return;

fail:
    showMessage(QString::fromUtf8 ("Cannot create or open database. Application quits."));
    sqlite3_close(_db);
    qApp->exit(1);

#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
    exit(1);
#else
    quick_exit(1);
#endif
}


static const size_t UserAgentCount = 12;
static const char* UserAgents[ UserAgentCount ] =
{
  "Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.2 Safari/537.36",
  "Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X, AppleWebKit/536.26 (KHTML, like Gecko, Version/6.0 Mobile/10A5355d Safari/8536.25",
  "Mozilla/5.0 (Windows NT 6.2, AppleWebKit/537.36 (KHTML, like Gecko, Chrome/28.0.1467.0 Safari/537.36",
  "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0, Gecko/20100101 Firefox/40.1",
  "Mozilla/5.0 (X11; Linux x86_64; rv:17.0, Gecko/20121202 Firefox/17.0 Iceweasel/17.0.1",
  "Mozilla/5.0 (Windows NT 6.1, AppleWebKit/537.36 (KHTML, like Gecko, Chrome/41.0.2228.0 Safari/537.36",
  "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.0, Opera 12.14",
  "Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0,",
  "Opera/12.0 (Windows NT 5.2;U;en,Presto/22.9.168 Version/12.00",
  "Mozilla/5.0 (Windows NT 10.0; Win64; x64, AppleWebKit/537.36 (KHTML, like Gecko, Chrome/42.0.2311.135 Safari/537.36 Edge/12.246",
  "Mozilla/4.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/5.0,",
  "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; AS; rv:11.0, like Gecko"
};

// random http header
QByteArray httpHeader()
{
  QTime time = QTime::currentTime();

  qsrand((uint)time.msec());
  const char* str = UserAgents[qrand() % UserAgentCount];
  return QByteArray::fromRawData( str, strlen(str) );
}


// set global error
void setGlobalError(CG_ERR_RESULT err, const char *_file_, int _line_)
{
  Q_UNUSED (_file_)
  Q_UNUSED (_line_)

  if (GlobalError.fetchAndAddAcquire (0) == CG_ERR_OK)
    GlobalError = err;
}


static const size_t ErrorMessageCount = 25;
static const char* ErrorMessages[ ErrorMessageCount ] =
{
    "No error",
    "Cannot open file",
    "Cannot create temporary table",
    "Cannot create table",
    "Cannot insert data",
    "Cannot delete data",
    "Cannot access database",
    "Invalid reply or network error",
    "Cannot create temporary file",
    "Cannot write to file",
    "Transaction error",
    "Not enough memory",
    "Symbol does not exist",
    "Cannot access data",
    "Network timeout",
    "Invalid data",
    "Request pending",
    "Buffer not found",
    "No quotes for symbol",
    "Operation failed",
    "Compiler not found",
    "Compilation failed",
    "No data",
    "No api key",
    "Invalid object type"
};

// error messages
QString errorMessage(CG_ERR_RESULT err)
{
  GlobalError = CG_ERR_OK;
  return QString(ErrorMessages[err]);
}


InstrumentDatabase::InstrumentDatabase() : _db(nullptr), _sqlitebuff(nullptr)
{
    gDatabase = this;
}


InstrumentDatabase::~InstrumentDatabase()
{
    // Close database & free buffer.
    if(_db)
    {
        sqlite3_close(_db);
        _db = nullptr;
    }

    QString backup = QDir::homePath () % QDir::separator() %
        QStringLiteral (".config") % QDir::separator() % APPDIR %
        QDir::separator() % DBNAMEBACKUP;
    QFile::remove(backup);
    QFile::copy(_dbPath, backup);

    if (_sqlitebuff)
    {
        sqlite3_config(SQLITE_CONFIG_HEAP, nullptr, 0, 64);
        free(_sqlitebuff);
    }

    gDatabase = nullptr;
}


static int sqlcb_dbdata(void *dummy, int argc, char **argv, char **column)
{
  QString colname;

  if (dummy != nullptr)
    return 1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("UID"))
      UID = QString::fromUtf8 (argv[counter]);
    if (colname == QLatin1String ("RUNCOUNTER"))
      RunCounter = QString::fromUtf8 (argv[counter]);
  }

  return 0;
}


bool InstrumentDatabase::openFile( const QString& filename, const char** err )
{
  QFileInfo dbfile;
  QString SQLCommand;
  int dbversion;
  int rc;

#define FATAL(msg) \
  *err = msg; \
  return false


  GlobalError = CG_ERR_OK;
  _dbPath = filename;

  // set the sqlite db status
  QFileInfo encstatus;
  QString encstatusname = QDir::homePath () % "/" % ".config" % "/" %
                          APPDIR % "/" % ENCSTATUS;
  encstatus.setFile(encstatusname);

  // check db existence
  dbfile.setFile(_dbPath);

  if (dbfile.exists())
  {
    // check if db is encrypted and decrypt it
    if (encstatus.exists() == false)
    {
      WaitDialog *decryptdlg = new WaitDialog;
      decryptdlg->setMessage (QString::fromUtf8 ("Decrypting database. Please wait..."));
      decryptdlg->show ();
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

      SQLCommand  = "PRAGMA key = " + DBKEY + ";";
      SQLCommand += "PRAGMA locking_mode = EXCLUSIVE;BEGIN EXCLUSIVE;COMMIT;";
      SQLCommand += "ATTACH DATABASE '" + _dbPath + "2' AS plaintext KEY '';";
      SQLCommand += "SELECT sqlcipher_export('plaintext');  DETACH DATABASE plaintext;";

      // open sqlite db
      rc = sqlite3_open(_dbPath.toUtf8(), &_db);
      if (rc != SQLITE_OK)
      {
        delete decryptdlg;
        FATAL("Cannot create or open database.");
      }

      // apply decrypt pragmas
      rc = sqlite3_exec(_db, SQLCommand.toUtf8(), nullptr, this, nullptr);
      if (rc != SQLITE_OK)
      {
        delete decryptdlg;
        FATAL("Cannot create or open database.");
      }
      sqlite3_close (_db);

      // delete old and rename new file
      QFile::remove(_dbPath);
      QFile::rename(_dbPath + "2", _dbPath);

      // create status file
      QFile statfile (encstatusname);
      statfile.open(QIODevice::WriteOnly);
      statfile.close ();

      decryptdlg->hide ();
      showMessage (QString::fromUtf8 ("Decryption completed. Now restart the application."));
      qApp->exit (0);

#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
      exit (1);
#else
      quick_exit (1);
#endif
    }
  }

  // _pragma += "PRAGMA key = " + DBKEY + ";";
  _pragma = QStringLiteral(
    "PRAGMA locking_mode = EXCLUSIVE;BEGIN EXCLUSIVE;COMMIT;\
     PRAGMA max_page_count = 4294967291; PRAGMA mmap_size=33554432;\
     PRAGMA synchronous = EXTRA;\
     PRAGMA secure_delete = 0;\
     PRAGMA threads = 0;\
     PRAGMA automatic_index = ON; \
     PRAGMA journal_mode=TRUNCATE; PRAGMA temp_store=MEMORY;\
     PRAGMA wal_checkpoint(TRUNCATE);");

  // set static heap size for sqlite: 48M
  _sqlitebuff = malloc (1024*1024*1024);
  if (_sqlitebuff != nullptr)
    sqlite3_config (SQLITE_CONFIG_HEAP, _sqlitebuff, 1024*1024*48, 64);

  // enable multithreading
  sqlite3_config (SQLITE_CONFIG_SERIALIZED);


  // check db existence and create it if needed
  dbfile.setFile (_dbPath);
  if (dbfile.exists () == false)
  {
    if (!QDir (QDir::homePath () % QDir::separator()  % QStringLiteral (".config") % QDir::separator()  % APPDIR).exists ())
      QDir ().mkpath (QDir::homePath () % QDir::separator()  % QStringLiteral (".config") % QDir::separator()  % APPDIR);

    // open sqlite db
    rc = sqlite3_open(_dbPath.toUtf8 (), &_db);
    if (rc != SQLITE_OK)
    {
      sqlite3_close (_db);
      FATAL("Cannot create or open database.");
    }
    sqlite3_extended_result_codes(_db, 1);

    // apply pragmas
    rc = sqlite3_exec(_db, _pragma.toUtf8(), nullptr, this, nullptr);
    if (rc != SQLITE_OK) // if open failed, quit application
    {
      sqlite3_close (_db);
      FATAL("Cannot create or open database.");
    }

    if (dbman (1, _db) != CG_ERR_OK)
    {
      sqlite3_close (_db);
      FATAL("Cannot create or open database.");
    }

    // create status file
    QFile statfile (encstatusname);
    statfile.open(QIODevice::WriteOnly);
    statfile.close ();

    /*  Keep this here for development and debug. DO NOT DELETE
        initcopy.setFileName("geanymasterbase.dat");
        initcopy.copy(_dbPath);
    */
  }
  else
  {
    // open sqlite db
    rc = sqlite3_open(_dbPath.toUtf8 (), &_db);
    if (rc != SQLITE_OK) // if open failed, quit application
    {
      showMessage (QString::fromUtf8 ("Cannot create or open database. Application quits."));
      sqlite3_close (_db);
      qApp->exit (1);

#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
      exit (1);
#else
      quick_exit (1);
#endif
    }
    sqlite3_extended_result_codes(_db, 1);
  }

  // apply pragmas
  rc = sqlite3_exec(_db, _pragma.toUtf8(), nullptr, this, nullptr);
  if (rc != SQLITE_OK) // if open failed, quit application
  {
    sqlite3_close (_db);
    FATAL("Cannot create or open database.");
  }

  // check version
  SQLCommand = QStringLiteral ("SELECT * FROM VERSION;");
  rc = sqlite3_exec(_db, SQLCommand.toUtf8(),
                    sqlcb_dbversion, (void *) &dbversion, nullptr);
  if (rc != SQLITE_OK) // if open failed, quit application
  {
    sqlite3_close (_db);
    FATAL("Cannot create or open database.");
  }

  if (dbversion == -1) // invalid dbversion
  {
    sqlite3_close (_db);
    FATAL("Invalid data file.");
  }

  dbversion ++; // upgrade db file
  if (dbman (dbversion, _db) != CG_ERR_OK)
  {
    sqlite3_close (_db);
    FATAL("Cannot create or open database.");
  }

  // sqlite3 limit options for linux
#ifdef Q_OS_LINUX
  // Set maximum columns, sql length, compound select, & expression depth.
  sqlite3_limit(_db, SQLITE_LIMIT_COLUMN, 256);
  sqlite3_limit(_db, SQLITE_LIMIT_SQL_LENGTH, 20971520);
  sqlite3_limit(_db, SQLITE_LIMIT_COMPOUND_SELECT, 128);
  sqlite3_limit(_db, SQLITE_LIMIT_EXPR_DEPTH, 256);
#endif

  // increase run counter
  SQLCommand = QStringLiteral ("UPDATE VERSION SET RUNCOUNTER = RUNCOUNTER + 1;");
  rc = sqlite3_exec(_db, SQLCommand.toUtf8(), nullptr, this, nullptr);

  // vacuum
  SQLCommand = QStringLiteral ("REINDEX; VACUUM;");
  rc = sqlite3_exec(_db, SQLCommand.toUtf8(), sqlcb_dbdata, nullptr, nullptr);

  // load run counter and UID
  SQLCommand = QStringLiteral ("SELECT * FROM VERSION;");
  rc = sqlite3_exec(_db, SQLCommand.toUtf8(), sqlcb_dbdata, nullptr, nullptr);

  return true;
}


bool InstrumentDatabase::disableModules()
{
    QString cmd;
    int rc;

#ifdef DEBUG
    // disable all modules
    sqlite3_exec(_db, "UPDATE modules SET STATUS ='DISABLED';",
                 sqlcb_dbdata, nullptr, nullptr);
#endif

    // disable all modules when move from a platform to another
    cmd = QStringLiteral("UPDATE modules SET STATUS = 'DISABLED' WHERE PLATFORM <> '") %
                         platformString () % QStringLiteral ("';");
    rc = sqlite3_exec(_db, cmd.toUtf8(), sqlcb_dbdata, nullptr, nullptr);
    return( rc == SQLITE_OK );
}


int InstrumentDatabase::loadChartData( const QString& base, QTAChartData* dat )
{
    QString cmd;
    int rc;

    cmd = QStringLiteral("select * from basedata where base = '") %
                         base % QStringLiteral ("'");
    rc = sqlite3_exec(_db, cmd.toUtf8(), sqlcb_fundamentals,
                      static_cast <void *>(dat), nullptr);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_ACCESS_DATA, __FILE__, __LINE__);
        return CG_ERR_ACCESS_DATA;
    }
    return CG_ERR_OK;
}
#endif