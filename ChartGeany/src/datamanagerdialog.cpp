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

#include <QScopedPointer>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QResizeEvent>
#include <QDateTime>
#include "ui_datamanagerdialog.h"
#include "mainwindow.h"
#include "common.h"
#include "databrowserdialog.h"
#include "downloaddatadialog.h"
#include "feedyahoo.h"
#include "feedav.h"
#include "feediex.h"

static const int NCOLUMNS = 14;

extern int
sqlcb_symbol_table(void *classptr, int argc, char **argv, char **column);

// constructor
DataManagerDialog::DataManagerDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::DataManagerDialog), downloadDialog(nullptr)
{
  const QString
  stylesheet = QStringLiteral ("background: transparent; background-color: white;"),
  buttonstylesheet = QStringLiteral ("background: transparent; background-color: white; color:black;"),
  stylesheet3 = QStringLiteral ("selection-background-color: blue;");
  QStringList cheadersList, // list of columns' headers
              filter;

  int colwidth = 0;

  cheadersList << QStringLiteral ("  Symbol  ")
               << QStringLiteral ("  Name  ")
               << QStringLiteral ("  Market  ")
               << QStringLiteral ("  Feed  ")
               << QStringLiteral ("  T.F.  ")
               << QStringLiteral ("  Starts  ")
               << QStringLiteral ("  Ends  ")
               << QStringLiteral ("  Curr.  ")
               << QStringLiteral ("  Key  ")
               << QStringLiteral ("  Adj.  ")
               << QStringLiteral ("  Base  ")
               << QStringLiteral ("  Path  ")
               << QStringLiteral ("  Format ")
               << QStringLiteral ("  Last Update ");

  symFilter = QStringLiteral ("");
  updateBeforeOpen = false;
  ui->setupUi (this);

  this->setStyleSheet (stylesheet);
  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tableWidget->sortByColumn (0, Qt::AscendingOrder);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->importButton->setStyleSheet (buttonstylesheet);
  ui->downloadButton->setStyleSheet (buttonstylesheet);
  ui->trashButton->setStyleSheet (buttonstylesheet);
  ui->refreshButton->setStyleSheet (buttonstylesheet);
  ui->updateButton->setStyleSheet (buttonstylesheet);
  ui->browserButton->setStyleSheet (buttonstylesheet);
  ui->exitButton->setStyleSheet (buttonstylesheet);
  ui->chartButton->setStyleSheet (buttonstylesheet);
  ui->upToolButton->setStyleSheet (buttonstylesheet);
  ui->downToolButton->setStyleSheet (buttonstylesheet);
  ui->symbolFilterComboBox->setStyleSheet (stylesheet % stylesheet3 % QStringLiteral ("combobox-popup: 0"));
  ui->tableWidget->setColumnHidden (4, true);
  ui->tableWidget->setColumnHidden (8, true);
  ui->tableWidget->setColumnHidden (10, true);
  ui->tableWidget->setColumnHidden (11, true);
  ui->tableWidget->setColumnHidden (12, true);
  ui->tableWidget->setColumnHidden (13, true);
  ui->tableWidget->setColumnWidth(0, 10);
  // ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  reloadSymbols ();

  for (qint32 counter = 0; counter < NCOLUMNS - 1; counter ++)
    colwidth += ui->tableWidget->columnWidth (counter);

  filter += QStringLiteral ("ALL");
  filter += QStringLiteral ("^");
  for (char c='A'; c <= 'Z'; c++)
    filter += QString (QChar (c));
  for (char c='0'; c <= '9'; c++)
    filter += QString (QChar (c));
  ui->symbolFilterComboBox->addItems (filter);
  ui->symbolFilterComboBox->setMaxVisibleItems (10);

  this->resize (colwidth + 40, height ());
  browser = new DataBrowserDialog (this);

  // connect to signals
  connect (ui->chartButton, SIGNAL (clicked ()), this,
           SLOT (chartButton_clicked ()));
  connect (ui->importButton, SIGNAL (clicked ()), ui->tableWidget,
           SLOT (clearSelection()));
  connect (ui->importButton, SIGNAL (clicked ()), parent,
           SLOT (showCsvDialog()));
  connect (ui->downloadButton, SIGNAL (clicked ()), this,
           SLOT (downloadButton_clicked ()));
  connect (ui->refreshButton, SIGNAL (clicked ()), this,
           SLOT (refreshButton_clicked ()));
  connect (ui->exitButton, SIGNAL (clicked ()), this,
           SLOT (exitButton_clicked ()));
  connect (ui->trashButton, SIGNAL (clicked ()), this,
           SLOT (trashButton_clicked ()));
  connect (ui->updateButton, SIGNAL (clicked ()), this,
           SLOT (updateButton_clicked ()));
  connect (ui->browserButton, SIGNAL (clicked ()), this,
           SLOT (browserButton_clicked ()));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect(ui->tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(symbol_double_clicked ()));
  connect(ui->symbolFilterComboBox, SIGNAL(currentIndexChanged ( const QString &)),
          this, SLOT(filter_combol_changed (const QString &)));

  // correctWidgetFonts (this);
  // correctTitleBar (this);
}

// destructor
DataManagerDialog::~DataManagerDialog ()
{
  cleartable ();
  delete ui;
}

// clear table
void
DataManagerDialog::cleartable ()
{
  int row, nrows, col, ncols = NCOLUMNS;

  nrows = ui->tableWidget->rowCount ();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete ui->tableWidget->takeItem(row,col);
}

// fill table column
void
DataManagerDialog::fillcolumn (QStringList list, int col)
{
  for (qint32 counter = 0; counter < list.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (list[counter]);
    ui->tableWidget->setItem(counter,col,item);
  }
}

// reload all symbols
void
DataManagerDialog::reloadSymbols ()
{
  int rc;
  QString SQLCommand;

  symbolList.clear ();
  descList.clear ();
  marketList.clear ();
  sourceList.clear ();
  timeframeList.clear ();
  datefromList.clear ();
  datetoList.clear ();
  currencyList.clear ();
  keyList.clear ();
  adjustedList.clear ();
  baseList.clear ();
  pathList.clear ();
  formatList.clear ();
  lastupdateList.clear ();

  SQLCommand =  QStringLiteral ("select SYMBOL, DESCRIPTION, MARKET, SOURCE, TIMEFRAME, LASTUPDATE, ") %
                QStringLiteral ("DATEFROM, DATETO, CURRENCY, KEY, ADJUSTED, BASE, DNLSTRING, FORMAT ") %
                QStringLiteral ("from SYMBOLS_ORDERED where SYMBOL like '") % symFilter % QStringLiteral ("%';");

  rc = selectfromdb(SQLCommand.toUtf8 (), sqlcb_symbol_table, this);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS), this);
    this->hide ();
    return;
  }

  cleartable ();
  ui->tableWidget->setSortingEnabled (false);
  ui->tableWidget->setRowCount (symbolList.size ());
  fillcolumn (symbolList, 0);
  fillcolumn (descList, 1);
  fillcolumn (marketList, 2);
  fillcolumn (sourceList, 3);
  fillcolumn (timeframeList, 4);
  fillcolumn (datefromList, 5);
  fillcolumn (datetoList, 6);
  fillcolumn (currencyList, 7);
  fillcolumn (keyList, 8);
  fillcolumn (adjustedList, 9);
  fillcolumn (baseList, 10);
  fillcolumn (pathList, 11);
  fillcolumn (formatList, 12);
  fillcolumn (lastupdateList, 13);

  ui->tableWidget->resizeColumnToContents (0);
  ui->tableWidget->resizeColumnToContents (1);
  ui->tableWidget->resizeColumnToContents (2);
  ui->tableWidget->resizeColumnToContents (3);
  ui->tableWidget->resizeColumnToContents (4);
  ui->tableWidget->resizeColumnToContents (5);
  ui->tableWidget->resizeColumnToContents (6);
  ui->tableWidget->resizeColumnToContents (7);
  ui->tableWidget->resizeColumnToContents (9);
  ui->tableWidget->viewport()->update();

  ui->tableWidget->setSortingEnabled (true);
}

/// Signals
///
// downloadButton_clicked ()
void
DataManagerDialog::downloadButton_clicked ()
{
  if( ! downloadDialog )
  {
    downloadDialog = new DownloadDataDialog(this);
    if( ! downloadDialog )
      return;
  }

  ui->tableWidget->clearSelection ();
  downloadDialog->show ();
}

// refreshButton_clicked ()
void
DataManagerDialog::refreshButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  reloadSymbols ();
}

// exitButton_clicked ()
void
DataManagerDialog::exitButton_clicked ()
{
  ui->tableWidget->clearSelection ();
  this->hide ();
}

// sqlite3_exec callback for retieving sqlite_master table
static int
sqlcb_sqlite_master (void *classptr, int argc, char **argv, char **column)
{
  DataManagerDialog *dialog = static_cast <DataManagerDialog *> (classptr);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("TYPE"))
      dialog->sqlite_master_type << QString::fromUtf8 (argv[counter]);
    if (colname == QLatin1String ("NAME"))
      dialog->sqlite_master_name << QString::fromUtf8 (argv[counter]);
  }

  return 0;
}

// form a DROP SQL sentence for the given table or view
QString
DataManagerDialog::formSQLDropSentence (QString table, qint32 *nentries)
{
  QString query, SQLCommand = "";
  int rc;

  *nentries = 0;
  query = QStringLiteral ("select type, name from sqlite_master where name like '") %
          table % QStringLiteral ("%';");
  rc = selectfromdb(query.toUtf8(), sqlcb_sqlite_master, this);

  if (rc == SQLITE_OK)
    for (qint32 counter = 0, maxcounter = sqlite_master_type.size ();
         counter < maxcounter; counter ++)
    {
      if (sqlite_master_type[counter] == "table" ||
          sqlite_master_type[counter] == "view")
      {

        (*nentries) ++;
        SQLCommand += QStringLiteral ("DROP ") %  sqlite_master_type[counter] %
                      QStringLiteral (" IF EXISTS ") % sqlite_master_name[counter] % QStringLiteral (";");
        SQLCommand.append ('\n');
        SQLCommand += QStringLiteral ("DELETE FROM CHART_SETTINGS WHERE KEY = '") %
                      sqlite_master_name[counter] % QStringLiteral ("'; ");
        SQLCommand.append ('\n');
        SQLCommand += QStringLiteral ("DELETE FROM SYMBOLS WHERE KEY = '") %
                      sqlite_master_name[counter] % QStringLiteral ("'; ");
        SQLCommand.append ('\n');
        SQLCommand += QStringLiteral ("DROP TABLE IF EXISTS template_") %
                      sqlite_master_name[counter] % QStringLiteral (";");
        SQLCommand.append ('\n');
      }
    }
  SQLCommand += QStringLiteral ("DELETE FROM SYMBOLS WHERE KEY LIKE '") % table % QStringLiteral ("%'; ");
  return SQLCommand;
}

// trashButton_clicked ()
void
DataManagerDialog::trashButton_clicked ()
{
  QScopedPointer<WaitDialog> waitdlg(new WaitDialog(this));
  QString SQLCommand, table, base;
  QStringList selected_tables, selected_bases;
  qint32 entries = 0;
  int row, maxrow, rc;

  selected_tables.clear ();
  selected_bases.clear ();
  sqlite_master_name.clear ();
  sqlite_master_type.clear ();

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      selected_tables << ui->tableWidget->item (row, 8)->text ();
      selected_bases << ui->tableWidget->item (row, 10)->text ();
    }

  ui->tableWidget->clearSelection ();

  if (selected_tables.size () == 0)
  {
    showMessage ("Select symbols first please.", this);
    return;
  }

  if (showOkCancel ("Delete selected entries?", this) == false)
    return;

  if (showOkCancel ("Entries depended on other entries (eg: entries for adjusted prices) will be deleted too. Are you sure?", this) == false)
    return;

  correctWidgetFonts (qobject_cast <QDialog *> (waitdlg.data ()));
  waitdlg->setMessage (QString::fromUtf8 ("Deleting. Please wait..."));
  waitdlg->show ();
  qApp->processEvents ();
  delay (1);

  maxrow = selected_tables.size ();
  for (row = 0; row < maxrow; row ++)
  {
    QString tmpSQL;
    table = selected_tables.at (row);
    base = selected_bases.at (row);
    tmpSQL = formSQLDropSentence (table, &entries);
    if (row % 10 != 0)
      SQLCommand += tmpSQL;
    else
    {
      rc = updatedb (SQLCommand);
      SQLCommand += tmpSQL;
    }
  }

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    waitdlg->hide ();
    setGlobalError(CG_ERR_DELETE_DATA, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DELETE_DATA), this);
  }

  refreshButton_clicked ();
}

// updateButton_clicked ()
void
DataManagerDialog::updateButton_clicked ()
{
  YahooFeed YF;
  IEXFeed EF;
  AlphaVantageFeed AF;

  QStringList symbol, timeframe, currency, feed, dateto, adjusted,
              base, key, name, market, path, format;
  QString prevbase = QStringLiteral ("");
  CG_ERR_RESULT result = CG_ERR_OK;
  int row, maxrow, errcounter = 0;
  bool adjbool = true;

  maxrow = ui->tableWidget->rowCount ();
  for (qint32 counter = 0; counter < maxrow; counter ++)
  {
    if (ui->tableWidget->item (counter, 0)->isSelected ())
      for (row = 0; row < maxrow; row ++)
      {
        if (ui->tableWidget->item (counter, 10)->text () ==
            ui->tableWidget->item (row, 8)->text ())
          if (!base.contains (ui->tableWidget->item (counter, 10)->text (), Qt::CaseSensitive))
          {
            symbol << ui->tableWidget->item (row, 0)->text ();
            name << ui->tableWidget->item (row, 1)->text ();
            market << ui->tableWidget->item (row, 2)->text ();
            timeframe << ui->tableWidget->item (row, 4)->text ();
            currency << ui->tableWidget->item (row, 7)->text ();
            feed << ui->tableWidget->item (row, 3)->text ();
            dateto << ui->tableWidget->item (row, 6)->text ();
            key << ui->tableWidget->item (row, 8)->text ();
            adjusted << ui->tableWidget->item (row, 9)->text ();
            base << ui->tableWidget->item (row, 10)->text ();
            path << ui->tableWidget->item (row, 11)->text ();
            format << ui->tableWidget->item (row, 12)->text ();
          }
      }
  }

  ui->tableWidget->clearSelection ();
  if (feed.size () == 0)
  {
    if (!updateBeforeOpen)
      showMessage ("Select symbols first please.", this);
    return;
  }

  if (!updateBeforeOpen)
    if (showOkCancel ("Update selected entries?", this) == false)
      return;

  GlobalProgressBar = progressdialog->getProgressBar ();
  GlobalProgressBar->setValue (0);
  progressdialog->setMessage (QStringLiteral ("Please wait..."));
  progressdialog->show ();
  qApp->processEvents ();
  delay (1);

  maxrow = feed.size ();
  for (row = 0; row < maxrow; row ++)
  {
    GlobalProgressBar->setValue (0);
    progressdialog->setMessage (QStringLiteral ("Updating data for symbol: ") % symbol.at (row));
    qApp->processEvents ();

    if (feed.at (row) == QLatin1String ("YAHOO"))
    {
      result = YF.downloadData (symbol.at (row), timeframe.at (row), currency.at (row),
                                QStringLiteral ("UPDATE"), adjbool);
      if (result != CG_ERR_OK)
      {
        errcounter ++;
        if (maxrow == 1)
        {
          progressdialog->hide ();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (feed.at (row) == QLatin1String ("IEX"))
    {
      result = EF.downloadData (symbol.at (row), timeframe.at (row), currency.at (row),
                                QStringLiteral ("UPDATE"), adjbool);
      if (result != CG_ERR_OK)
      {
        errcounter ++;
        if (maxrow == 1)
        {
          progressdialog->hide ();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (feed.at (row) == QLatin1String ("ALPHAVANTAGE"))
    {
      result = AF.downloadData (symbol.at (row), timeframe.at (row), currency.at (row),
                                QStringLiteral ("UPDATE"), adjbool);
      if (result != CG_ERR_OK)
      {
        errcounter ++;
        if (maxrow == 1)
        {
          progressdialog->hide ();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (feed.at (row) == QLatin1String ("CSV") && path.at (row) != "" && format.at (row) != "")
    {
      SymbolEntry symboldata;

      symboldata.csvfile = path.at (row);
      symboldata.symbol = symbol.at (row);
      symboldata.name = name.at (row);
      symboldata.currency = currency.at (row);
      symboldata.format = format.at (row);
      symboldata.timeframe = timeframe.at (row);
      symboldata.source = QStringLiteral ("CSV");

      if (QLatin1String ("YAHOO CSV") == symboldata.format)
        symboldata.adjust = true;
      else
        symboldata.adjust = false;

      if (market.at (row) == QLatin1String (""))
        symboldata.market = QStringLiteral ("NONE");
      else
        symboldata.market = market.at (row);

      symboldata.tablename =  symboldata.symbol % QStringLiteral ("_") %
                              symboldata.market % QStringLiteral ("_") %
                              symboldata.source % QStringLiteral ("_");

      symboldata.tablename += symboldata.timeframe;
      symboldata.tmptablename = QStringLiteral ("TMP_") % symboldata.tablename;
      symboldata.dnlstring = symboldata.csvfile;

      result = csv2sqlite (&symboldata, QStringLiteral ("CREATE"));
      if (result != CG_ERR_OK)
      {
        errcounter ++;
        if (maxrow == 1)
        {
          progressdialog->hide ();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (feed.at (row) == QLatin1String ("XLS") && path.at (row) != "")
    {
      SymbolEntry symboldata;

      symboldata.csvfile = path.at (row);
      symboldata.symbol = symbol.at (row);
      symboldata.name = name.at (row);
      symboldata.currency = currency.at (row);
      symboldata.format = QStringLiteral ("MICROSOFT EXCEL");
      symboldata.timeframe = timeframe.at (row);
      symboldata.source = QStringLiteral ("XLS");
      symboldata.adjust = false;

      if (market.at (row) == QLatin1String (""))
        symboldata.market = QStringLiteral ("NONE");
      else
        symboldata.market = market.at (row);

      symboldata.tablename =  symboldata.symbol % QStringLiteral ("_") %
                              symboldata.market % QStringLiteral ("_") %
                              symboldata.source % QStringLiteral ("_");

      symboldata.tablename += symboldata.timeframe;
      symboldata.tmptablename = QStringLiteral ("TMP_") % symboldata.tablename;
      symboldata.dnlstring = symboldata.csvfile;

      result = csv2sqlite (&symboldata, QStringLiteral ("CREATE"));
      if (result != CG_ERR_OK)
      {
        errcounter ++;
        if (maxrow == 1)
        {
          progressdialog->hide ();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (progressdialog->getCancelRequestFlag ())
    {
      progressdialog->hide ();
      showMessage (QStringLiteral ("Update canceled."), this);
      refreshButton_clicked ();
      return;
    }
  }

  progressdialog->hide ();
  if (!updateBeforeOpen)
    showMessage (QStringLiteral ("Update completed with ") +
                 QString::number (errcounter) % QStringLiteral (" errors."),
                 this);
  refreshButton_clicked ();
}

// browserButton_clicked ()
void
DataManagerDialog::browserButton_clicked ()
{
  QString tablename, symbol, timeframe, name, adjusted, base, wtitle;
  int row, maxrow, selected = 0;

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      if (selected == 0)
      {
        tablename = ui->tableWidget->item (row, 8)->text ();
        symbol = ui->tableWidget->item (row, 0)->text ();
        name = ui->tableWidget->item (row, 1)->text ();
        timeframe = ui->tableWidget->item (row, 4)->text ();
        adjusted = ui->tableWidget->item (row, 9)->text ();
        base = ui->tableWidget->item (row, 10)->text ();
        selected = row + 1;
      }
    }

  if (selected == 0)
  {
    showMessage ("Select a symbol first please.", this);
    return;
  }

  wtitle = symbol % QStringLiteral (" ") % name % QStringLiteral (" ");
  if (adjusted == QLatin1String ("NO"))
    wtitle += QStringLiteral ("Raw");
  else
    wtitle += QStringLiteral ("Adj");

  browser->setWindowTitle (wtitle);
  browser->setTableName (tablename);
  browser->show ();
}

// chartButton_clicked ()
void
DataManagerDialog::chartButton_clicked ()
{
  QStringList tablename, symbol, timeframe, name, adjusted, base;
  int row, maxrow;

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      tablename << ui->tableWidget->item (row, 8)->text ();
      symbol << ui->tableWidget->item (row, 0)->text ();
      name << ui->tableWidget->item (row, 1)->text ();
      timeframe << ui->tableWidget->item (row, 4)->text ();
      adjusted << ui->tableWidget->item (row, 9)->text ();
      base << ui->tableWidget->item (row, 10)->text ();
    }

  maxrow = tablename.size ();
  if (maxrow == 0)
  {
    showMessage ("Select symbols first please.", this);
    return;
  }

  updateBeforeOpen = true;
  for (row = 0; row < maxrow; row ++)
  {
    QStringList symkeys;
    int index = -1;

    int rc = gDatabase->loadTableData (base.at(row), adjusted.at(row),
                                       &TDVector);
    if (rc != CG_ERR_OK)
    {
      showMessage(errorMessage(rc), this);
      return;
    }

    symkeys = (qobject_cast <MainWindow*> (parent ())->getTabKeys ("Chart"));
    if (symkeys.size () != 0)
    {
      for (qint32 counter = 0; counter < symkeys.size (); counter ++)
        if (TDVector[0].tablename == symkeys[counter])
          index = counter;
    }

    if (index != -1)
    {
      this->hide ();
      (qobject_cast <MainWindow*> (parent ()))->tabWidget->setCurrentIndex (index);
    }
    else
    {
      if (qAbs (TDVector[0].lastupdate.toLongLong () -
                (QDateTime::currentMSecsSinceEpoch() / 1000)) > 7200 &&
          Application_Options->autoupdate)
        updateButton_clicked ();
      this->hide ();
      (qobject_cast <MainWindow*> (parent ()))->addChart (TDVector);
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 100);
  }
  updateBeforeOpen = false;
}

/// Events
///
// resize
void
DataManagerDialog::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ui->exitButton->move (newsize.width () - 50, ui->exitButton->y ());
  ui->upToolButton->move (newsize.width () - 50, ui->upToolButton->y ());
  ui->downToolButton->move (newsize.width () - 50, ui->downToolButton->y ());
  ui->tableWidget->resize (newsize.width () - 60, newsize.height () - 90);
  ui->filterFrame->move ((newsize.width () / 2) - 150, newsize.height () - 29);
}

// show
void
DataManagerDialog::showEvent (QShowEvent * event)
{
  if (!event->spontaneous ())
    ui->tableWidget->clearSelection ();
}

// change
void
DataManagerDialog::changeEvent (QEvent * event)
{
  if (event->spontaneous ())
    refreshButton_clicked ();
}

// delete key
void
DataManagerDialog::keyPressEvent (QKeyEvent * event)
{
  if (event->key () == Qt::Key_Delete)
    trashButton_clicked ();
}

/// Slots
// symbol double clicked
void
DataManagerDialog::symbol_double_clicked ()
{
  chartButton_clicked ();
}

// down
void
DataManagerDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
DataManagerDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// filter changed
void
DataManagerDialog::filter_combol_changed (const QString &f)
{
  if (f == QLatin1String ("ALL"))
    symFilter = QStringLiteral ("");
  else
    symFilter = f;

  reloadSymbols ();
}
