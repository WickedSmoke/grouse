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

/// PriceWorkerTicker
// constructor
PriceWorkerTicker::PriceWorkerTicker ()
{
  parentObject = NULL;
  state = 0;
  runflag = 1;
#ifndef SIMULATE_FEED
  yfeed = new YahooFeed (this);
  efeed = new IEXFeed (this);
  afeed = new AlphaVantageFeed (this);
#endif
}

// destructor
PriceWorkerTicker::~PriceWorkerTicker ()
{
  runflag = 0;
}

// process slot
void
PriceWorkerTicker::process()
{
  const int sleepms = 50;
  RTPriceList lrtprice;
  qint32 counter = 0;

  state = 1;
  while (runflag.fetchAndAddAcquire (0) == 1)
  {
    if (counter == 0)
    {
      QStringList lsymbol, lfeed;
      qint32 max;

      if (gDatabase->loadTickerSymbols (lsymbol, lfeed) == CG_ERR_OK)
      {
        symbol = lsymbol;
        datafeed = lfeed;
      }

      max = symbol.size ();

      lrtprice.clear ();
      lrtprice.reserve (max);

      for (qint32 counter2 = 0; counter2 < max && runflag.fetchAndAddAcquire (0); counter2 ++)
      {
        RTPrice dummy;
        if (runflag.fetchAndAddAcquire (0) == 1)
        {
          lrtprice += dummy;
          if (runflag.fetchAndAddAcquire (0) == 1)
          {
#ifdef SIMULATE_FEED
            RTPrice& rp = lrtprice[counter2];
            rp.symbol = symbol.at(counter2);
            rp.price  = QStringLiteral("666.00");
#else
            if (datafeed.at (counter2).toUpper () == QLatin1String ("YAHOO"))
              yfeed->getRealTimePrice (symbol.at (counter2), lrtprice[counter2], YahooFeed::HTTP);
            else if (datafeed.at (counter2).toUpper () == QLatin1String ("IEX"))
              efeed->getRealTimePrice (symbol.at (counter2), lrtprice[counter2]);
            else if (datafeed.at (counter2).toUpper () == QLatin1String ("ALPHAVANTAGE"))
              afeed->getRealTimePrice (symbol.at (counter2), lrtprice[counter2], AlphaVantageFeed::CSV);
#endif
          }
        }
      }

      if (runflag.fetchAndAddAcquire (0) == 1)
      {
        for (qint32 counter3 = 0; counter3 < lrtprice.size (); counter3 ++)
        {
          if (lrtprice[counter3].price.toFloat () == 0)
            lrtprice.removeAt(counter3);
        }
        rtprice = lrtprice;
        if (parentObject != NULL) parentObject->emitUpdateTicker (rtprice);
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

// terminate slot
void
PriceWorkerTicker::terminate () NOEXCEPT
{
  if (state.fetchAndAddAcquire (0) == 0)
    return;

  runflag = 0;
}

/// PriceUpdater
// constructor
PriceUpdater::PriceUpdater (QString symbol, QString feed, QTACObject *parent)
{
  if (parent != NULL)
    setParent (parent);

  tickerworker = NULL;
  worker = new PriceWorker (symbol, feed);
  if (parent != NULL)
  {
    worker->setParentObject (parent);
    parent->onlineprice = true;
  }
  worker->moveToThread(&thread);
  connect(&thread, SIGNAL(started()), worker, SLOT(process()));
  thread.start();
  thread.setPriority (QThread::LowestPriority);
}

PriceUpdater::PriceUpdater (StockTicker *parent)
{
  if (parent != NULL)
  {
    setParent (parent);
    setObjectName (QStringLiteral ("PriceUpdater"));
  }

  worker = NULL;
  tickerworker = NULL;

  tickerworker = new PriceWorkerTicker ();
  if (parent != NULL)
    tickerworker->setParentObject (parent);

  tickerworker->moveToThread(&thread);
  connect(&thread, SIGNAL(started()), tickerworker, SLOT(process()));
  thread.start();
  thread.setPriority (QThread::LowestPriority);
}

// destructor
PriceUpdater::~PriceUpdater ()
{
  if (worker != NULL)
  {
    worker->terminate ();
    thread.quit ();
    thread.wait ();
    delete worker;
  }

  if (tickerworker != NULL)
  {
    tickerworker->terminate ();
    thread.quit ();
    thread.wait ();
    delete tickerworker;
  }
}


//----------------------------------------------------------------------------


// constructor
PriceWorker::PriceWorker (QString symbol, QString feed)
{
  parentObject = NULL;
  state = 0;
  runflag = 1;
  if (feed == QLatin1String ("YAHOO"))
    yfeed = new YahooFeed (this);
  else if (feed == QLatin1String ("IEX"))
    efeed = new IEXFeed (this);
  else if (feed == QLatin1String ("ALPHAVANTAGE"))
    afeed = new AlphaVantageFeed (this);
  else
    runflag = 0;
  datafeed = feed;
  this->symbol = symbol;

}

// destructor
PriceWorker::~PriceWorker ()
{
  runflag = 0;
}

// process slot
void
PriceWorker::process()
{
  const int sleepms = 50;
  CG_ERR_RESULT result = CG_ERR_OK;
  qint32 counter = 0;

  state = 1;
  while (runflag.fetchAndAddAcquire (0) == 1)
  {
    if (counter == 0 && runflag.fetchAndAddAcquire (0))
    {
      if (datafeed.toUpper () == QLatin1String ("YAHOO") && runflag.fetchAndAddAcquire (0))
        result = yfeed->getRealTimePrice (symbol, rtprice, YahooFeed::HTTP);
      else
      if (datafeed.toUpper () == QLatin1String ("IEX") && runflag.fetchAndAddAcquire (0))
        result = efeed->getRealTimePrice (symbol, rtprice);
      else
      if (datafeed.toUpper () == QLatin1String ("ALPHAVANTAGE") &&
          runflag.fetchAndAddAcquire (0))
        result = afeed->getRealTimePrice (symbol, rtprice, AlphaVantageFeed::CSV);

      if (result == CG_ERR_OK && runflag.fetchAndAddAcquire (0) && parentObject != NULL)
        if (parentObject != NULL) parentObject->emitUpdateOnlinePrice (rtprice);
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
void
PriceWorker::terminate () NOEXCEPT
{
  if (state.fetchAndAddAcquire (0) == 0)
    return;

  runflag = 0;
}
