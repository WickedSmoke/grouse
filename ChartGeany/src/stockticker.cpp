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

#include "optsize.h"
#include <QTextDocument>
#include "ui_stockticker.h"
#include "stockticker.h"
#include "mainwindow.h"

/// TickerWorker
// constructor
TickerWorker::TickerWorker () NOEXCEPT
{
  parentObject = NULL;
  state = false;
  runflag = true;
}

// destructor
TickerWorker::~TickerWorker ()
{
  runflag = false;
}

// process slot
void
TickerWorker::process()
{
  while (runflag == true)
  {
    if (parentObject != NULL)
      parentObject->emitAdvanceTicker ();
    Sleeper::msleep (250);
  }
}

// terminate slot
void
TickerWorker::terminate () NOEXCEPT
{
  runflag = false;
}

// constructor
StockTicker::StockTicker (QWidget * parent):
  QWidget (parent), ui (new Ui::StockTicker)
{
  QStringList symbol, feed;
  const QString
  buttonstylesheet = QStringLiteral ("background: transparent; background-color: white; color:black");

  newdata = false;
  tickerdata = NULL;
  tickerlabel = NULL;
  tickerspeed = Application_Settings->options.scrollspeed;
  worker = NULL;
  tickerstring = QStringLiteral ("");
  ticker_running = false;
  firstrun = true;

  ui->setupUi (this);
  ui->graphicsView->setViewportUpdateMode (QGraphicsView::NoViewportUpdate);
  ui->graphicsView->setCacheMode (QGraphicsView::CacheBackground);
  ui->graphicsView->setAlignment (Qt::AlignLeft | Qt::AlignTop);

  scene = new QTCGraphicsScene (this);
  ui->graphicsView->setScene (scene);

  scene->setItemIndexMethod (QTCGraphicsScene::NoIndex);
  scene->setBackgroundBrush (Qt::black);
  scene->setBackgroundBrush (Qt::SolidPattern);

  tickerdata = new PriceUpdater (this);

  qRegisterMetaType<RTPriceList> ("RTPriceList");
  connect(this ,SIGNAL(updateTicker (RTPriceList)),
          this, SLOT (updateTickerSlot (RTPriceList)));

  connect(this ,SIGNAL(advanceTicker ()),
          this, SLOT (advanceTickerSlot ()));

  tickerlabel = new QGraphicsTextItem;
  tickerlabel->setFont (QFont (DEFAULT_FONT_FAMILY));
  tickerlabel->setHtml (QStringLiteral ("<td bgcolor=black><font size = 5 color=white>Please wait...</font></td>"));
  tickerlabel->setVisible (true);
  scene->addItem (tickerlabel);
  tickerlabel->setPos (0, -5);

  if (parent != NULL)
    setParent (parent);
}

// destructor
StockTicker::~StockTicker ()
{
  if (worker != NULL)
  {
    worker->terminate ();
    thread.quit ();
    thread.wait ();
    delete worker;
  }

  if (tickerdata != NULL)
    delete tickerdata;

  if (tickerlabel != NULL)
    delete tickerlabel;

  delete ui;
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

    foreach (QGraphicsItem *item, scene->items ())
      scene->removeItem (item);

    if (rtplist.size () > 0)
    {
      tickerlabel->setHtml (tickerstring);
      tickerlabel->setVisible (false);
      scene->addItem (tickerlabel);

      tickerlabel->boundingRect ().getRect (&x, &y, &w, &h);
      wd = width ();
      counter = wd;
      QPixmap srcPixmap(w, h), fPixmap;
      QPainter tmpPainter(&srcPixmap);
      tickerlabel->document()->drawContents(&tmpPainter);
      tmpPainter.end();
      scene->removeItem (tickerlabel);
      fPixmap = srcPixmap.copy (5, 0, w - 10, h - 5);
      pixtickerlabel = scene->addPixmap (fPixmap);
      newdata = false;
    }
  }

  if (counter == wd)
    ticker_running = true;

  pixtickerlabel->setPos (counter, -4);
  ui->graphicsView->scene ()->update ();
  counter -= tickerspeed;

  if (firstrun)
  {
    (qobject_cast <MainWindow *> (parent ()))->enableTickerButton ();
    firstrun = false;
  }

  if (counter < (w * -1))
  {
    ticker_running = false;
    wd = width ();
    counter = wd;
  }
}

// update ticker signal emittion and slot
void
StockTicker::emitUpdateTicker (RTPriceList rtprice)
{
  emit updateTicker (rtprice);
}

// advance ticker signal emittion and slot
void
StockTicker::emitAdvanceTicker ()
{
  emit advanceTicker ();
}

/// events
// resize
void
StockTicker::resizeEvent (QResizeEvent * event)
{
  Q_UNUSED (event);
  ui->graphicsView->resize (width () - 2, height ());
  scene->setSceneRect (0, 0, width () - 10, height () - 5);
}

/// slots
void
StockTicker::updateTickerSlot (RTPriceList rtprice)
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

  if (newdata && worker == NULL)
  {
    worker = new TickerWorker ;
    worker->setParentObject (this);
    worker->moveToThread(&thread);
    connect(&thread, SIGNAL(started()), worker, SLOT(process()));
    thread.start();
    thread.setPriority (QThread::LowestPriority);
  }
}

void
StockTicker::advanceTickerSlot ()
{
  ticker ();
}
