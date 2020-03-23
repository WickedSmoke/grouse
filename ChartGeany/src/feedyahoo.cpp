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

/* Possible solutions
   https://query2.finance.yahoo.com/v10/finance/quoteSummary/IBM?modules=financialData
   https://query1.finance.yahoo.com/v7/finance/quote?symbols=IBM
*/

#include <QApplication>
#include <QByteArray>
#include <QTextStream>
#include <QTemporaryFile>
#include <QDateTime>
#include "common.h"
#include "feedyahoo.h"
#include "netservice.h"

static QString cookie (""), crumb ("");

// constructor
YahooFeed::YahooFeed (QObject *parent)
{
  tableName = Currency = symbolName = QStringLiteral ("");
  if (parent != NULL)
    setParent (parent);
}

// destructor
YahooFeed::~YahooFeed ()
{

  return;
}

// validate symbol
bool
YahooFeed::validSymbol (QString symbol)
{
  for (qint32 counter = 0, max = symbol.size ();
       counter < max; counter ++)
  {
    if (!((symbol[counter] >= 'A' && symbol[counter] <= 'Z') ||
          (symbol[counter] >= 'a' && symbol[counter] <= 'z') ||
          (symbol[counter] >= '0' && symbol[counter] <= '9') ||
          symbol[counter] == '.' || symbol[counter] == '='  ||
          symbol[counter] == '^' || symbol[counter] == '-'))
      return false;
  }

  return true;
}

// return symbol check URL
// eg: http://finance.yahoo.com/d/quotes.csv?s=IBM&f=nx
// yql:
// select * from csv where
// url='http://download.finance.yahoo.com/d/quotes.csv?s=IBM&f=nx'
// and columns='name,exchange'
QString
YahooFeed::symbolURL (QString symbol)
{
  return symbolURLjson (symbol);
/* Keep it here
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://finance.yahoo.com/d/quotes.csv?s=");
  urlstr += symbol;
  urlstr += QStringLiteral ("&f=nxc4");
  return urlstr;
*/
}

// https://query1.finance.yahoo.com/v7/finance/quote?symbols=IBM
QString
YahooFeed::symbolURLjson (QString symbol)
{
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://query1.finance.yahoo.com/v7/finance/quote?symbols=");
  urlstr += symbol;
  return urlstr;
}


// http://finance.yahoo.com/d/quotes.csv?s=RIO.AX&f=c4
QString
YahooFeed::symbolCurrencyURL (QString symbol)
{
  return symbolURL (symbol);
/* Keep it here
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://finance.yahoo.com/d/quotes.csv?s=");
  urlstr += symbol;
  urlstr += QStringLiteral ("&f=c4");
  return urlstr;
*/
}

// return symbol statistics URL from http
// eg: http://finance.yahoo.com/d/quotes.csv?s=IBM&f=b4j1j4rr5ye7e8ep5p6
// dividends
// eg: http://real-chart.finance.yahoo.com/table.csv?s=ABB&g=v
QString
YahooFeed::symbolStatsURL (QString symbol)
{
  return symbolURL (symbol);
/* Keep it here
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://finance.yahoo.com/d/quotes.csv?s=");
  urlstr += symbol;
  urlstr += QStringLiteral ("&f=b4j1j4rr5ye7e8ep5p6");
  return urlstr;
*/
}

// return symbol statistics URL from yql
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3DIBM%26f%3Db4j1j4rr5ye7e8ep5p6%26e%3D.csv'%20and%20columns%20%3D%20'bookvalue%2Cmarketcap%2Cebitda%2Cpe%2Cpeg%2Cyield%2Cepscy%2Cepsny%2Ces%2Cps%2Cpbv'&format=json&callback=
QString
YahooFeed::symbolStatsURLyql (QString symbol)
{
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D");
  urlstr += symbol;
  urlstr += QStringLiteral ("%26f%3Db4j1j4rr5ye7e8ep5p6%26e%3D.csv'%20and%20columns%20%3D%20'bookvalue%2Cmarketcap%2Cebitda%2Cpe%2Cpeg%2Cyield%2Cepscy%2Cepsny%2Ces%2Cps%2Cpbv'&format=json&callback=");
  return urlstr;
}

// return historical download URL
// eg:
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=d&ignore=.csv DAY
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=w&ignore=.csv WEEK
// http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=m&ignore=.csv MONTH
//
// **** YQL Syntax ****
// select * from csv where url='http://ichart.finance.yahoo.com/table.csv?s=GE&a=0&b=1&c=1960&d=0&e=31&f=2010&g=d'
// **** YQL URL ****
//
// YQL XML output for one year
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.historicaldata%20where%20symbol%20in%20%28%27GE%27%29%20and%20startDate%20=%20%271962-01-01%27%20and%20endDate%20=%20%271962-12-31%27&diagnostics=true&env=store://datatables.org/alltableswithkeys
// YQL JSON output
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D%27http%3A%2F%2Freal-chart.finance.yahoo.com%2Ftable.csv%3Fs%3DIBM%26d%3D9%26e%3D22%26f%3D2014%26g%3Dd%26a%3D0%26b%3D2%26c%3D1962%26ignore%3D.csv%27&format=json&callback=
// *** New Yahoo Historical Data API
// curl 'https://query1.finance.yahoo.com/v7/finance/download/%5EGSPC?period1=1492463852&period2=1495055852&interval=1d&events=history&crumb=XXXXXXX' -H 'user-agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0)' -H 'cookie: B=YYYYYY;' -H 'referer: https://finance.yahoo.com/quote/%5EGSPC/history?p=%5EGSPC'
QString
YahooFeed::downloadURL (QString symbol, QString timeframe, QString crumb)
{
  QString downstr = QStringLiteral ("");
  QDateTime today (QDate::currentDate()),
            startdate (QDate(1970, 1, 1));

  if (symbol.size () == 0)
    return downstr;

  downstr = QStringLiteral ("https://query1.finance.yahoo.com/v7/finance/download/") % symbol %
            QStringLiteral ("?period1=") % QString::number (startdate.toMSecsSinceEpoch() / 1000) %
            QStringLiteral ("&period2=") %
            QString::number (today.toMSecsSinceEpoch() / 1000) %
            QStringLiteral ("&interval=");

  if (timeframe == QLatin1String ("DAY"))
    downstr += QStringLiteral ("1d");
  else if (timeframe == QLatin1String ("WEEK"))
    downstr += QStringLiteral ("1w");
  else if (timeframe == QLatin1String ("MONTH"))
    downstr += QStringLiteral ("1m");
  else
    return QStringLiteral ("");

  downstr += "&events=history&crumb=" % crumb;
  return downstr;

/* Old API
  QString downstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return downstr;

  downstr = QStringLiteral ("https://ichart.finance.yahoo.com/table.csv?s=") % symbol %
            QStringLiteral ("&a=0&b=1&c=1950&d=") % Month % QStringLiteral ("&e=") %
            Day % QStringLiteral ("&f=") % Year % QStringLiteral ("&g=");

  if (timeframe == QLatin1String ("DAY"))
    downstr += QStringLiteral ("d");
  else if (timeframe == QLatin1String ("WEEK"))
    downstr += QStringLiteral ("w");
  else if (timeframe == QLatin1String ("MONTH"))
    downstr += QStringLiteral ("m");
  else
    return QStringLiteral ("");

  downstr += QStringLiteral ("&ignore=.csv");
  return downstr;
*/
}

// return real time price URL using http api
// eg:
// http://finance.yahoo.com/d/quotes.csv?s=RIO.AX&f=l1c6p2
QString
YahooFeed::realTimePriceURL (QString symbol)
{
  return symbolURL (symbol);
/*  Keep it here
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://finance.yahoo.com/d/quotes.csv?s=") % symbol %
           QStringLiteral ("&f=l1c1p2d1t1ohgv");
  return urlstr;
*/
}

// return real time price URL using yql api
// eg:
// http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D'http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3DGE%26f%3Dsl1d1t1c1ohgvp2%26e%3D.csv'%20and%20columns%3D'symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Ccol1%2Chigh%2Clow%2Ccol2%2Cpercent'&format=json&callback=
QString
YahooFeed::realTimePriceURLyql (QString symbol)
{
  QString urlstr = QStringLiteral ("");

  if (symbol.size () == 0)
    return urlstr;

  urlstr = QStringLiteral ("https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20csv%20where%20url%3D%27http%3A%2F%2Fdownload.finance.yahoo.com%2Fd%2Fquotes.csv%3Fs%3D");
  urlstr += symbol;
  urlstr += QStringLiteral ("%26f%3Dsl1d1t1c1ohgvp2%26e%3D.csv%27%20and%20columns%3D%27symbol%2Cprice%2Cdate%2Ctime%2Cchange%2Copen%2Chigh%2Clow%2Cvolume%2Cpercent%27&format=json&callback=");
  return urlstr;
}

// return update URL
QString
YahooFeed::updateURL (QString symbol, QString timeframe, QString datefrom)
{
  QString updstr = "", fromYear, fromMonth, fromDay;
  QStringList column;

  if (symbol.size () == 0)
    return updstr;

  column = datefrom.split(QStringLiteral ("-"), QString::KeepEmptyParts);
  fromYear = column[0];
  fromMonth = QStringLiteral ("0");
  fromDay = column[2];

  updstr = QStringLiteral ("http://ichart.yahoo.com/table.csv?s=");
  updstr += symbol;
  updstr += "&a=" % fromMonth % "&b=" % fromDay +"&c=" % fromYear % "&d=" % Month % "&e=" % Day % "&f=" % Year % "&g=";

  if (timeframe == "DAY")
    updstr += "d";
  else if (timeframe == "WEEK")
    updstr += "w";
  else if (timeframe == "MONTH")
    updstr += "m";
  else
    return "";

  updstr += "&ignore=.csv";
  return updstr;
}

// get real time price
CG_ERR_RESULT
YahooFeed::getRealTimePrice (QString symbol, RTPrice & rtprice, YAHOO_API api)
{
  Q_UNUSED (api)

  return getRealTimePricejson (symbol, rtprice);
/*
  QTemporaryFile tempFile;      // temporary file
  RTPrice realtimeprice;        // real time price
  QTextStream in;
  QString url, line;
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  if (symbol.contains (QStringLiteral ("^")))
    api = HTTP;

  if (api == HTTP)
    url = realTimePriceURL (symbol);
  else
    url = realTimePriceURLyql (symbol);

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto getRealTimePrice_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  result = netservice->httpGET (url, tempFile, NULL);
  if (result != CG_ERR_OK)
    goto getRealTimePrice_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
    if (api == HTTP)
    {
      QStringList token;
      line.remove (10);
      line.remove (13);
      line = line.simplified ();
      token.clear ();
      token = line.split(QStringLiteral (","), QString::KeepEmptyParts);

      for (qint32 counter = 0; counter < token.size (); counter ++)
      {
        token[counter] = token[counter].simplified ();
        token[counter].remove ('"');
        token[counter].remove ('+');
      }

      if (token.size () < 3)
        result = CG_ERR_INVALID_DATA;
      else
      {
        QStringList yyyymmdd;
        realtimeprice.price = token[0]; //QString::number (token[0].toFloat (), 'f', fracdig (token[0].toFloat ()));
        realtimeprice.change = token[1]; //QString::number (token[1].toFloat (), 'f', fracdig (token[0].toFloat ()));
        realtimeprice.prcchange = token[2];
        realtimeprice.date = token[3];
        yyyymmdd = realtimeprice.date.split (QStringLiteral ("/"), QString::KeepEmptyParts);
        if (yyyymmdd.size () < 3)
          realtimeprice.date = QStringLiteral ("0000-00-00");
        else
        {
          if (yyyymmdd[0].size () == 1)
            yyyymmdd[0] = QStringLiteral ("0") % yyyymmdd[0];
          if (yyyymmdd[1].size () == 1)
            yyyymmdd[1] = QStringLiteral ("0") % yyyymmdd[1];
          realtimeprice.date = yyyymmdd[2] % QStringLiteral ("-") %
                               yyyymmdd[0] % QStringLiteral ("-") % yyyymmdd[1];
        }
        realtimeprice.time = token[4];
        realtimeprice.open = token[5];
        realtimeprice.high = token[6];
        realtimeprice.low = token[7];
        realtimeprice.volume = token[8];
      }
    }
    else
    {
      QStringList node, value;
      if (json_parse (line, &node, &value, NULL))
      {
        for (qint32 counter = 0; counter < node.size (); counter ++)
        {
          if (node.at (counter).toLower () == QLatin1String ("price"))
            realtimeprice.price = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();

          if (node.at (counter).toLower () == QLatin1String ("change"))
            realtimeprice.change = value[counter].remove ("+");

          if (node.at (counter).toLower () == QLatin1String ("percent"))
          {
            realtimeprice.prcchange = value[counter].simplified();
            realtimeprice.prcchange = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("open"))
          {
            realtimeprice.open = value[counter].simplified();
            realtimeprice.open = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("high"))
          {
            realtimeprice.high = value[counter].simplified();
            realtimeprice.high = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("low"))
          {
            realtimeprice.low = value[counter].simplified();
            realtimeprice.low = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("volume"))
          {
            realtimeprice.volume = value[counter].simplified();
            realtimeprice.volume = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("date"))
          {
            QStringList datepart;
            QString yyyymmdd;

            datepart = value[counter].split (QStringLiteral (" "), QString::KeepEmptyParts);
            if (datepart.size () != 3)
            {
              result = CG_ERR_INVALID_DATA;
              goto getRealTimePrice_end;
            }
            yyyymmdd = datepart[2] % QStringLiteral ("-") %
                       QString("%1").arg(datepart[0].toInt (), 2, 10, QChar('0')) % QStringLiteral ("-") %
                       QString("%1").arg(datepart[1].toInt (), 2, 10, QChar('0'));
            realtimeprice.date = yyyymmdd;
          }

          if (node.at (counter).toLower () == QLatin1String ("time"))
            realtimeprice.time = value[counter].simplified();
        }
      }
      else
        result = CG_ERR_INVALID_DATA;
    }
  }
  else
    result = CG_ERR_INVALID_DATA;

getRealTimePrice_end:
  setGlobalError(result, __FILE__, __LINE__);
  realtimeprice.symbol = symbol;
  realtimeprice.feed = QStringLiteral ("YAHOO");
  rtprice = realtimeprice;
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  if (result == CG_ERR_OK)
    updatePrice (rtprice);
  return result;
*/
}

// get real time price
CG_ERR_RESULT
YahooFeed::getRealTimePricejson (QString symbol, RTPrice & rtprice)
{
  QTemporaryFile tempFile;      // temporary file
  RTPrice realtimeprice;        // real time price
  QTextStream in;
  QString url, line;
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  url = realTimePriceURL (symbol);

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto getRealTimePrice_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  result = netservice->httpGET (url, tempFile, NULL);
  if (result != CG_ERR_OK)
    goto getRealTimePrice_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
      QStringList node, value;
      if (json_parse (line, &node, &value, NULL))
      {
        for (qint32 counter = 0; counter < node.size (); counter ++)
        {
          if (node.at (counter).toLower () == QLatin1String ("regularmarketprice"))
          {
            realtimeprice.price = value[counter].remove ("+");
            realtimeprice.price = QString::number (realtimeprice.price.toDouble (), 'g', 8);
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketchange"))
          {
            realtimeprice.change = value[counter].remove ("+");
            realtimeprice.change = QString::number (realtimeprice.change.toDouble (), 'g', 4);
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketchangepercent"))
          {
            realtimeprice.prcchange = value[counter].simplified().remove ("+");
            realtimeprice.prcchange =
              QString::number (realtimeprice.prcchange.toFloat (), 'f', 2) % QStringLiteral ("%");
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketopen"))
          {
            realtimeprice.open = value[counter].simplified();
            realtimeprice.open = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketdayhigh"))
          {
            realtimeprice.high = value[counter].simplified();
            realtimeprice.high = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketdaylow"))
          {
            realtimeprice.low = value[counter].simplified();
            realtimeprice.low = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarketvolume"))
          {
            realtimeprice.volume = value[counter].simplified();
            realtimeprice.volume = value[counter].remove ("+");
          }

          if (node.at (counter).toLower () == QLatin1String ("regularmarkettime"))
          {
            QDateTime timestamp;
            const unsigned int s = value.at (counter).left (10).toUInt ();
            timestamp.setTime_t(s);
            realtimeprice.date = timestamp.toString("yyyy-MM-dd");
            realtimeprice.time = timestamp.toString("hh:mm:ss");
          }
        }
      }
      else
        result = CG_ERR_INVALID_DATA;

  }
  else
    result = CG_ERR_INVALID_DATA;

getRealTimePrice_end:
  setGlobalError(result, __FILE__, __LINE__);
  realtimeprice.symbol = symbol;
  realtimeprice.feed = QStringLiteral ("YAHOO");
  rtprice = realtimeprice;
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  if (result == CG_ERR_OK)
    updatePrice (rtprice);
  return result;
}

// download statistics
CG_ERR_RESULT
YahooFeed::downloadStats (QString symbol, YAHOO_API api)
{
  Q_UNUSED (api)

  return downloadStatsjson (symbol);
/*
  QTemporaryFile tempFile;      // temporary file
  QTextStream in;
  QString url, line;
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  if (symbol.contains (QStringLiteral ("^")))
    api = HTTP;

  if (api == HTTP)
    url = symbolStatsURL (symbol);
  else
    url = symbolStatsURLyql (symbol);

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto downloadStats_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  result = netservice->httpGET (url, tempFile, NULL);
  if (result != CG_ERR_OK)
    goto downloadStats_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
    if (api == HTTP)
    {
      QStringList token;
      line.remove (10);
      line.remove (13);
      line = line.simplified ();
      token.clear ();
      token = line.split(QStringLiteral (","), QString::KeepEmptyParts);
      for (qint32 counter = 0; counter < token.size (); counter ++)
      {
        token[counter] = token[counter].simplified ();
        token[counter].remove ('"');
      }

      BookValue = token[0].trimmed ();
      MarketCap = token[1].trimmed ();
      EBITDA = token[2].trimmed ();
      PE = token[3].trimmed ();
      PEG = token[4].trimmed ();
      Yield = token[5].trimmed ();
      EPScy = token[6].trimmed ();
      EPSny = token[7].trimmed ();
      ES = token[8].trimmed ();
      PS = token[9].trimmed ();
      PBv = token[10].trimmed ();
    }
    else
    {
      QStringList node, value;
      if (json_parse (line, &node, &value, NULL))
      {
        for (qint32 counter = 0; counter < node.size (); counter ++)
        {
          if (node.at (counter) == QLatin1String ("bookvalue"))
            BookValue = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("marketcap"))
            MarketCap = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("ebitda"))
            EBITDA = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("pe"))
            PE = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("peg"))
            PEG = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("yield"))
            Yield = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("epscy"))
            EPScy = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("epsny"))
            EPSny = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("es"))
            ES = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("ps"))
            PS = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
          else if (node.at (counter) == QLatin1String ("pbv"))
            PBv = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        }
      }
      else
        result = CG_ERR_INVALID_DATA;
    }
  }

downloadStats_end:
  setGlobalError(result, __FILE__, __LINE__);
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return  result;
*/
}

// download statistics
CG_ERR_RESULT
YahooFeed::downloadStatsjson (QString symbol)
{
  QTemporaryFile tempFile;      // temporary file
  QTextStream in;
  QString url, line;
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  url = symbolStatsURL (symbol);

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    goto downloadStats_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  result = netservice->httpGET (url, tempFile, NULL);
  if (result != CG_ERR_OK)
    goto downloadStats_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
    QStringList node, value;
    if (json_parse (line, &node, &value, NULL))
    {
      for (qint32 counter = 0; counter < node.size (); counter ++)
      {
        if (node.at (counter) == QLatin1String ("bookValue"))
          BookValue = QString("%1").arg (value.at (counter).toDouble (), 0, 'f', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("marketCap"))
          MarketCap = QString("%1").arg (value.at (counter).toFloat (), 0, 'f', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("ebitda"))
          EBITDA = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("trailingPE"))
          PE = QString("%1").arg (value.at (counter).toFloat (), 0, 'f', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("peg"))
          PEG = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("trailingAnnualDividendYield"))
          Yield = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("epsTrailingTwelveMonths"))
          EPScy = QString("%1").arg (value.at (counter).toDouble (), 0, 'f', 5).simplified ();
        else if (node.at (counter) == QLatin1String ("epsForward"))
          EPSny = QString("%1").arg (value.at (counter).toDouble (), 0, 'f', 5).simplified ();
        else if (node.at (counter) == QLatin1String ("es"))
          ES = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("ps"))
          PS = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
        else if (node.at (counter) == QLatin1String ("priceToBook"))
          PBv = QString("%1").arg (value.at (counter).toFloat (), 0, 'g', 2).simplified ();
      }
    }
    else
      result = CG_ERR_INVALID_DATA;

  }

downloadStats_end:
  setGlobalError(result, __FILE__, __LINE__);
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return  result;
}


// check if symbol exists
bool
YahooFeed::symbolExistence (QString & symbol, QString & name, QString & market,
                            QString & currency)
{

  return symbolExistencejson (symbol, name, market, currency);
/* Keep it here
  QTemporaryFile tempFile;      // temporary file
  QTextStream in;
  QString urlstr, line;
  QStringList token;
  NetService *netservice = NULL;
  CG_ERR_RESULT ioresult = CG_ERR_OK;
  bool result = false;

  symbol = symbol.trimmed ();
  if (!validSymbol (symbol))
    goto symbolExistence_end;

  urlstr = symbolURL (symbol);
  if (urlstr.size () == 0)
    goto symbolExistence_end;

  Symbol = symbol;
  symbolName = name;

  // open temporary file
  if (!tempFile.open ())
  {
    ioresult = CG_ERR_CREATE_TEMPFILE;
    goto symbolExistence_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  ioresult = netservice->httpGET (urlstr, tempFile, NULL);
  if (ioresult != CG_ERR_OK)
    goto symbolExistence_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
    line.remove (10);
    line.remove (13);
    line.replace (QStringLiteral (","), QStringLiteral (" "));
    line = line.simplified ();
    token.clear ();
    token = line.split(QStringLiteral ("\""), QString::SkipEmptyParts);
    if (token.size () < 4)
      goto symbolExistence_end;
    token[0].replace (QStringLiteral (","), QStringLiteral (" "));
    symbolName = token[0];
    symbolName = symbolName.simplified ();
    if (symbolName.contains (QStringLiteral ("Missing")))
      goto symbolExistence_end;

    if (symbolName == Symbol)
      goto symbolExistence_end;

    Market = QStringLiteral ("");
    token[2].remove (QChar ('"'), Qt::CaseInsensitive);
    if (token[2].size () != 0)
      Market = token [2].trimmed ();

    Currency = QStringLiteral ("");
    if (token.size () > 4)
    {
      token[4].remove (QChar ('"'), Qt::CaseInsensitive);
      if (token[4].size () != 0)
        Currency = token [4].trimmed ();
    }

    market = Market;
    name = symbolName;
    currency = Currency;
    result = true;
  }
  else
    result = false;

symbolExistence_end:
  setGlobalError(ioresult, __FILE__, __LINE__);

  if (result == false)
    Symbol = QStringLiteral ("");

  tempFile.close ();
  if (netservice != NULL)
    delete netservice;

  return result;
*/
}

// check if symbol exists from json
bool
YahooFeed::symbolExistencejson (QString & symbol, QString & name, QString & market,
                            QString & currency)
{
  QTemporaryFile tempFile;      // temporary file
  QTextStream in;
  QString urlstr, line;
  QStringList token;
  NetService *netservice = NULL;
  CG_ERR_RESULT ioresult = CG_ERR_OK;
  bool result = false;

  symbol = symbol.trimmed ();
  if (!validSymbol (symbol))
    goto symbolExistence_end;

  urlstr = symbolURL (symbol);
  if (urlstr.size () == 0)
    goto symbolExistence_end;

  Symbol = symbol;
  symbolName = name;

  // open temporary file
  if (!tempFile.open ())
  {
    ioresult = CG_ERR_CREATE_TEMPFILE;
    goto symbolExistence_end;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  ioresult = netservice->httpGET (urlstr, tempFile, NULL);
  if (ioresult != CG_ERR_OK)
    goto symbolExistence_end;

  in.setDevice (&tempFile);
  in.seek (0);
  line = in.readAll ();

  if (line.size () != 0)
  {
     QStringList node, value;

    if (json_parse (line, &node, &value, NULL))
    {
      symbolName = Market = Currency = QStringLiteral ("");
      for (qint32 counter = 0; counter < node.size (); counter ++)
      {
        if (node.at (counter) == QLatin1String ("shortName"))
          symbolName = value.at (counter).simplified();

        if (node.at (counter) == QLatin1String ("longName") &&
            symbolName == QLatin1String (""))
          symbolName = value.at (counter).simplified();

        if (node.at (counter) == QLatin1String ("exchange"))
          Market = value.at (counter).simplified();

        if (node.at (counter) == QLatin1String ("currency"))
          Currency = value.at (counter).simplified();

        if (node.at (counter) == QLatin1String ("financialCurrency") &&
            Currency == QLatin1String (""))
          Currency = value.at (counter).simplified();
      }

      name = symbolName.remove ("amp;");
      market = Market;
      currency = Currency;

      result = true;
    }
    else
      ioresult = CG_ERR_INVALID_DATA;
  }
  else
    result = false;

symbolExistence_end:
  setGlobalError(ioresult, __FILE__, __LINE__);

  if (result == false)
    Symbol = QStringLiteral ("");

  tempFile.close ();
  if (netservice != NULL)
    delete netservice;

  return result;
}

// get Yahoo's crumb cookie hash
QString
YahooFeed::getCrumb (const QString & namein)
{
  QFile tFile;
  QString ypage, result;

  tFile.setFileName (namein);
  if (!tFile.open (QIODevice::ReadOnly|QIODevice::Text))
    return QStringLiteral ("");

  QTextStream in (&tFile);
  ypage.reserve (1024*600);
  ypage = in.read (1024*600);
  int pos =
    ypage.indexOf (QStringLiteral ("\"CrumbStore\":{\"crumb\":\""), 0, Qt::CaseInsensitive) +
    QString ("\"CrumbStore\":{\"crumb\":\"").size ();
  tFile.close ();
  result = ypage.mid (pos, 11);
  return result;
}

// download historical data
CG_ERR_RESULT
YahooFeed::downloadData (QString symbol, QString timeframe, QString currency,
                         QString task, bool adjust)
{
  QTemporaryFile tempFile;      // temporary file
  QString url;
  NetService *netservice = NULL;
  CG_ERR_RESULT result = CG_ERR_OK;

  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (0);

  // check symbol existence
  if (symbol != Symbol)
  {
    if (!symbolExistence (symbol, entry.name, entry.market, currency))
    {
      result = GlobalError.fetchAndAddAcquire (0);
      if (result == CG_ERR_OK)
        result = CG_ERR_NOSYMBOL;
      return result;
    }
  }

  // open temporary file
  if (!tempFile.open ())
  {
    result = CG_ERR_CREATE_TEMPFILE;
    setGlobalError(result, __FILE__, __LINE__);
    return result;
  }
  tempFile.resize (0);

  netservice = new NetService (Application_Settings->options.nettimeout,
                               httpHeader(), this);
  // get cookie and crumb
  // if (cookie == QLatin1String (""))
  // {
    Cookies cookies;
    result = netservice->httpGET (QStringLiteral ("https://finance.yahoo.com/quote/") %
                                    symbol % QStringLiteral ("/history"),
                                    tempFile, &cookies);
    if (result != CG_ERR_OK)
    {
      delete netservice;
      return result;
    }

    crumb = getCrumb (tempFile.fileName ());
    tempFile.resize (0);

    foreach (const QNetworkCookie ncookie, cookies)
      if (QString (ncookie.name ()) == QLatin1String ("B"))
        cookie = QStringLiteral ("B=") % QString (ncookie.value ());
  // }
  // fill symbol entry
  entry.symbol = Symbol;
  entry.timeframe = timeframe;
  entry.csvfile = tempFile.fileName ();
  entry.source = QStringLiteral("YAHOO");
  entry.format = QStringLiteral("YAHOO CSV");
  entry.currency = currency;
  entry.name = symbolName;
  entry.market = Market;
  entry.adjust = adjust;
  entry.tablename = entry.symbol % QStringLiteral("_") %
                    entry.market % QStringLiteral("_") %
                    entry.source % QStringLiteral("_");

  entry.tablename += entry.timeframe;
  entry.tmptablename = QStringLiteral("TMP_") + entry.tablename;
  url = downloadURL (symbol, timeframe, crumb);
  entry.dnlstring = url;

  netservice->setCookie (cookie);
  result = netservice->httpGET (url, tempFile, NULL);
  if (result != CG_ERR_OK)
    goto downloadData_end;

  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (33);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10);

  if (tempFile.size () != 0)
  {
    result = downloadStats (Symbol,  YahooFeed::HTTP /* YahooFeed::YQL */);
    if (result != CG_ERR_OK)
      goto downloadData_end;

    if (GlobalProgressBar != NULL)
      GlobalProgressBar->setValue (50);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10);

    entry.BookValue = BookValue;
    entry.MarketCap = MarketCap;
    entry.EBITDA = EBITDA;
    entry.PE = PE;
    entry.PEG = PEG;
    entry.Yield = Yield;
    entry.EPScy = EPScy;
    entry.EPSny = EPSny;
    entry.ES = ES;
    entry.PS = PS;
    entry.PBv = PBv;

    if (GlobalProgressBar != NULL)
      GlobalProgressBar->setValue (66);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10);

    if (task == QLatin1String ("DOWNLOAD"))
      result = csv2sqlite (&entry, QStringLiteral ("CREATE"));
    else
      result = csv2sqlite (&entry, QStringLiteral ("UPDATE"));
  }

  if (GlobalProgressBar != NULL)
    GlobalProgressBar->setValue (100);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 10);

downloadData_end:
  tableName = entry.tablename;
  setGlobalError(result, __FILE__, __LINE__);
  tempFile.close ();
  if (netservice != NULL)
    delete netservice;
  return result;
}
