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

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QResizeEvent>
#include <QDateTime>
#include "ui_portfoliomanagerdialog.h"
#include "portfoliomanagerdialog.h"
#include "mainwindow.h"
#include "common.h"

static const int NCOLUMNS = 6;

// constructor
PortfolioManagerDialog::PortfolioManagerDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::PortfolioManagerDialog)
{
  const QString
  stylesheet = QStringLiteral ("background: transparent; background-color: white;"),
  stylesheet2 = QStringLiteral ("background: transparent; background-color: white; color:black");
  QStringList cheadersList; // list of columns' headers
  int rc;

  cheadersList << QStringLiteral ("  Title  ")
               << QStringLiteral ("  Description  ")
               << QStringLiteral ("  Currency  ")
               << QStringLiteral ("  Feed  ")
               << QStringLiteral ("  Data View  ")
               << QStringLiteral ("  ID  ");

  ui->setupUi (this);
  this->setStyleSheet (stylesheet);

  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->openButton->setStyleSheet (stylesheet2);
  ui->newButton->setStyleSheet (stylesheet2);
  ui->editButton->setStyleSheet (stylesheet2);
  ui->deleteButton->setStyleSheet (stylesheet2);
  ui->exitButton->setStyleSheet (stylesheet2);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->setColumnHidden (4, true);
  ui->tableWidget->setColumnHidden (5, true);
  ui->tableWidget->setSelectionMode (QAbstractItemView::SingleSelection);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  addportfoliodlg = new AddPortfolioDialog (this);

  // connect to signals
  connect (ui->openButton, SIGNAL (clicked ()), this,
           SLOT (openButton_clicked ()));
  connect (ui->newButton, SIGNAL (clicked ()), this,
           SLOT (newButton_clicked ()));
  connect (ui->editButton, SIGNAL (clicked ()), this,
           SLOT (editButton_clicked ()));
  connect (ui->deleteButton, SIGNAL (clicked ()), this,
           SLOT (deleteButton_clicked ()));
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect(ui->tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(portfolio_double_clicked ()));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  // correctTitleBar (this);

  ComboItems->transactiontypeList.clear ();
  rc = selectfromdb (ComboItems->transactiontypes_query, sqlcb_transactiontypes, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }

  ComboItems->commissiontypeList.clear ();
  rc = selectfromdb (ComboItems->commissiontypes_query, sqlcb_commissiontypes, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
}

// destructor
PortfolioManagerDialog::~PortfolioManagerDialog ()
{
  cleartable ();
  delete ui;
}

// clear table
void
PortfolioManagerDialog::cleartable ()
{
  int row, nrows, col, ncols = NCOLUMNS;

  nrows = ui->tableWidget->rowCount ();

  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete ui->tableWidget->takeItem(row,col);

  ui->tableWidget->setRowCount (0);
  ui->tableWidget->setSortingEnabled (false);
}

// sqlite3_exec callback for retrieving portfolios
static int
sqlcb_portfolios (void *classptr, int argc, char **argv, char **column)
{
  QTableWidget *tw = static_cast <QTableWidget *> (classptr);
  QString colname;
  int rcount = 0;
  while (tw->item (rcount, 1)->text () != "Empty")
    rcount ++;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString (column[counter]).toUpper ();
    QTableWidgetItem *item;

    if (colname == "TITLE")
    {
      item = tw->item (rcount, 0);
      item->setText (QString (argv[counter]));
    }

    if (colname == "DESCRIPTION")
    {
      item = tw->item (rcount, 1);
      item->setText (QString (argv[counter]));
    }

    if (colname == "CURRENCY")
    {
      item = tw->item (rcount, 2);
      item->setText (QString (argv[counter]));
    }

    if (colname == "DATAFEED")
    {
      item = tw->item (rcount, 3);
      item->setText (QString (argv[counter]));
    }

    if (colname == "DATAVIEW")
    {
      item = tw->item (rcount, 4);
      item->setText (QString (argv[counter]));
    }

    if (colname == "PF_ID")
    {
      item = tw->item (rcount, 5);
      item->setText (QString (argv[counter]));
    }
  }

  return 0;
}

// reload the portfolios
void
PortfolioManagerDialog::reloadPortfolios ()
{
  QString SQL;
  int rc;

  SQL = QStringLiteral ("SELECT count (*) FROM portfolios");
  int count = selectcount (SQL);
  if (count == -1)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  cleartable ();

  ui->tableWidget->setRowCount (count);
  ui->tableWidget->setColumnCount (NCOLUMNS);

  for (int row = 0; row < count; row ++)
    for (int col = 0; col < 8; col++)
  {
    QTableWidgetItem *item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText ("Empty");
    ui->tableWidget->setItem (row, col, item);
  }

  SQL = "SELECT * FROM portfolios";
  rc = selectfromdb(SQL.toUtf8(), sqlcb_portfolios,
                    static_cast <void *> (ui->tableWidget));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  ui->tableWidget->resizeColumnToContents (0);
  ui->tableWidget->resizeColumnToContents (1);
  ui->tableWidget->resizeColumnToContents (2);
  ui->tableWidget->resizeColumnToContents (3);
  ui->tableWidget->resizeRowsToContents ();
  ui->tableWidget->setSortingEnabled (true);
  ui->tableWidget->viewport()->update();
}

/// Events
///
// resize
void
PortfolioManagerDialog::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());
  ui->upToolButton->move (newsize.width () - 50, ui->upToolButton->y ());
  ui->downToolButton->move (newsize.width () - 50, ui->downToolButton->y ());
  ui->tableWidget->resize (newsize.width () - 60, newsize.height () - 90);

}

// show
void
PortfolioManagerDialog::showEvent (QShowEvent * event)
{
  if (!event->spontaneous ())
    ui->tableWidget->clearSelection ();
}

// change
void
PortfolioManagerDialog::changeEvent (QEvent * event)
{
  if (!event->spontaneous ())
    ui->tableWidget->clearSelection ();

  reloadPortfolios ();
}

// delete key
void
PortfolioManagerDialog::keyPressEvent (QKeyEvent * event)
{
  if (event->key () == Qt::Key_Delete)
    deleteButton_clicked ();
}

/// Slots
// down
void
PortfolioManagerDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
PortfolioManagerDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// openButton_clicked ()
void
PortfolioManagerDialog::openButton_clicked ()
{
  QString title = "", currency = "", feed = "";
  int pf_id = -1;

  for (qint32 row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      title = ui->tableWidget->item (row, 0)->text ();
      currency = ui->tableWidget->item (row, 2)->text ();
      feed = ui->tableWidget->item (row, 3)->text ();
      pf_id = ui->tableWidget->item (row, 5)->text ().toInt ();
    }
  }

  if (pf_id == -1)
  {
    showMessage ("Select a portfolio first please.");
    return;
  }

  (qobject_cast <MainWindow*> (parent ()))->addPortfolio (pf_id, title, currency, feed);
  this->hide ();
}

// newButton_clicked ()
void
PortfolioManagerDialog::newButton_clicked ()
{
  addportfoliodlg->setAddMode ();
  addportfoliodlg->show ();
}

// editButton_clicked ()
void
PortfolioManagerDialog::editButton_clicked ()
{
  int pf_id = -1;

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
      pf_id = ui->tableWidget->item (row, 5)->text ().toInt ();
  }

  if (pf_id == -1)
  {
    showMessage ("Select a portfolio first please.");
    return;
  }

  addportfoliodlg->setEditMode (pf_id);
  addportfoliodlg->show ();
}

// deleteButton_clicked ()
void
PortfolioManagerDialog::deleteButton_clicked ()
{
  QString SQL = "", viewname;
  int pf_id = -1, rc;

  for (int row = 0; row < ui->tableWidget->rowCount (); row ++)
  {
    if (ui->tableWidget->item (row, 0)->isSelected ())
      pf_id = ui->tableWidget->item (row, 5)->text ().toInt ();
  }

  if (pf_id == -1)
  {
    showMessage ("Select a portfolio first please.");
    return;
  }

  if (showOkCancel ("Delete selected portfolio ? ") == false)
    return;

  viewname = "pftrans_" % QString::number (pf_id);
  SQL +=
    "DROP VIEW IF EXISTS V_NAME;\
     DROP VIEW IF EXISTS V_NAMEcview;\
     DROP VIEW IF EXISTS V_NAMEcview2;\
     DROP VIEW IF EXISTS V_NAMEcview3;\
     DROP VIEW IF EXISTS V_NAMEcview4;\
     DROP VIEW IF EXISTS V_NAMEcview5;\
     DROP VIEW IF EXISTS V_NAMEcview6;\
     DROP VIEW IF EXISTS V_NAMEfull;\
     DROP VIEW IF EXISTS V_NAMEpc;\
     DROP VIEW IF EXISTS V_NAMEpq;\
     DROP VIEW IF EXISTS V_NAMEsummary;";
  SQL +=
    "DELETE FROM portfolios WHERE pf_id = " %  QString::number (pf_id) % ";";
  SQL +=
    "DELETE FROM transactions WHERE pf_id = " %  QString::number (pf_id) % ";";
  SQL.replace ("V_NAME", viewname);

  rc = updatedb (SQL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_TRANSACTION, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_TRANSACTION));
    return;
  }

}

// exitButton_clicked ()
void
PortfolioManagerDialog::exitButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  this->hide ();
}

// portfolio_double_clicked ()
void
PortfolioManagerDialog::portfolio_double_clicked ()
{
  openButton_clicked ();
}
