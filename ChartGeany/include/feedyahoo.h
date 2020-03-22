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

#ifndef FEEDYAHOO_H
#define FEEDYAHOO_H

#include <QObject>
#include "defs.h"

namespace Ui
{
  class YahooFeed;
}

class YahooFeed: public QObject
{
  Q_OBJECT
  
public:
  explicit YahooFeed (QObject *parent = 0); // constructor
  ~YahooFeed (void);	     // destructor

  enum YAHOO_API { HTTP, YQL, JSON };

  // functions
  bool symbolExistence (QString & symbol,
                        QString & name,
                        QString & market,
                        QString & currency); // check if symbol exists
 
  bool symbolExistencejson (QString & symbol,
                            QString & name,
                            QString & market,
                            QString & currency); // check if symbol exists from json                       

  CG_ERR_RESULT downloadData (QString symbol,    // download
                              QString timeframe, // historical
                              QString currency,  // data
                              QString task,
                              bool    adjust);
  CG_ERR_RESULT downloadStats (QString symbol, YAHOO_API api); // download statistics
  CG_ERR_RESULT downloadStatsjson (QString symbol); // download statistics
  CG_ERR_RESULT getRealTimePrice (QString symbol,
                                  RTPrice & rtprice, YAHOO_API api); // get real time price
  CG_ERR_RESULT getRealTimePricejson (QString symbol, RTPrice & rtprice); // get real time price                                  
  QString getTableName () const NOEXCEPT
  {
    return tableName;
  };   // get the table name of the last operation
  QString getSymbolName () const NOEXCEPT
  {
    return symbolName;
  }; // get the name of the last symbol retrieved

  bool validSymbol (QString symbol);	// validate yahoo symbol

private:
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

  // functions
  QString symbolURL (QString symbol); // returns symbol check URL
  QString symbolURLjson (QString symbol); // returns symbol check URL
  QString symbolCurrencyURL (QString symbol); // returns symbol's currency URL
  QString symbolStatsURL (QString symbol); // returns symbol statistics URL
  QString symbolStatsURLyql (QString symbol); // returns symbol statistics URL for yql
  QString downloadURL (QString symbol, QString timeframe, QString crumb); // download URL
  QString updateURL (QString symbol, QString timeframe, QString datefrom); // update URL
  QString realTimePriceURL (QString symbol); // real time price URL
  QString realTimePriceURLyql (QString symbol); // real time price URL for yql
  QString getCrumb (const QString & namein);
};

#endif
