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

#ifndef IDB_H
#define IDB_H


#include <QMetaType>
#include <QMutex>
#include <QStringList>
#include "errors.h"
#include "sqlite3.h"


// Real time price
#ifdef Q_OS_WIN
typedef struct alignas (4)
#else
typedef struct alignas (8)
#endif
{
    QString symbol;       // symbol
    QString feed;         // data feed (GOOGLE, YAHOO etc)
    QString price;        // current price
    QString change;       // price change
    QString prcchange;    // price percent change
    QString open;         // day open
    QString high;         // day high
    QString low;          // day low
    QString volume;       // day volume
    QString date;         // date
    QString time;         // time
} RTPrice;

Q_DECLARE_METATYPE (RTPrice);
Q_DECLARE_TYPEINFO (RTPrice, Q_MOVABLE_TYPE);
typedef QList < RTPrice > RTPriceList;
Q_DECLARE_METATYPE (RTPriceList);


#define QSTR_SIZE   128

// Lists populated by SQL statements
typedef struct
{
    QStringList formatList;       // list of supported formats
    QStringList timeframeList;    // list of supported timeframes
    QStringList currencyList;     // list of supported currencies
    QStringList marketList;       // list of supported markets
    QStringList datafeedsList;    // list of supported datafeeds
    QStringList realtimeList;     // list of real time flags for datafeeds
    QStringList symlistList;      // list of symlist tables for datafeeds
    QStringList symlisturlList;   // list of symlist urls for datafeeds
    QStringList transactiontypeList;    // list of portfolio transaction types
    QStringList commissiontypeList;     // list of commission type
    char formats_query[QSTR_SIZE];      // "select FORMAT from FORMATS"
    char timeframes_query[QSTR_SIZE];   // "select TIMEFRAME from TIMEFRAMES_ORDERED"
    char currencies_query[QSTR_SIZE];   // "select SYMBOL from CURRENCIES"
    char markets_query[QSTR_SIZE];      // "select MARKET from MARKETS"
    char datafeeds_query[QSTR_SIZE];    // "select * from DATAFEEDS"
    char transactiontypes_query[QSTR_SIZE]; // "select * from TRANSACTIONTYPES"
    char commissiontypes_query[QSTR_SIZE];  // "select * from COMMISSIONTYPES"
} SQLists;


// symbol entry input data
typedef struct
{
    QString csvfile;      // path of csv file
    QString tablename;    // table name (eg AAPL_OTHER_DAY_CSV)
    QString tmptablename; // temporary table name (eg TMP_GSPC_OTHER_DAY_CSV)
    QString symbol;       // symbol (eg AAPL)
    QString name;         // company/index name (eg Apple Inc.)
    QString market;       // market (eg NYSE)
    QString timeframe;    // timeframe (DAY, WEEK, MONTH)
    QString currency;     // currency (USD, EUR etc)
    QString format;       // csv format (eg YAHOO)
    QString source;       // source (CSV, YAHOO etc)
    QString dnlstring;    // download string
    QString BookValue;    // book value
    QString MarketCap;    // market capitalization
    QString EBITDA;       // EBITDA
    QString PE;           // Price/Earnings
    QString PEG;          // Price/Earnings
    QString Yield;        // Yield
    QString EPScy;        // EPS current year
    QString EPSny;        // EPS next year
    QString ES;           // Earnings/Share
    QString PS;           // Price/Sales
    QString PBv;          // Price/Book Value
    bool    adjust;       // true: adjust data, false: do not adjust
} SymbolEntry;


// fundamenta data as loaded from sqlite table
typedef struct
{
    QString bv;
    QString mc;
    QString ebitda;
    QString pe;
    QString peg;
    QString dy;
    QString epscurrent;
    QString epsnext;
    QString es;
    QString ps;
    QString pbv;
} QTAChartData;

Q_DECLARE_TYPEINFO (QTAChartData, Q_MOVABLE_TYPE);


class InstrumentDatabase
{
public:
    InstrumentDatabase();
    ~InstrumentDatabase();

    bool openFile( const QString& filename, const char** err );
    bool disableModules();
    int loadChartData( const QString& base, QTAChartData* data );
    void reset();
    int dbVersion();

private:
    QMutex _iomutex;        // mutex for multithreaded use of sqlite
    sqlite3* _db;           // database handler
    void* _sqlitebuff;      // buffer for sqlite
    QString _dbPath;        // full path of database file
    QString _pragma;        // PRAGMA statements

    friend int selectfromdb(const char*,
                            int (*callback)(void*,int,char**,char**), void*);
    friend int updatedb(QString&, bool);
};


extern QAtomicInt GlobalError;      // global error code
extern QString RunCounter, UID;


// insert or update rows in database tables
extern int updatedb(QString &SQL);
extern int updatedb(QString &SQL, bool trylock);

// update price table
extern void updatePrice(RTPrice rtprice);

// select from database
extern int selectfromdb(const char *sql,
                        int (*callback)(void*,int,char**,char**), void *arg1);

// select count (*) query. returns the counter or -1 on error
extern int selectcount(QString &SQL);

// reset database
extern void resetDatabase();

// random http header
extern QByteArray httpHeader();

// set global error
extern void setGlobalError(CG_ERR_RESULT err, const char *_file_, int _line_);

// returns description of an error code
extern QString errorMessage(CG_ERR_RESULT err);

// platform string (eg linux64-gcc)
extern QString platformString();


#endif // IDB_H