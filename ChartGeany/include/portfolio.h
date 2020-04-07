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

#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <QWidget>
#include <QGraphicsView>
#include <QShowEvent>

#include "qtcgraphicsscene.h"
#include "feedyahoo.h"
#include "feediex.h"
#include "feedav.h"
#include "addtransactiondialog.h"
#include "addpricedialog.h"

class Portfolio;

namespace Ui
{
  class PriceWorkerPortfolio;
}

// Price worker for a portfolio
class PriceWorkerPortfolio : public QObject
{
  Q_OBJECT

public:
  explicit PriceWorkerPortfolio (int pfid); //constructor
  ~PriceWorkerPortfolio (void);  // destructor
  bool isRunning ()
  {
    return (bool) state.fetchAndAddAcquire (0);
  }; // returns running state
  void setParentObject (Portfolio *obj)
  {
    parentObject = obj;
  }; // set the parent object

public slots:
  void process(void);       // thread process
  void terminate (void);    // thread terminate

signals:

private:
  int pf_id;                // portfolio id
  Portfolio *parentObject;  // parent object
  QAtomicInt runflag;     // set false to terminate execution
  QAtomicInt state;   // true if running
  QStringList datafeed;   // data feed
  QStringList symbol;     // symbol
  YahooFeed *yfeed;  // Yahoo Finance feed
  AlphaVantageFeed *afeed; // Alpha Vantage feed
  IEXFeed *efeed; // IEX feed
};

// Portfolio totals widget
class PortfolioTotals : public QWidget
{
  Q_OBJECT

public:
  explicit PortfolioTotals (QWidget * parent = 0);
  ~PortfolioTotals (void);

  void reloadTotals (const QString &sql);      // reload portfolio totals
private:
  QGraphicsView *graphicsView;          // graphics view
  QTCGraphicsScene *scene;              // graphics scene
  QGraphicsTextItem *totalslabel;       // graphics label for totals
  QGraphicsPixmapItem *pixtotals;       // graphics pixmap for totals
  QPixmap fPixmap;                      // pixmap for totals;

protected:
  virtual void resizeEvent (QResizeEvent * event);  // resize event
};

namespace Ui
{
  class Portfolio;
}

// custom event filter for portfolios
class PortfolioEventFilter:public QObject
{
Q_OBJECT public:
  explicit PortfolioEventFilter (QObject * parent); // constructor
  ~PortfolioEventFilter (void); // destructor

protected:
  bool eventFilter (QObject * watched, QEvent * event);
};

class Portfolio : public QWidget
{
  Q_OBJECT

public:
  explicit Portfolio (int pfid, QWidget * parent = 0);
  ~Portfolio (void);

  void    emitUpdatePortfolioPrices (int pf_id);    // update ticker signal emittion
  inline QString title () const NOEXCEPT
  {
    return ptitle;
  };                        // returns portfolio's title
  inline int id () const NOEXCEPT
  {
    return pf_id;
  };                            // returns portfolio's id
  void    reloadTransaction (int tr_id);            // reload specific transaction
  void    reloadSymbols ();                     // reload symbols
  void    reloadPositions ();                       // reload the positions
  void    setTitle (QString text)
  {
    ptitle = text;
  };                        // sets the tab's text
  void    setFeed (QString text);               // sets portfolio's data feed
  void    setCurrency (QString text)
  {
    currency = text;
  };                        // sets portfolio's currency

private:
  Ui::Portfolio * ui;

  QString symFilter;                // symbol filter
  QString feed;                 // data feed
  QString currency;             // currency
  QString ptitle;                   // portfolio's title
  QThread thread;                   // worker thread
  AddTransactionDialog *trdlg;      // transaction dialog
  AddPriceDialog *prcdlg;           // price dialog
  PriceWorkerPortfolio *worker;     // worker class
  PortfolioTotals *ptotals;         // portfolio totals
  PortfolioEventFilter *evfilter;   // event filter
  int pf_id;                        // portfolio id

  void cleartransactiontable ();    // clear the transaction table
  void clearpositiontable ();       // clear the transaction table
  void reloadTransactions ();       // reload the transactions
  CG_ERR_RESULT exportTransactions (const QString & filename); // export transactions
  CG_ERR_RESULT importTransactions (const QString & filename, bool del); // export transactions

private slots:
  void transactionButton_clicked (void);
  void chartButton_clicked (void);
  void optionsButton_clicked (void);
  void portfolioButton_clicked (void);
  void colPortfolioButton_clicked (void);
  void addButton_clicked (void);
  void editButton_clicked (void);
  void deleteButton_clicked (void);
  void trUpButton_clicked (void);
  void trDownButton_clicked (void);
  void upButton_clicked (void);
  void downButton_clicked (void);
  void transaction_double_clicked (void);
  void filter_combol_changed (const QString &);
  void updatePortfolioPricesSlot (int pfid);
  void position_double_clicked (void);
  void importButton_clicked (void);
  void exportButton_clicked (void);
  void setPriceButton_clicked (void);
  void colUpButton_clicked (void);
  void colDownButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);  // resize event
  virtual void showEvent (QShowEvent * event);      // show event

signals:
  void updatePortfolioPrices (int pfid); // update portfolio prices
  void expandChartToggle ();
  void showChart (const TableDataVector&);
};

#endif // PORTFOLIO_H
