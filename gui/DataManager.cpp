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

#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QResizeEvent>
#include <QScopedPointer>
#include <QTableWidget>
#include <QToolButton>
#include <QDateTime>
#include "MainWindow.h"
#include "DataManager.h"
#include "common.h"
#include "databrowserdialog.h"
#include "feedyahoo.h"
#include "feedav.h"
#include "feediex.h"

static const int NCOLUMNS = 14;

extern int
sqlcb_symbol_table(void *classptr, int argc, char **argv, char **column);


static QToolButton* _toolButton(QBoxLayout* lo, const char* iconName)
{
    QToolButton* btn = new QToolButton;
    QIcon icon;
    icon.addFile( QString(":/png/images/icons/PNG/%1").arg(iconName),
                  QSize(), QIcon::Normal, QIcon::Off );
    btn->setIcon(icon);
    btn->setIconSize(QSize(35, 35));

    lo->addWidget(btn);
    return btn;
}


// constructor
DataManager::DataManager(QWidget* parent) :
    QDialog(parent)
{
    QStringList colHeaders, filter;

    colHeaders << QStringLiteral("Symbol")
        << QStringLiteral("Name")
        << QStringLiteral("Market")
        << QStringLiteral("Feed")
        << QStringLiteral("T.F.")
        << QStringLiteral("Starts")
        << QStringLiteral("Ends")
        << QStringLiteral("Curr.")
        << QStringLiteral("Key")
        << QStringLiteral("Adj.")
        << QStringLiteral("Base")
        << QStringLiteral("Path")
        << QStringLiteral("Format")
        << QStringLiteral("Last Update");

    symFilter = QStringLiteral ("");
    updateBeforeOpen = false;

    setWindowTitle( "Data Manager" );
    setModal(true);

    QBoxLayout* lo = new QVBoxLayout( this );
    QBoxLayout* lt = new QHBoxLayout;
    lo->addLayout( lt );

    importButton   = _toolButton(lt, "File_Upload.png");
    downloadButton = _toolButton(lt, "Internet_Download.png");
    trashButton    = _toolButton(lt, "Trash_Delete.png");
    refreshButton  = _toolButton(lt, "Button_Sync.png");
    updateButton   = _toolButton(lt, "Update_Button.png");
    chartButton    = _toolButton(lt, "Chart_Graph.png");
    browserButton  = _toolButton(lt, "File_Table.png");

    // connect to signals
    //connect( importButton, SIGNAL(clicked()), parent,
    //         SLOT(showCsvDialog()));
    connect( downloadButton, SIGNAL(clicked ()), this,
             SLOT(downloadButton_clicked()) );
    connect( trashButton, SIGNAL(clicked()), this,
             SLOT(trashButton_clicked()) );
    connect( refreshButton, SIGNAL(clicked()), this,
             SLOT(refreshButton_clicked()) );
    connect( updateButton, SIGNAL(clicked()), this,
             SLOT(updateButton_clicked()) );
    connect( chartButton, SIGNAL(clicked()), this,
             SLOT(chartButton_clicked()) );
    connect( browserButton, SIGNAL(clicked()), this,
             SLOT(browserButton_clicked()) );

    tableWidget = new QTableWidget(this);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setColumnCount(NCOLUMNS);
    tableWidget->setHorizontalHeaderLabels(colHeaders);
    tableWidget->sortByColumn(0, Qt::AscendingOrder);
    tableWidget->setColumnHidden(4, true);
    tableWidget->setColumnHidden(8, true);
    tableWidget->setColumnHidden(10, true);
    tableWidget->setColumnHidden(11, true);
    tableWidget->setColumnHidden(12, true);
    tableWidget->setColumnHidden(13, true);
    tableWidget->setColumnWidth(0, 10);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    lo->addWidget( tableWidget );

    connect( tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
             SLOT(chartButton_clicked()) );
    connect( importButton, SIGNAL(clicked()), tableWidget,
             SLOT(clearSelection()) );


    lt->addStretch();


    QLabel* label = new QLabel("Show symbols starting with:");
    lt->addWidget(label);

    symbolFilterCombo = new QComboBox;
    connect( symbolFilterCombo, SIGNAL(currentIndexChanged(const QString&)),
             this, SLOT(filter_combol_changed(const QString&)) );
    lt->addWidget(symbolFilterCombo);


    QDialogButtonBox* bbox = new QDialogButtonBox( QDialogButtonBox::Close );
    connect( bbox, SIGNAL(rejected()), this, SLOT(reject()) );
    lo->addWidget( bbox );


    reloadSymbols();

    filter += QStringLiteral ("ALL");
    filter += QStringLiteral ("^");
    for (char c='A'; c <= 'Z'; c++)
        filter += QString (QChar (c));
    for (char c='0'; c <= '9'; c++)
        filter += QString (QChar (c));
    symbolFilterCombo->addItems (filter);
    symbolFilterCombo->setMaxVisibleItems (10);


    browser = new DataBrowserDialog (this);


    setMinimumSize(QSize(750, 500));
    int colwidth = 0;
    for (qint32 counter = 0; counter < NCOLUMNS - 1; counter ++)
        colwidth += tableWidget->columnWidth(counter);
    resize(colwidth + 80, height());
}


DataManager::~DataManager ()
{
    cleartable();
}


void DataManager::cleartable()
{
  int row, nrows, col, ncols = NCOLUMNS;

  nrows = tableWidget->rowCount();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete tableWidget->takeItem(row,col);
}


void DataManager::fillcolumn (QStringList list, int col)
{
  for (qint32 counter = 0; counter < list.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem(QTableWidgetItem::Type);
    item->setText(list[counter]);
    tableWidget->setItem(counter,col,item);
  }
}


void DataManager::reloadSymbols()
{
  int rc;
  QString SQLCommand;

  symbolList.clear();
  descList.clear();
  marketList.clear();
  sourceList.clear();
  timeframeList.clear();
  datefromList.clear();
  datetoList.clear();
  currencyList.clear();
  keyList.clear();
  adjustedList.clear();
  baseList.clear();
  pathList.clear();
  formatList.clear();
  lastupdateList.clear();

  SQLCommand =  QStringLiteral("select SYMBOL, DESCRIPTION, MARKET, SOURCE, TIMEFRAME, LASTUPDATE, ") %
                QStringLiteral("DATEFROM, DATETO, CURRENCY, KEY, ADJUSTED, BASE, DNLSTRING, FORMAT ") %
                QStringLiteral("from SYMBOLS_ORDERED where SYMBOL like '") % symFilter % QStringLiteral("%';");

  rc = selectfromdb(SQLCommand.toUtf8 (), sqlcb_symbol_table, this);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage(errorMessage (CG_ERR_DBACCESS), this);
    hide();
    return;
  }

  cleartable ();
  tableWidget->setSortingEnabled(false);
  tableWidget->setRowCount(symbolList.size ());
  fillcolumn(symbolList, 0);
  fillcolumn(descList, 1);
  fillcolumn(marketList, 2);
  fillcolumn(sourceList, 3);
  fillcolumn(timeframeList, 4);
  fillcolumn(datefromList, 5);
  fillcolumn(datetoList, 6);
  fillcolumn(currencyList, 7);
  fillcolumn(keyList, 8);
  fillcolumn(adjustedList, 9);
  fillcolumn(baseList, 10);
  fillcolumn(pathList, 11);
  fillcolumn(formatList, 12);
  fillcolumn(lastupdateList, 13);

  tableWidget->resizeColumnToContents(0);
  tableWidget->resizeColumnToContents(1);
  tableWidget->resizeColumnToContents(2);
  tableWidget->resizeColumnToContents(3);
  tableWidget->resizeColumnToContents(4);
  tableWidget->resizeColumnToContents(5);
  tableWidget->resizeColumnToContents(6);
  tableWidget->resizeColumnToContents(7);
  tableWidget->resizeColumnToContents(9);
  tableWidget->viewport()->update();

  tableWidget->setSortingEnabled(true);
}


void DataManager::downloadButton_clicked()
{
    tableWidget->clearSelection();
    downloaddatadialog->show();
}


void DataManager::refreshButton_clicked()
{
    tableWidget->clearSelection();
    reloadSymbols();
}


// sqlite3_exec callback for retieving sqlite_master table
static int sqlcb_sqlite_master (void *classptr, int argc, char **argv, char **column)
{
  DataManager *dialog = static_cast <DataManager *> (classptr);

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
QString DataManager::formSQLDropSentence (QString table, qint32 *nentries)
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


void DataManager::trashButton_clicked ()
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

  maxrow = tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (tableWidget->item (row, 0)->isSelected ())
    {
      selected_tables << tableWidget->item (row, 8)->text ();
      selected_bases << tableWidget->item (row, 10)->text ();
    }

  tableWidget->clearSelection ();

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
    waitdlg->hide();
    setGlobalError(CG_ERR_DELETE_DATA, __FILE__, __LINE__);
    showMessage(errorMessage (CG_ERR_DELETE_DATA), this);
  }

  refreshButton_clicked ();
}


void DataManager::updateButton_clicked ()
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

  maxrow = tableWidget->rowCount ();
  for (qint32 counter = 0; counter < maxrow; counter ++)
  {
    if (tableWidget->item (counter, 0)->isSelected ())
      for (row = 0; row < maxrow; row ++)
      {
        if (tableWidget->item (counter, 10)->text () ==
            tableWidget->item (row, 8)->text ())
          if (!base.contains (tableWidget->item (counter, 10)->text (), Qt::CaseSensitive))
          {
            symbol << tableWidget->item (row, 0)->text ();
            name << tableWidget->item (row, 1)->text ();
            market << tableWidget->item (row, 2)->text ();
            timeframe << tableWidget->item (row, 4)->text ();
            currency << tableWidget->item (row, 7)->text ();
            feed << tableWidget->item (row, 3)->text ();
            dateto << tableWidget->item (row, 6)->text ();
            key << tableWidget->item (row, 8)->text ();
            adjusted << tableWidget->item (row, 9)->text ();
            base << tableWidget->item (row, 10)->text ();
            path << tableWidget->item (row, 11)->text ();
            format << tableWidget->item (row, 12)->text ();
          }
      }
  }

  tableWidget->clearSelection ();
  if (feed.size () == 0)
  {
    if (!updateBeforeOpen)
      showMessage ("Select symbols first please.", this);
    return;
  }

  if (!updateBeforeOpen)
  {
    if (showOkCancel ("Update selected entries?", this) == false)
      return;
  }

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
          progressdialog->hide();
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
          progressdialog->hide();
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
          progressdialog->hide();
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
          progressdialog->hide();
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
          progressdialog->hide();
          showMessage (errorMessage (result), this);
          return;
        }
      }
    }

    if (progressdialog->getCancelRequestFlag ())
    {
      progressdialog->hide();
      showMessage (QStringLiteral ("Update canceled."), this);
      refreshButton_clicked ();
      return;
    }
  }

  progressdialog->hide();
  if (!updateBeforeOpen)
  {
    showMessage (QStringLiteral ("Update completed with ") +
                 QString::number (errcounter) % QStringLiteral (" errors."),
                 this);
  }
  refreshButton_clicked ();
}


void DataManager::browserButton_clicked ()
{
  QString tablename, symbol, timeframe, name, adjusted, base, wtitle;
  int row, maxrow, selected = 0;

  maxrow = tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (tableWidget->item (row, 0)->isSelected ())
    {
      if (selected == 0)
      {
        tablename = tableWidget->item (row, 8)->text ();
        symbol = tableWidget->item (row, 0)->text ();
        name = tableWidget->item (row, 1)->text ();
        timeframe = tableWidget->item (row, 4)->text ();
        adjusted = tableWidget->item (row, 9)->text ();
        base = tableWidget->item (row, 10)->text ();
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


void DataManager::chartButton_clicked()
{
    MainWindow* view;
    QStringList tablename, symbol, timeframe, name, adjusted, base;
    int row, maxrow;

    maxrow = tableWidget->rowCount ();
    for (row = 0; row < maxrow; row ++)
    {
        if (tableWidget->item (row, 0)->isSelected ())
        {
            tablename << tableWidget->item(row, 8)->text();
            symbol    << tableWidget->item(row, 0)->text();
            name      << tableWidget->item(row, 1)->text();
            timeframe << tableWidget->item(row, 4)->text();
            adjusted  << tableWidget->item(row, 9)->text();
            base      << tableWidget->item(row, 10)->text();
        }
    }

    maxrow = tablename.size();
    if (maxrow == 0)
    {
        showMessage("Select symbols first please.", this);
        return;
    }

    view = qobject_cast<MainWindow*>( parent() );
    if( ! view )
        return;

    updateBeforeOpen = true;
    for (row = 0; row < maxrow; row ++)
    {
        QStringList symkeys;
        int index = -1;

        int rc = gDatabase->loadTableData( base.at(row), adjusted.at(row),
                                           &TDVector );
        if (rc != CG_ERR_OK)
        {
            showMessage(errorMessage(rc), this);
            return;
        }

        symkeys = view->getTabKeys("Chart");
        if (symkeys.size() != 0)
        {
          for (qint32 counter = 0; counter < symkeys.size(); counter ++)
            if (TDVector[0].tablename == symkeys[counter])
              index = counter;
        }

        if (index != -1)
        {
            hide();
            view->_tabWidget->setCurrentIndex(index);
        }
        else
        {
            if( qAbs(TDVector[0].lastupdate.toLongLong() -
                (QDateTime::currentMSecsSinceEpoch() / 1000)) > 7200 &&
                Application_Options->autoupdate )
                updateButton_clicked();
            hide();
            view->addChart(TDVector);
        }

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 100);
    }
    updateBeforeOpen = false;
}


void DataManager::closeEvent(QCloseEvent * event)
{
    // QDialog::close deletes all child widgets so use hide instead.
    hide();
    event->ignore();
}


void DataManager::showEvent(QShowEvent * event)
{
    if (! event->spontaneous())
        tableWidget->clearSelection();
}


void DataManager::changeEvent(QEvent * event)
{
    if (event->spontaneous())
        refreshButton_clicked();
}


void DataManager::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
        trashButton_clicked();
}


void DataManager::filter_combol_changed(const QString &f)
{
    if (f == QLatin1String("ALL"))
        symFilter.clear();
    else
        symFilter = f;

    reloadSymbols();
}
