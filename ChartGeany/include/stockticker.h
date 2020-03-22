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

#ifndef STOCKTICKER_H
#define STOCKTICKER_H

#include <QWidget>
#include <QGraphicsTextItem>
#include "qtcgraphicsscene.h"
#include "priceupdater.h"

namespace Ui
{
  class StockTicker;
}

// class for worker thread that signals the ticker advance
class TickerWorker : public QObject
{
  Q_OBJECT
public:
  TickerWorker (void) NOEXCEPT; // constructor
  ~TickerWorker (void);	// destructor
  inline bool isRunning () const NOEXCEPT
  {
    return state;
  }; // returns running state
  
  void setParentObject (StockTicker *obj) NOEXCEPT
  {
    parentObject = obj;
  }; // sets the parent

public slots:
  void process(void);		// thread process
  void terminate (void) NOEXCEPT;	// thread terminate

signals:

private:
  StockTicker *parentObject; // the parent
  bool runflag;	  // set false to terminate execution
  bool state;	  // true if running
};

// stock ticker widget
class StockTicker:public QWidget
{
  Q_OBJECT
public:
  explicit StockTicker (QWidget * parent = 0); // constructor
  ~StockTicker (void); // destructor

  void ticker (void);		// stock ticker implementation
  inline bool tickerRunning () const NOEXCEPT
  {
    return ticker_running;
  }; // returns running state
  
  inline qint16 speed () const NOEXCEPT
  {
    return tickerspeed;
  }; // returns ticker's speed
  
  inline void setSpeed (qint16 speed) NOEXCEPT
  {
    tickerspeed = speed;
  }; // sets tickerspeed
  
  void emitUpdateTicker (RTPriceList rtprice); // update ticker signal emittion
  void emitAdvanceTicker (void); // update ticker signal emittion

signals:
  void updateTicker (RTPriceList rtprice); // update the ticker data
  void advanceTicker (void); // advance ticker by one step

private:
  Ui::StockTicker *ui; 					// user interface
  QTCGraphicsScene *scene;				// ticker's graphics scene
  PriceUpdater *tickerdata;				// thread: update the prices
  QGraphicsPixmapItem *pixtickerlabel;	// ticker's pixmap
  QGraphicsTextItem *tickerlabel;		// ticker's label
  RTPriceList rtplist;					// list of real time prices and symbols
  QString tickerstring;					// html string of ticker for tickerlabel
  bool newdata;							// true: new data arrived
  bool ticker_running;					// true: ticker is running
  bool firstrun;						// true: ticker's first run
  QThread thread;	// worker thread
  TickerWorker *worker; // worker class
  qint16 tickerspeed; // ticker's speed

private slots:
  void updateTickerSlot (RTPriceList rtprice);  // update ticker
  void advanceTickerSlot (void);  				// advance ticker

protected:
  virtual void resizeEvent (QResizeEvent * event);  // resize event

};

#endif // STOCKTICKER_H
