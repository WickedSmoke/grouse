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

#include <vector>
#include "feedyahoo.h"
#include "feediex.h"
#include "feedav.h"
#include "qtachart_object.h"

// Price worker for a single symbol
class PriceWorker : public QObject
{
  Q_OBJECT

public:
  PriceWorker (QString symbol, QString feed);
  ~PriceWorker ();

  QString getSymbol () const NOEXCEPT { return symbol; }
  bool isRunning () { return (bool) state.fetchAndAddAcquire (0); }
  void setParentObject (QTACObject *obj) { parentObject = obj; }

public slots:
  void process();               // thread process
  void terminate () NOEXCEPT;   // thread terminate

private:
  QTACObject *parentObject; // parent object
  QAtomicInt runflag;       // set false to terminate execution
  QAtomicInt state;         // true if running
  FeedSource source;
  QString symbol;           // symbol
  YahooFeed *yfeed;         // Yahoo Finance feed
  IEXFeed *efeed;           // IEX feed
  AlphaVantageFeed *afeed;  // Alpha Vantage feed
  RTPrice rtprice;          // real time price
};


struct TickerPrice
{
    char symbol[8];
    char price[8];
    char prcchange[10];
};

Q_DECLARE_METATYPE(TickerPrice);
Q_DECLARE_TYPEINFO(TickerPrice, Q_MOVABLE_TYPE);

typedef std::vector< TickerPrice > TickerPrices;
Q_DECLARE_METATYPE(TickerPrices);


// Price worker for a symbol list
class PriceWorkerTicker : public QObject
{
  Q_OBJECT

public:
  PriceWorkerTicker ();
  ~PriceWorkerTicker ();
  bool isRunning () { return (bool) state.fetchAndAddAcquire (0); }

signals:
  void updatePrices(TickerPrices);

public slots:
  void process();               // thread process
  void terminate () NOEXCEPT;   // thread terminate

private:
  QAtomicInt runflag;           // set false to terminate execution
  QAtomicInt state;             // non-zero if running
  YahooFeed *yfeed;             // Yahoo Finance feed
  IEXFeed *efeed;               // IEX feed
  AlphaVantageFeed *afeed;
};


class StockTicker;

class PriceUpdater: public QObject
{
  Q_OBJECT

public:
  explicit PriceUpdater (StockTicker *parent);
  PriceUpdater (QString symbol, QString feed, QTACObject *parent);
  ~PriceUpdater ();

private:
  QThread thread;           // worker thread

  // Only one of the following is used based upon constructor.
  PriceWorker *worker;              // Used with QTACObject
  PriceWorkerTicker *tickerworker;  // Used with StockTicker
};

#endif // PRICEUPDATER_H
