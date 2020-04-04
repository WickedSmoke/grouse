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

#include <QGraphicsTextItem>
#include "defs.h"
#include "priceupdater.h"
#include "StockTicker.h"
#include "mainwindow.h"

//#define SIMULATE_FEED


// PriceWorkerTicker

PriceWorkerTicker::PriceWorkerTicker ()
{
  state = 0;
  runflag = 1;
#ifndef SIMULATE_FEED
  yfeed = new YahooFeed (this);
  efeed = new IEXFeed (this);
  afeed = new AlphaVantageFeed (this);
#endif
}

PriceWorkerTicker::~PriceWorkerTicker ()
{
  runflag = 0;
}

#define COPY_QSTR(len,cs,qs) \
    strncpy(cs, qs.toUtf8().constData(), len-1); \
    cs[len-1] = '\0'

void PriceWorkerTicker::process()
{
  const int sleepms = 50;
  TickerPrices prices;
  TickerPrice tmp;
  RTPrice fprice;
  qint32 counter = 0;

  state = 1;
  while (runflag.fetchAndAddAcquire (0) == 1)
  {
    if (counter == 0)
    {
      QStringList lsymbol, lfeed;
      qint32 max;

      if (gDatabase->loadTickerSymbols (lsymbol, lfeed) != CG_ERR_OK)
      {
          qDebug("PriceWorkerTicker: loadTickerSymbols error");
      }

      max = lsymbol.size ();

      prices.clear();
      prices.reserve(max);

      for (qint32 i = 0; i < max && runflag.fetchAndAddAcquire (0); i ++)
      {
        if (runflag.fetchAndAddAcquire (0) == 1)
        {
#ifdef SIMULATE_FEED
          static const char* _pct[4] = { "", "1.00%", "-0.70%", "0" };
          COPY_QSTR(8, tmp.symbol, lsymbol.at(i));
          strcpy(tmp.price, "666.00");
          strcpy(tmp.prcchange, _pct[i & 3]);
          prices.push_back( tmp );
#else
          fprice.price.clear();
          switch( InstrumentDatabase::feedSource( lfeed.at(i) ) )
          {
            case SourceNone:
              break;
            case SourceYahoo:
              yfeed->getRealTimePrice(lsymbol.at(i), fprice, YahooFeed::HTTP);
              break;
            case SourceIEX:
              efeed->getRealTimePrice(lsymbol.at(i), fprice);
              break;
            case SourceAlphaVantage:
              afeed->getRealTimePrice(lsymbol.at(i), fprice,
                                    AlphaVantageFeed::CSV);
              break;
          }
          if( ! fprice.price.isEmpty() )
          {
            COPY_QSTR( 8, tmp.symbol, fprice.symbol);
            COPY_QSTR( 8, tmp.price, fprice.price);
            COPY_QSTR(10, tmp.prcchange, fprice.prcchange);
            prices.push_back( tmp );
          }
#endif
        }
      }

      if (runflag.fetchAndAddAcquire (0) == 1)
      {
        // The list should get copied by Qt::QueuedConnection.
        emit updatePrices(prices);
      }
    }

    if (runflag.fetchAndAddAcquire (0) == 1)
    {
      Sleeper::msleep(sleepms);
      counter += sleepms;
      if (counter >= (Application_Options->nettimeout * 2200))
        counter = 0;
    }
  }

  state = 0;
}

void PriceWorkerTicker::terminate () NOEXCEPT
{
  if (state.fetchAndAddAcquire (0) == 0)
    return;

  runflag = 0;
}


//----------------------------------------------------------------------------
// PriceUpdater


static void _startWorker( QObject* worker, QThread* thread )
{
  worker->moveToThread(thread);
  worker->connect(thread, SIGNAL(started()), SLOT(process()));
  thread->start();
  thread->setPriority (QThread::LowestPriority);
}

PriceUpdater::PriceUpdater(QString symbol, QString feed, QTACObject *parent) :
    QObject(parent)
{
  worker = new PriceWorker (symbol, feed);
  tickerworker = nullptr;

  if (parent)
  {
    parent->onlineprice = true;
    worker->setParentObject (parent);
  }

  _startWorker( worker, &thread );
}

PriceUpdater::PriceUpdater(StockTicker *parent) :
    QObject(parent)
{
  worker = nullptr;
  tickerworker = new PriceWorkerTicker;

  connect(tickerworker, SIGNAL(updatePrices(TickerPrices)),
          parent, SLOT(updatePrices(TickerPrices)), Qt::QueuedConnection);

  _startWorker( tickerworker, &thread );
}

PriceUpdater::~PriceUpdater ()
{
  if (worker)
  {
    worker->terminate ();
    thread.quit ();
    thread.wait ();
    delete worker;
  }
  else if( tickerworker )
  {
    tickerworker->terminate ();
    thread.quit ();
    thread.wait ();
    delete tickerworker;
  }
}


//----------------------------------------------------------------------------
// PriceWorker


PriceWorker::PriceWorker (QString symbol, QString feed) :
    yfeed(nullptr), efeed(nullptr), afeed(nullptr)
{
  parentObject = nullptr;
  state = 0;
  runflag = 1;
  source = InstrumentDatabase::feedSource(feed);
  switch( source )
  {
    case SourceYahoo:
      yfeed = new YahooFeed (this);
      break;
    case SourceIEX:
      efeed = new IEXFeed (this);
      break;
    case SourceAlphaVantage:
      afeed = new AlphaVantageFeed (this);
      break;
    default:
      runflag = 0;
      break;
  }
  this->symbol = symbol;
}

PriceWorker::~PriceWorker ()
{
  runflag = 0;
}

// process slot
void PriceWorker::process()
{
  const int sleepms = 50;
  CG_ERR_RESULT err = CG_ERR_OK;
  qint32 counter = 0;

  state = 1;
  while (runflag.fetchAndAddAcquire (0) == 1)
  {
    if (counter == 0 && runflag.fetchAndAddAcquire (0))
    {
      switch( source )
      {
        case SourceYahoo:
          if (runflag.fetchAndAddAcquire(0))
            err = yfeed->getRealTimePrice(symbol, rtprice, YahooFeed::HTTP);
          break;
        case SourceIEX:
          if (runflag.fetchAndAddAcquire(0))
            err = efeed->getRealTimePrice(symbol, rtprice);
          break;
        case SourceAlphaVantage:
          if (runflag.fetchAndAddAcquire(0))
            err = afeed->getRealTimePrice(symbol, rtprice, AlphaVantageFeed::CSV);
          break;
        default:
          break;
      }

      if (err == CG_ERR_OK && runflag.fetchAndAddAcquire(0) &&
          parentObject != nullptr)
            parentObject->emitUpdateOnlinePrice (rtprice);
    }

    if (runflag.fetchAndAddAcquire (0) == 1)
    {
      Sleeper::msleep(sleepms);
      counter += sleepms;
      if (counter >= (Application_Options->nettimeout * 1100))
        counter = 0;
    }
  }
  state = 0;
}

// terminate slot
void PriceWorker::terminate () NOEXCEPT
{
  if (state.fetchAndAddAcquire (0) == 0)
    return;

  runflag = 0;
}
