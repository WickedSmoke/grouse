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

#ifndef PRICEUPDATER_H
#define PRICEUPDATER_H

#include "feedyahoo.h"
#include "feediex.h"
#include "feedav.h"
#include "qtachart_object.h"

// Price worker for a single symbol
class PriceWorker : public QObject
{
  Q_OBJECT

public:
  PriceWorker (QString symbol, QString feed); // constructor
  ~PriceWorker (void); // destructor

  QString getFeed () const NOEXCEPT
  {
    return datafeed;
  }; // returns the data feed
  QString getSymbol () const NOEXCEPT
  {
    return symbol;
  }; // returns the symbol
  bool isRunning ()
  {
    return (bool) state.fetchAndAddAcquire (0);
  }; // returns running state
  void setParentObject (QTACObject *obj)
  {
    parentObject = obj;
  }; // set the parent object

public slots:
  void process(void);           // thread process
  void terminate (void) NOEXCEPT;       // thread terminate

private:
  QTACObject *parentObject; // parent object
  QAtomicInt runflag;     // set false to terminate execution
  QAtomicInt state;   // true if running
  QString datafeed;   // data feed
  QString symbol;     // symbol
  YahooFeed *yfeed;  // Yahoo Finance feed
  IEXFeed *efeed; // IEX feed
  AlphaVantageFeed *afeed; // Alpha Vantage feed
  RTPrice rtprice; // real time price
};


class StockTicker;

// Price worker for a symbol list
class PriceWorkerTicker : public QObject
{
  Q_OBJECT

public:
  PriceWorkerTicker (void); //constructor
  ~PriceWorkerTicker (void);  // destructor
  bool isRunning ()
  {
    return (bool) state.fetchAndAddAcquire (0);
  }; // returns running state
  void setParentObject (StockTicker *obj)
  {
    parentObject = obj;
  }; // set the parent object

public slots:
  void process(void);       // thread process
  void terminate (void) NOEXCEPT;   // thread terminate

signals:

private:
  StockTicker *parentObject;    // parent object
  QAtomicInt runflag;     // set false to terminate execution
  QAtomicInt state;   // true if running
  QStringList datafeed;   // data feed
  QStringList symbol;     // symbol
  YahooFeed *yfeed;  // Yahoo Finance feed
  IEXFeed *efeed; // IEX feed
  AlphaVantageFeed *afeed; //
  RTPriceList rtprice; // real time price
};


class PriceUpdater: public QObject
{
  Q_OBJECT

public:
  explicit PriceUpdater (StockTicker *parent); // constructor
  PriceUpdater (QString symbol, QString feed, QTACObject *parent); // constructor
  ~PriceUpdater (void);      // destructor

private:
  QThread thread;   // worker thread
  PriceWorker *worker; // worker class
  PriceWorkerTicker *tickerworker; // worker class

};

#endif // PRICEUPDATER_H
