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

#ifdef _WIN32
#define stringEqualI(A,B)  (_stricmp(A,B) == 0)
#else
#define stringEqualI(A,B)  (strcasecmp(A,B) == 0)
#endif

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

QAtomicInt GlobalError( CG_ERR_OK );
InstrumentDatabase* gDatabase = nullptr;
QString UID, RunCounter;



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


// TODO: Make this a callback to let user control response.
static void _dbExit( int exitStatus, const char* msg )
{
    showMessage(QString::fromUtf8(msg));
    qApp->exit( exitStatus );

#if defined (Q_OS_WIN) || defined (Q_OS_MAC)
    exit(exitStatus);
#else
    quick_exit(exitStatus);
#endif
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
    sqlite3_close(_db);
    _dbExit(1, "Cannot create or open database. Application quits.");
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
  const char* colname;

  if (dummy != nullptr)
    return 1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = column[counter];
    if (stringEqualI(colname, "UID"))
      UID = QString::fromUtf8 (argv[counter]);
    else if (stringEqualI(colname, "RUNCOUNTER"))
      RunCounter = QString::fromUtf8 (argv[counter]);
  }
  return 0;
}


bool InstrumentDatabase::openFile( const QString& filename, const char** err )
{
  QFileInfo dbfile;
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

      QString SQLCommand;
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
      _dbExit(0, "Decryption completed. Now restart the application.");
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
      goto create_fail;
    sqlite3_extended_result_codes(_db, 1);

    // apply pragmas
    rc = sqlite3_exec(_db, _pragma.toUtf8(), nullptr, this, nullptr);
    if (rc != SQLITE_OK) // if open failed, quit application
      goto create_fail;

    if (dbman (1, _db) != CG_ERR_OK)
      goto create_fail;

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
      goto create_fail;
    sqlite3_extended_result_codes(_db, 1);
  }

  // apply pragmas
  rc = sqlite3_exec(_db, _pragma.toUtf8(), nullptr, this, nullptr);
  if (rc != SQLITE_OK) // if open failed, quit application
    goto create_fail;

  // check version
  dbversion = dbVersion();
  if (dbversion == -1) // invalid dbversion
  {
    sqlite3_close (_db);
    _db = nullptr;
    FATAL("Invalid data file.");
  }

  dbversion ++; // upgrade db file
  if (dbman (dbversion, _db) != CG_ERR_OK)
    goto create_fail;

  // sqlite3 limit options for linux
#ifdef Q_OS_LINUX
  // Set maximum columns, sql length, compound select, & expression depth.
  sqlite3_limit(_db, SQLITE_LIMIT_COLUMN, 256);
  sqlite3_limit(_db, SQLITE_LIMIT_SQL_LENGTH, 20971520);
  sqlite3_limit(_db, SQLITE_LIMIT_COMPOUND_SELECT, 128);
  sqlite3_limit(_db, SQLITE_LIMIT_EXPR_DEPTH, 256);
#endif

  // increase run counter
  rc = sqlite3_exec(_db, "UPDATE VERSION SET RUNCOUNTER = RUNCOUNTER + 1;",
                    nullptr, nullptr, nullptr);

  // load run counter and UID
  rc = sqlite3_exec(_db, "SELECT * FROM VERSION;",
                    sqlcb_dbdata, nullptr, nullptr);
  //printf("RunCounter: %d\n", RunCounter.toInt());

  // Vacuum occasionally.  This process can take a few seconds even when
  // the historical data for only ten securities has been stored.
  if( (RunCounter.toInt() & 15) == 15 )
  {
    rc = sqlite3_exec(_db, "REINDEX; VACUUM;", nullptr, nullptr, nullptr);
  }

  return true;

create_fail:

  sqlite3_close (_db);
  _db = nullptr;
  FATAL("Cannot create or open database.");
}


void InstrumentDatabase::initializeListQueries( SQLists& lists )
{
    lists.formats_query          = "select FORMAT from FORMATS";
    lists.timeframes_query       = "select TIMEFRAME from TIMEFRAMES_ORDERED";
    lists.currencies_query       = "select SYMBOL from CURRENCIES";
    lists.markets_query          = "select MARKET from MARKETS";
    lists.datafeeds_query        = "select * from DATAFEEDS order by FEEDNAME";
    lists.transactiontypes_query = "select * from TRANSACTIONTYPES";
    lists.commissiontypes_query  = "select * from COMMISSIONTYPES";
}


bool InstrumentDatabase::disableModules()
{
    QString cmd;
    int rc;

#ifdef DEBUG
    // disable all modules
    sqlite3_exec(_db, "UPDATE modules SET STATUS ='DISABLED';",
                 nullptr, nullptr, nullptr);
#endif

    // disable all modules when move from a platform to another
    cmd = QStringLiteral("UPDATE modules SET STATUS = 'DISABLED' WHERE PLATFORM <> '") %
                         platformString () % QStringLiteral ("';");
    rc = sqlite3_exec(_db, cmd.toUtf8(), nullptr, nullptr, nullptr);
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


static int sqlcb_table_data(void *user, int argc, char **argv,
                            char **column)
{
  TableDataClass tdc;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    // key, symbol,  timeframe, description, adjusted, base, market, source
    if (colname == QLatin1String ("KEY"))
      tdc.tablename = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("SYMBOL"))
      tdc.symbol = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("TIMEFRAME"))
      tdc.timeframe = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("DESCRIPTION"))
      tdc.name = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("ADJUSTED"))
      tdc.adjusted = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("BASE"))
      tdc.base = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("MARKET"))
      tdc.market = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("SOURCE"))
      tdc.source = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("LASTUPDATE"))
      tdc.lastupdate = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("CURRENCY"))
      tdc.currency = QString (argv[counter]);
  }
  ((TableDataVector*) user)->push_back(tdc);

  return 0;
}


/**
  \param data   Vector to fill with data.  It will be cleared before the
                database select operation is done.
*/
int InstrumentDatabase::loadTableData( const QString& base,
                                       const QString& adjusted,
                                       TableDataVector* data )
{
    QString query;
    int rc;

    query = QStringLiteral(
        "SELECT key, symbol, timeframe, description, adjusted, base, market,"
        " source, lastupdate, currency FROM symbols WHERE base = '") % base %
        QStringLiteral("' AND ADJUSTED = '") % adjusted %
        QStringLiteral("' ORDER BY tfresolution ASC;");

    data->clear();
    rc = selectfromdb(query.toUtf8(), sqlcb_table_data, data);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
        return CG_ERR_DBACCESS;
    }
    return CG_ERR_OK;
}


int sqlcb_dbversion(void *versionptr, int argc, char **argv, char **column);

/**
  Return database version or -1 if error.
*/
int InstrumentDatabase::dbVersion()
{
  int version = -1;
  sqlite3_exec(_db, "SELECT * FROM VERSION;",
               sqlcb_dbversion, (void *) &version, nullptr);
  return version;
}


/*
  Load lists.datafeedsList, symlistList, realtimeList, & symlisturlList.
  Returns CG_ERR_OK if successful.
*/
int InstrumentDatabase::loadDatafeeds( SQLists& lists )
{
    lists.datafeedsList.clear();
    lists.symlistList.clear();
    lists.realtimeList.clear();
    lists.symlisturlList.clear();

    int rc = selectfromdb(lists.datafeeds_query, sqlcb_datafeeds, NULL);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
        return CG_ERR_DBACCESS;
    }

    lists.datafeedsList.sort();
    return CG_ERR_OK;
}


/*
  Load lists.currencyList.
  Returns CG_ERR_OK if successful.
*/
int InstrumentDatabase::loadCurrencies( SQLists& lists )
{
    lists.currencyList.clear();

    int rc = selectfromdb(lists.currencies_query, sqlcb_currencies, NULL);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
        return CG_ERR_DBACCESS;
    }

    return CG_ERR_OK;
}


// SQL callbacks

#define LIST_QUERY1(NAME, OUTPUT) \
    const char* colname; \
    if (dummy != nullptr) \
        return 1; \
    for (qint32 counter = 0; counter < argc; counter ++) { \
        colname = column[counter]; \
        if (stringEqualI(colname, NAME)) \
            ComboItems->OUTPUT << QString(argv[counter]); \
    } \
    return 0;


int sqlcb_formats(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("FORMAT", formatList)
}

int sqlcb_timeframes(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("TIMEFRAME", timeframeList)
}

int sqlcb_currencies(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("SYMBOL", currencyList)
}

int sqlcb_markets(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("MARKET", marketList)
}

int sqlcb_transactiontypes(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("DESCRIPTION", transactiontypeList)
}

int sqlcb_commissiontypes(void *dummy, int argc, char **argv, char **column)
{
    LIST_QUERY1("DESCRIPTION", commissiontypeList)
}

static int sqlcb_tickersymbols(void *data, int argc, char **argv, char **column)
{
    QStringList *slist = static_cast <QStringList *> (data);
    const char* colname;

    for (qint32 counter = 0; counter < argc; counter ++)
    {
        colname = column[counter];
        if (stringEqualI(colname, "SYMBOL"))
            slist->append(QString(argv[counter]));
    }
    return 0;
}

static int sqlcb_tickerfeed(void *data, int argc, char **argv, char **column)
{
    QStringList *feed = static_cast <QStringList *> (data);
    const char* colname;

    for (qint32 counter = 0; counter < argc; counter ++)
    {
        colname = column[counter];
        if (stringEqualI(colname, "FEED") ||
            stringEqualI(colname, "DATAFEED"))
        feed->append(QString(argv[counter]));
    }
    return 0;
}


CG_ERR_RESULT loadPortfolioSymbols(QStringList& symbol, QStringList& feed,
                                   int pfid)
{
  QString query, table;
  QStringList wsymbol, wfeed;
  int rc;

  table = QStringLiteral ("pftrans_") % QString::number (pfid) % QStringLiteral ("summary");
  query = QStringLiteral ("SELECT SYMBOL FROM ") % table % QStringLiteral (" WHERE QUANTITY <> 0 ORDER BY SYMBOL;");
  rc = selectfromdb(query.toUtf8(), sqlcb_tickersymbols,
                    static_cast <void *> (&wsymbol));
  if (rc == SQLITE_OK)
  {
    query += QStringLiteral ("SELECT DATAFEED FROM ") % table % QStringLiteral (" WHERE QUANTITY <> 0 ORDER BY SYMBOL;");
    rc = selectfromdb(query.toUtf8(), sqlcb_tickerfeed,
                      static_cast <void *> (&wfeed));
    if (rc != SQLITE_OK)
    {
      symbol.clear ();
      return CG_ERR_DBACCESS;
    }
  }
  else
    return CG_ERR_DBACCESS;

  symbol = wsymbol;
  feed = wfeed;

  return CG_ERR_OK;
}


CG_ERR_RESULT InstrumentDatabase::loadTickerSymbols(QStringList& symbol,
                                                    QStringList& feed)
{
    int rc;

    rc = selectfromdb("SELECT SYMBOL FROM TICKER_SYMBOLS ORDER BY SYMBOL;",
                      sqlcb_tickersymbols, static_cast <void *> (&symbol));
    if (rc != SQLITE_OK)
        return CG_ERR_DBACCESS;

    rc = selectfromdb( "SELECT FEED FROM TICKER_SYMBOLS ORDER BY SYMBOL;",
                       sqlcb_tickerfeed, static_cast <void *> (&feed));
    if (rc != SQLITE_OK)
    {
        symbol.clear();
        return CG_ERR_DBACCESS;
    }
    return CG_ERR_OK;
}


CG_ERR_RESULT InstrumentDatabase::saveTickerSymbols(QStringList& symbol,
                                                    QStringList& feed)
{
  QString query;
  int rc;

  query = QStringLiteral ("DELETE FROM TICKER_SYMBOLS;");
  for (qint32 counter = 0; counter < symbol.size (); counter ++)
  {
    query += QStringLiteral ("INSERT INTO TICKER_SYMBOLS (SYMBOL, FEED) VALUES ('") %
             symbol[counter] % QStringLiteral ("','") % feed[counter] % QStringLiteral ("');");
  }
  query.append ('\n');

  rc = updatedb (query);
  if (rc != SQLITE_OK)
    return CG_ERR_DBACCESS;

  return CG_ERR_OK;
}

#endif


// datafeeds' callback
int sqlcb_datafeeds (void *dummy, int argc, char **argv, char **column)
{
  const char* colname;

#ifdef CGTOOL
  QStringList *datafeedsList = static_cast <QStringList *> (dummy);
#else
  if (dummy != nullptr)
    return 1;
#endif

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = column[counter];
    if (stringEqualI(colname, "FEEDNAME"))
#ifdef CGTOOL
      *datafeedsList << QString (argv[counter]);
#else
      ComboItems->datafeedsList << QString (argv[counter]);
    else if (stringEqualI(colname, "REALTIME"))
      ComboItems->realtimeList << QString (argv[counter]);
    else if (stringEqualI(colname, "SYMLIST"))
      ComboItems->symlistList << QString (argv[counter]);
    else if (stringEqualI(colname, "SYMLISTURL"))
      ComboItems->symlisturlList << QString (argv[counter]);
#endif
  }
  return 0;
}


int sqlcb_dbversion (void *versionptr, int argc, char **argv, char **column)
{
  const char* colname;
  int version = -1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = column[counter];
    if (stringEqualI(colname, "VERSION"))
      version = atoi(argv[counter]);
  }
  *(int *) versionptr = version;
  return 0;
}
