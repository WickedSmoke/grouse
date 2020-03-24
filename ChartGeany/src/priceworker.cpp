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

#include "priceworker.h"

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
