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

// member functions of QTAChart except
// constructor, destructors and event handlers

#include "qtachart_core.h"

/// Aa

/// Bb

/// Cc

/// Dd

/// Ee

/// Ff

/// Gg
// get chart's symbol database key
QString
QTAChart::getSymbolKey ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  return core->SymbolKey;
}

/// Hh

/// Ii

/// Jj

/// Kk

/// Ll

/*
 * unused. keep it here for possible future use
 *
// qSort callback for descending sort
static bool
descend (const QTAChartFrame & v1, const QTAChartFrame & v2) NOEXCEPT
{
  static QString ds1, ds2;
  ds1 = QString (v1.Date);
  ds2 = QString (v2.Date);

  if (ds1 != ds2)
    return ds1 > ds2;

  ds1 = QString (v1.Time);
  ds2 = QString (v2.Time);

  return ds1 > ds2;
}
*/

// load the frames
// callback for sqlite3_exec
static int
sqlcb_table_data (void *classptr, int argc, char **argv, char **column)
{
  TableDataClass *tdc = static_cast <TableDataClass *> (classptr);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    // symbol,  timeframe, description, adjusted, base, market, source
    if (colname == QLatin1String ("SYMBOL"))
      tdc->symbol = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("TIMEFRAME"))
      tdc->timeframe = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("DESCRIPTION"))
      tdc->name = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("ADJUSTED"))
      tdc->adjusted = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("BASE"))
      tdc->base = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("MARKET"))
      tdc->market = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("SOURCE"))
      tdc->source = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("LASTUPDATE"))
      tdc->lastupdate = QString (argv[counter]).toUpper ();
    if (colname == QLatin1String ("CURRENCY"))
      tdc->currency = QString (argv[counter]);
  }
  return 0;
}

/*
inline static qreal
MAX (qreal a, qreal b)
{
  if (a > b)
    return a;

  return b;
}

#define MAX(a,b)        (a>b?a:b)
*/

qreal
MAX (qreal a, qreal b, qreal c)
{
  int m = a;
  (void)((m < b) && (m = b)); //these are not conditional statements.
  (void)((m < c) && (m = c)); //these are just boolean expressions.
  return m;
}

void
QTAChart::loadFrames (QString tablename)
{
  const QString
  dash ("-"), D ("D: "), SP (" "), O (" O: "), C ("  C: "),
       H ("  H: "), L ("  L: ");
  TFClass TF;
  FrameVector frames;
  TableDataClass tdc;
  QString btext, SQLCommand;
  QStringList ymd;
  QTAChartFrame frame, haframe, prevframe;
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  qreal excess_drag_width = 0;
  qint32 nframes, startbar = 0;
  int rc;
  bool ok;

#ifdef DEBUG
  QElapsedTimer timer;
  timer.start();
#endif

  ResourceMutex->lock ();
  classError = CG_ERR_OK;
  // sqlite3_release_memory(33554432);

// save the current excess_drag_width
  for (qint32 counter = 0; counter < core->TIMEFRAME.size (); counter ++)
  {
    if (core->TIMEFRAME[counter].TFTablename == tablename)
    {
      excess_drag_width = core->TIMEFRAME[counter].TFExcess_Drag_Width;
      if (core->tfinit)
        startbar = core->TIMEFRAME[counter].TFStartBar;
    }
  }

// read tablename row from symbols_l2
  SQLCommand = QStringLiteral ("SELECT * FROM symbols_l2 WHERE KEY = '") %
               tablename % QStringLiteral ("';");
  rc =  selectfromdb (SQLCommand.toUtf8(),
                      sqlcb_table_data, static_cast <void *> (&tdc));
  if (rc != SQLITE_OK)
  {
    classError = CG_ERR_ACCESS_DATA;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }
  TF.TFTablename = tablename;

// read the frames from disc
  frames.reserve (32768);
  SQLCommand = QStringLiteral ("SELECT * FROM ") % tablename % QStringLiteral (" ORDER BY DATE DESC, TIME DESC;");
  rc =  selectfromdb (SQLCommand.toUtf8(),
                      sqlcb_dataframes, static_cast <void *> (&frames));
  if (rc != SQLITE_OK)
  {
    classError = CG_ERR_ACCESS_DATA;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }
/*
#ifdef DEBUG
  qDebug () << "Frames loaded in:" << timer.elapsed();
  timer.start();
#endif
*/
  if (frames.count () == 0)
  {
    classError = CG_ERR_NO_QUOTES;
    setGlobalError(classError, __FILE__, __LINE__);
    goto loadFrames_end;
  }

  // if market is London and longbp = true;
  if ((tdc.currency.trimmed ().left (3) == QLatin1String ("GBp") ||
       tdc.market.trimmed ().left (3) == QLatin1String ("LON")) &&
      Application_Settings->options.longbp)
  {
    int fc = frames.count ();
    for (qint32 counter = 0; counter < fc; counter ++)
    {
      frames[counter].High /= 100;
      frames[counter].Low /= 100;
      frames[counter].Open /= 100;
      frames[counter].Close /= 100;
      frames[counter].AdjClose /= 100;
    }
  }

// populate frame vector
  TF.TFStartBar = startbar;
  TF.TFExcess_Drag_Width = excess_drag_width;
  TF.HLOC.clear ();
  TF.HEIKINASHI.clear ();
  TF.TFName = tdc.timeframe;
  TF.TFSymbol = tdc.timeframe.left (1);
  TF.TFMarket = tdc.market;
  TF.TFCurrency = tdc.currency;
  nframes = frames.size ();

  for (qint32 counter = 0; counter < nframes; counter ++)
  {
    frame = frames.at (counter);

    ymd = QString (frame.Date).split(dash, QString::KeepEmptyParts);
    frame.year = frame.month = frame.day = 0;
    if (ymd.size () > 2)
    {
      frame.year = ymd.at (0).toUShort(&ok, 10);
      frame.month = ymd.at (1).toUShort(&ok, 10);
      frame.day = ymd.at (2).toUShort(&ok, 10);
    }

    btext =
      D %
      QString (frame.Date) %
      SP %
      QString (frame.Time) %
      O %
      QString::number (frame.Open, 'f', fracdig (frame.Open)) %
      C %
      QString::number (frame.Close, 'f', fracdig (frame.Close)) %
      H %
      QString::number (frame.High, 'f', fracdig (frame.High)) %
      L %
      QString::number (frame.Low, 'f', fracdig (frame.Low));

    frame.Text = btext;
    frames[counter] = frame;
  }

  core->reloaded = true;
  TF.HLOC = frames;
/*
#ifdef DEBUG
  qDebug () << "Candles completed in:" << timer.elapsed();
  timer.start();
#endif
*/
  if (nframes < 2)
  {
    core->TIMEFRAME += TF;
    goto loadFrames_end;
  }

// heikinashi
  /*
    TF.HEIKINASHI.prepend (TF.HLOC.at (nframes - 1)); // <-- ATTENTION: THIS IS VERY SLOW
  */
  TF.HEIKINASHI.reserve (nframes);
  prevframe = TF.HLOC.at (nframes - 1);
  for (qint32 counter = 0; counter < nframes; counter ++)
    TF.HEIKINASHI.append (prevframe);

  for (qint32 counter = nframes - 2; counter > -1; counter--)
  {
    frame = haframe = TF.HLOC.at (counter);

    haframe.Close = (frame.Open + frame.High + frame.Close + frame.Low) / 4;
    haframe.Open = (prevframe.Open + prevframe.Close) / 2;
    haframe.High = qMax (qMax(haframe.Close, haframe.Open), frame.High);
    haframe.Low =  qMin (qMin(haframe.Close, haframe.Open), frame.Low);

    btext =
      D %
      QString (haframe.Date) %
      SP %
      QString (haframe.Time) %
      O %
      QString::number (haframe.Open, 'f', fracdig (haframe.Open)) %
      C %
      QString::number (haframe.Close, 'f', fracdig (haframe.Close)) %
      H %
      QString::number (haframe.High, 'f', fracdig (haframe.High)) %
      L %
      QString::number (haframe.Low, 'f', fracdig (haframe.Low));

    haframe.Text = btext;
    /*
        TF.HEIKINASHI.prepend (haframe); // <-- ATTENTION: THIS IS VERY SLOW
    */
    TF.HEIKINASHI[counter] = haframe;
    prevframe  = haframe;
  }
/*
#ifdef DEBUG
  qDebug () << "HA completed in:" << timer.elapsed();
  timer.start();
#endif
*/
  for (qint32 counter = 0; counter < core->TIMEFRAME.size (); counter ++)
  {
    if (core->TIMEFRAME.at (counter).TFTablename == TF.TFTablename)
    {
      core->TIMEFRAME[counter] = TF;
      goto loadFrames_end;
    }
  }
  core->TIMEFRAME.append (TF);

loadFrames_end:

  ResourceMutex->unlock ();
/*
#ifdef DEBUG
  qDebug () << "Chart Creation completed in:" << timer.elapsed();
  qDebug () << "===========";
#endif
*/
  return;
}

// load data
void
QTAChart::loadData (QTAChartData data)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  QString textdata = QStringLiteral ("");

  textdata += QStringLiteral ("Book Value:  ") % data.bv % QStringLiteral ("\n\n") %
              QStringLiteral ("Market Cap:  ") % data.mc % QStringLiteral ("\n\n") %
              QStringLiteral ("EBITDA:  ") % data.ebitda % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Earnings:  ") % data.pe % QStringLiteral ("\n\n") %
              QStringLiteral ("PEG Ratio:  ") % data.peg % QStringLiteral ("\n\n") %
              QStringLiteral ("Dividend Yield:  ") % data.dy % QStringLiteral ("\n\n") %
              QStringLiteral ("EPS Current:  ") % data.epscurrent % QStringLiteral ("\n\n") %
              QStringLiteral ("EPS Next:  ") % data.epsnext % QStringLiteral ("\n\n") %
              QStringLiteral ("Earnings/Share:  ") % data.es % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Sales:  ") % data.ps % QStringLiteral ("\n\n") %
              QStringLiteral ("Price/Book:  ") % data.pbv % QStringLiteral ("\n\n");
  core->dataScr->setData (textdata);
}

/// Mm

/// Nn

/// Oo

/// Pp

/// Qq

/// Rr
// restore bottom text
void
QTAChart::restoreBottomText ()
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  core->restoreBottomText ();
}

/// Ss

// always redraw chart on/off
void
QTAChart::setAlwaysRedraw (bool boolean)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->always_redraw = boolean;
}

// set chart's symbol
void
QTAChart::setSymbol (QString symbol)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->Symbol = symbol;
}

// set chart's symbol key
void
QTAChart::setSymbolKey (QString key)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->SymbolKey = key;

  // load setting
  core->loadSettings ();
}

// set symbol's feed
void
QTAChart::setFeed (QString feed)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->Feed = feed;
}

// set chart's title
void
QTAChart::setTitle (QString title, QString subtitle)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  core->setTitle (title, subtitle);
}

// set the bottom text to custom string
void
QTAChart::setCustomBottomText (QString string)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));
  core->setCustomBottomText (string);
}

// set linear chart on/off
void
QTAChart::setLinear (bool boolean)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->setLinearScale (boolean);
}

// set chart volumes on/off
void
QTAChart::showVolumes (bool boolean)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->show_volumes = boolean;
}

// set chart grid on/off
void
QTAChart::showGrid (bool boolean)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->show_grid = boolean;
}

// set online price on/off
void
QTAChart::showOnlinePrice (bool boolean)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  core->show_onlineprice = boolean;
}

/// Tt

/// Uu

/// Vv

/// Ww

/// Xx

/// Wy

/// Zz
