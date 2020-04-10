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

#ifndef FEEDIEX_H
#define FEEDIEX_H

#include <QVector>
#include <QObject>
#include "idb.h"

namespace Ui
{
  class IEXFeed;
}

typedef struct
{
  QString symbol;
  QString name;
} IEXSymbol;

typedef QVector < IEXSymbol > IEXSymbolVector;  	

class IEXFeed: public QObject
{
  Q_OBJECT
  
public:
  explicit IEXFeed (QObject *parent = 0); // constructor
  ~IEXFeed (void);	     // destructor

  // functions
  bool symbolExistence (const QString & symbol,
                        QString & name,
                        QString & market); // check if symbol exists

  CG_ERR_RESULT downloadData (const QString& symbol,    // download
                              const QString& timeframe, // historical
                              const QString& currency,  // data
                              const QString& task,
                              bool    adjust);
  CG_ERR_RESULT downloadStats (const QString& symbol); // download statistics
  CG_ERR_RESULT getRealTimePrice (const QString& symbol,
                                  RTPrice & rtprice); // get real time price
  QString getTableName ()
  {
    return tableName;
  };   // get the table name of the last operation

  bool validSymbol (const QString& symbol); // validate IEX symbol
  IEXSymbolVector getSymbols (); // get symbol list 
  CG_ERR_RESULT populateSymlist (); // populate the symlist table

private:
  // variables and classes
  RTPrice realtimeprice;// real time price
  QString Symbol;		// symbol
  QString symbolName; 	// symbolname
  QString Market;		// market
  QString Currency;		// currency
  QString BookValue;	// book value
  QString MarketCap;	// market capitalization
  QString EBITDA;		// EBITDA
  QString PE;			// Price/Earnings
  QString PEG;			// Price/Earnings
  QString Yield;		// Yield
  QString EPScy;		// EPS current year
  QString EPSny;		// EPS next year
  QString ES;			// Earnings/Share
  QString PS;			// Price/Sales
  QString PBv;			// Price/Book Value
  QString tableName;	// table name of the last operation
  SymbolEntry entry;	// symbol entry
  IEXSymbolVector IEXSymbols; // symbol list
  bool populated; // true if symlist table is populated

  // functions
  QString symbolURL (const QString& symbol); // returns symbol check URL
  QString symbolStatsURL (const QString& symbol); // returns symbol statistics URL
  QString downloadURL (const QString& symbol); // download URL
  QString updateURL (const QString& symbol); // update URL
  QString realTimePriceURL (const QString& symbol); // real time price URL
};

#endif // FEEDIEX_H
