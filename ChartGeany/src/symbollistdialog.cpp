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
 
#include <QResizeEvent>
#include <QScrollBar>
#include "ui_symbollistdialog.h"
#include "symbollistdialog.h"
#include "downloaddatadialog.h"
#include "common.h"

struct Symbol
{
  QString symbol;
  QString title;
  QString market;
  QString url1;
  QString url2;
};

Q_DECLARE_TYPEINFO (Symbol, Q_MOVABLE_TYPE);
typedef QVector < Symbol > SymbolVector;

static SymbolVector svector;
static const int NCOLUMNS = 5;

// constructor
SymbolListDialog::SymbolListDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::SymbolListDialog)
{
  const QString
  stylesheet = "background: transparent; background-color: white;",
  buttonstylesheet = "background: transparent; background-color: white; color:black";
  int colwidth = 0;
  
  symFilter = QStringLiteral ("");
  cheadersList << " Symbol "
               << " Title "
               << " Market "
               << " URL1  "
               << " URL2 ";

  ui->setupUi (this);
  this->setStyleSheet (stylesheet);
  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tableWidget->sortByColumn (0, Qt::AscendingOrder);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  ui->tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
  ui->upToolButton->setStyleSheet (buttonstylesheet);
  ui->downToolButton->setStyleSheet (buttonstylesheet);
  ui->updateButton->setVisible (false);
  ui->tableWidget->setColumnHidden (3, true);
  ui->tableWidget->setColumnHidden (4, true);
  ui->tableWidget->setSelectionMode (QAbstractItemView::SingleSelection);
  for (qint32 counter = 0; counter < NCOLUMNS - 1; counter ++)
    colwidth += ui->tableWidget->columnWidth (counter);

  this->resize (colwidth + 40, height ());
  
  QStringList filter;
  filter += QStringLiteral ("ALL");
  for (char c='A'; c <= 'Z'; c++)
    filter += QString (QChar (c));
  for (char c='0'; c <= '9'; c++)
    filter += QString (QChar (c));
  ui->symbolFilterComboBox->addItems (filter);
  ui->symbolFilterComboBox->setMaxVisibleItems (10);

  // connect to signals
  connect (ui->closeButton, SIGNAL (clicked ()), this,
           SLOT (closeButton_clicked ()));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect (ui->selectButton, SIGNAL (clicked ()), this,
           SLOT (selectButton_clicked ()));
  connect(ui->tableWidget, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(symbol_double_clicked ()));
  connect(ui->symbolFilterComboBox, SIGNAL(currentIndexChanged ( const QString &)),
          this, SLOT(filter_combol_changed (const QString &)));        
      
  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
SymbolListDialog::~SymbolListDialog ()
{
  svector.clear ();
  delete ui;
}

/// Events
///

// callback for sqlite3_exec
static int
sqlcb_table_data (void *dummy, int argc, char **argv, char **column)
{
  Symbol s;

  if (dummy != NULL)
    return 1;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == QLatin1String ("SYMBOL"))
      s.symbol = QString (argv[counter]);
    else if (colname == QLatin1String ("DESCRIPTION"))
      s.title = QString (argv[counter]);
    else if (colname == QLatin1String ("MARKET"))
      s.market = QString (argv[counter]);
    else if (colname == QLatin1String ("URL1"))
      s.url1 = QString (argv[counter]);
    else if (colname == QLatin1String ("URL2"))
      s.url2 = QString (argv[counter]);

  }

  svector += s;
  return 0;
}

// show
void
SymbolListDialog::showEvent (QShowEvent * event)
{
  if (tablename.contains ("iex"))
    ui->tableWidget->setColumnHidden (2, true);
  else
    ui->tableWidget->setColumnHidden (2, false);
  
  if (event->spontaneous ())
    return;
    
  reloadSymbols ();  
}

// reload symbols
void
SymbolListDialog::reloadSymbols ()
{  
  QString query;
  int rc;
  	
  ui->tableWidget->clearContents ();
  svector.clear ();
  query = QStringLiteral ("SELECT * FROM ") % tablename % 
          QStringLiteral (" WHERE SYMBOL LIKE '")   % symFilter % 
          QStringLiteral ("%' ORDER BY SYMBOL;");

  rc = selectfromdb (query.toUtf8(), sqlcb_table_data, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
    return;
  }

  ui->tableWidget->setSortingEnabled (false);
  ui->tableWidget->setRowCount (svector.size ());
  for (qint32 counter = 0; counter < svector.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (svector[counter].symbol);
    ui->tableWidget->setItem(counter,0,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (svector[counter].title);
    ui->tableWidget->setItem(counter,1,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (svector[counter].market);
    ui->tableWidget->setItem(counter,2,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (svector[counter].url1);
    ui->tableWidget->setItem(counter,3,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (svector[counter].url2);
    ui->tableWidget->setItem(counter,4,item);
  }

  ui->tableWidget->viewport()->update();
  lasttableused = tablename;
}

/// public slots
// close
void
SymbolListDialog::closeButton_clicked ()
{
  this->hide ();
}

// down
void
SymbolListDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
SymbolListDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// select
void
SymbolListDialog::selectButton_clicked (void)
{
  QStringList symbol, name, market, url;
  int row, maxrow;

  maxrow = ui->tableWidget->rowCount ();
  for (row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      symbol << ui->tableWidget->item (row, 0)->text ();
      name << ui->tableWidget->item (row, 1)->text ();
      market << ui->tableWidget->item (row, 2)->text ();
      url << ui->tableWidget->item (row, 3)->text ();
      url << ui->tableWidget->item (row, 4)->text ();
    }

  maxrow = symbol.size ();
  if (maxrow == 0)
  {
    showMessage ("Select a symbol first please.");
    return;
  }

  this->hide ();
  (qobject_cast <DownloadDataDialog *> (parent ()))->setSelectedSymbol (symbol[0], name[0],
      market[0], url[0], url[1]);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 100);

}

// symbol double clicked
void
SymbolListDialog::symbol_double_clicked ()
{
  selectButton_clicked ();
}

// filter changed
void
SymbolListDialog::filter_combol_changed (const QString &f)
{
  if (f == QLatin1String ("ALL"))
    symFilter = QStringLiteral ("");
  else
    symFilter = f;
  
  reloadSymbols ();  
}
