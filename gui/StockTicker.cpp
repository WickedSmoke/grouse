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

#ifndef GUI_DESKTOP
#include "mainwindow.h"
#endif

const int UPDATE_INTERVAL = 125;


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

  qRegisterMetaType<TickerPrices> ("TickerPrices");
  tickerdata = new PriceUpdater (this);

  tickerlabel = new QGraphicsTextItem;
  tickerlabel->setFont (QFont (DEFAULT_FONT_FAMILY));
  tickerlabel->setHtml (QStringLiteral ("<td bgcolor=black><font size = 5 color=white>Please wait...</font></td>"));
  tickerlabel->setVisible (true);
  gs->addItem (tickerlabel);
  tickerlabel->setPos (0, -5);
}


StockTicker::~StockTicker()
{
  if (timerId)
    killTimer(timerId);

  delete tickerdata;
  delete tickerlabel;
}


// stock ticker data updates and advance
void StockTicker::ticker()
{
  static qreal x = 0,y = 0,w = 0, h = 0, wd;
  static qint32 counter = 0;

  // ticker data updated
  if (newdata && !ticker_running)
  {
    QString tickerstr;

    tickerstr.reserve (2048);
    tickerstr = QStringLiteral ("<table border=0 cellpadding=0 cellspacing=0><tr>");
    TickerPrices::const_iterator it;
    for( it = rtplist.begin(); it != rtplist.end(); ++it )
    {
      const char *color;
      double p = atof( (*it).prcchange );
      if (p > 0)
        color = "green";
      else if (p < 0)
        color = "red";
      else
        color = "white";

      tickerstr.append( "<td bgcolor=black><font size=5 color=" );
      tickerstr.append( color );
      tickerstr.append( ">&nbsp;&nbsp;" );
      tickerstr.append( (*it).symbol );
      tickerstr.append( "&nbsp;" );
      tickerstr.append( (*it).price );
      tickerstr.append( "&nbsp;" );
      tickerstr.append( (*it).prcchange );
      tickerstr.append( "&nbsp;&nbsp;</font></td>" );
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
  counter -= tickerspeed / 2;

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
void StockTicker::resizeEvent (QResizeEvent *)
{
  scene()->setSceneRect (0, 0, width () - 10, height () - 5);
}


void StockTicker::timerEvent(QTimerEvent *)
{
    ticker();
}


static bool samePrice( const TickerPrice& p1, const TickerPrice& p2 )
{
    if( strcmp(p1.symbol, p2.symbol) || strcmp(p1.price, p2.price) )
        return false;
    return true;
}


/// slots
void StockTicker::updatePrices(TickerPrices prices)
{
    size_t rs = rtplist.size();
    if (rs != prices.size())
    {
        rtplist = prices;
        newdata = true;
    }
    else
    {
        rs = rtplist.size();
        for (size_t i = 0; i < rs; ++i)
        {
            if (! samePrice(rtplist[i], prices[i]))
            {
                rtplist = prices;
                newdata = true;
                break;
            }
        }
    }

    if (newdata && timerId == 0)
    {
        timerId = startTimer( UPDATE_INTERVAL );
    }
}

void StockTicker::setSpeed (qint16 speed)
{
    if( speed < 10 )
        speed = 10;
    if( speed != tickerspeed )
    {
        tickerspeed = speed;
#if 0
        // Would be used if the UPDATE_INTERVAL was variable.
        if( timerId )
            killTimer( timerId );
        timerId = startTimer( UPDATE_INTERVAL );
#endif
    }
}
