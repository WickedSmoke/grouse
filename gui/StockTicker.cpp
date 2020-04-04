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

#include <QTextDocument>
#include "StockTicker.h"
#include "priceupdater.h"

#ifndef GUI_DESKTOP
#include "mainwindow.h"
#endif

// constructor
StockTicker::StockTicker (QWidget * parent):
  QGraphicsView (parent), timerId(0)
{
  newdata = false;
  tickerdata = NULL;
  tickerlabel = NULL;
  tickerspeed = Application_Options->scrollspeed;
  tickerstring = QStringLiteral ("");
  ticker_running = false;
  firstrun = true;

  setViewportUpdateMode (QGraphicsView::NoViewportUpdate);
  setCacheMode (QGraphicsView::CacheBackground);
  setAlignment (Qt::AlignLeft | Qt::AlignTop);

  QTCGraphicsScene *gs;
  gs = new QTCGraphicsScene (this);
  setScene (gs);

  gs->setItemIndexMethod (QTCGraphicsScene::NoIndex);
  gs->setBackgroundBrush (Qt::black);

  qRegisterMetaType<RTPriceList> ("RTPriceList");
  tickerdata = new PriceUpdater (this);

  tickerlabel = new QGraphicsTextItem;
  tickerlabel->setFont (QFont (DEFAULT_FONT_FAMILY));
  tickerlabel->setHtml (QStringLiteral ("<td bgcolor=black><font size = 5 color=white>Please wait...</font></td>"));
  tickerlabel->setVisible (true);
  gs->addItem (tickerlabel);
  tickerlabel->setPos (0, -5);
}

// destructor
StockTicker::~StockTicker ()
{
  if (timerId)
    killTimer(timerId);

  delete tickerdata;
  delete tickerlabel;
}

// stock ticker data updates and advance
void
StockTicker::ticker ()
{
  static qreal x = 0,y = 0,w = 0, h = 0, wd;
  static qint32 counter = 0;

  // ticker data updated
  if (newdata && !ticker_running)
  {
    QString tickerstr;

    tickerstr.reserve (2048);
    tickerstr = QStringLiteral ("<table border=0 cellpadding=0 cellspacing=0><tr>");
    foreach (RTPrice rtp, rtplist)
    {
      QString prc;
      QChar c;
      const char green[] = "<td bgcolor=black><font size=5 color=green>%1</font></td>",
                 red[] = "<td bgcolor=black><font size = 5 color=red>%1</font></td>",
                 white[] = "<td bgcolor=black><font size = 5 color=white>%1</font></td>",
                 space[] = "<td bgcolor=black><font size = 5 color=black>&nbsp;</font></td>",
                 *fmt;

      // rtp.symbol = rtp.symbol.remove ("INDEX");
      prc = rtp.prcchange.replace (QStringLiteral ("%"), QStringLiteral (" "));
      float p = prc.toFloat ();
      if (p > 0)
        fmt = green;
      else if (p < 0)
        fmt = red;
      else
        fmt = white;

      tickerstr += QString (space) % QString (space);
      foreach (c, rtp.symbol)
      {
        tickerstr += QString (fmt).arg (c);
      }
      tickerstr += QString (space);

      foreach (c, rtp.price)
      {
        tickerstr += QString (fmt).arg (c);
      }
      tickerstr += QString (space);

      foreach (c, rtp.prcchange)
      {
        tickerstr += QString (fmt).arg (c);
      }

      if (rtp.prcchange.size () > 0)
        tickerstr += QString (fmt).arg ("%");

      tickerstr += QString (space) % QString (space);
    }
    tickerstr += QStringLiteral ("</tr></table>");
    tickerstring = tickerstr;

    QGraphicsScene* gs = scene();
    foreach (QGraphicsItem *item, gs->items ())
      gs->removeItem (item);

    if (rtplist.size () > 0)
    {
      tickerlabel->setHtml (tickerstring);
      tickerlabel->setVisible (false);
      gs->addItem (tickerlabel);

      tickerlabel->boundingRect ().getRect (&x, &y, &w, &h);
      wd = width ();
      counter = wd;
      QPixmap srcPixmap(w, h), fPixmap;
      QPainter tmpPainter(&srcPixmap);
      tickerlabel->document()->drawContents(&tmpPainter);
      tmpPainter.end();
      gs->removeItem (tickerlabel);
      fPixmap = srcPixmap.copy (5, 0, w - 10, h - 5);
      pixtickerlabel = gs->addPixmap (fPixmap);
      newdata = false;
    }
  }

  if (counter == wd)
    ticker_running = true;

  pixtickerlabel->setPos (counter, -4);
  scene ()->update ();
  counter -= 3;

#ifndef GUI_DESKTOP
  if (firstrun)
  {
    (qobject_cast <MainWindow *> (parent ()))->enableTickerButton ();
    firstrun = false;
  }
#endif

  if (counter < (w * -1))
  {
    ticker_running = false;
    wd = width ();
    counter = wd;
  }
}

/// events
// resize
void
StockTicker::resizeEvent (QResizeEvent *)
{
  scene()->setSceneRect (0, 0, width () - 10, height () - 5);
}

void
StockTicker::timerEvent(QTimerEvent *)
{
    ticker();
}

/// slots
void
StockTicker::updatePrices (RTPriceList rtprice)
{
  qint32 rs = rtplist.size ();
  if (rs != rtprice.size ())
  {
    rtplist = rtprice;
    newdata = true;
  }

  rs = rtplist.size ();
  for (qint32 counter = 0; counter < rs; counter ++)
  {
    if (rtplist.at (counter).symbol != rtprice.at (counter).symbol ||
        rtplist.at (counter).price != rtprice.at (counter).price)
    {
      rtplist = rtprice;
      newdata = true;
    }
  }

  if (newdata && timerId == 0)
  {
    timerId = startTimer( 1000 / tickerspeed );
  }
}

void StockTicker::setSpeed (qint16 speed)
{
    if( speed < 10 )
        speed = 10;
    if( speed != tickerspeed )
    {
        tickerspeed = speed;
        if( timerId )
            killTimer( timerId );
        timerId = startTimer( 1000 / tickerspeed );
    }
}
