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

#include <QBoxLayout>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QResizeEvent>
#include <QTableWidget>
#include <QToolButton>
#include "addportfoliodialog.h"
#include "PortfolioManager.h"
#include "MainWindow.h"
#include "common.h"
#include "defs.h"


static const int NCOLUMNS = 6;


static QToolButton* _pmToolButton(QBoxLayout* lo, const char* iconName,
                                  const char* tip)
{
    QToolButton* btn = new QToolButton;

    QIcon icon;
    icon.addFile( QString(":/icons/%1").arg(iconName),
                  QSize(), QIcon::Normal, QIcon::Off );
    btn->setIcon(icon);
    btn->setIconSize(QSize(35, 35));

    if( tip )
        btn->setToolTip(QApplication::translate("PortfolioManager", tip));

    lo->addWidget(btn);
    return btn;
}


PortfolioManager::PortfolioManager(QWidget * parent) :
    QDialog(parent)
{
    QStringList headings; // list of columns' headers
    int rc;

    setWindowTitle("Portfolio Manager");
    setModal(true);

    QBoxLayout* lo = new QVBoxLayout( this );
    QBoxLayout* lt = new QHBoxLayout;
    lo->addLayout( lt );

    QToolButton* editButton = _pmToolButton(lt,
                                "Pencil_2.png", "Edit Portfolio");
    QToolButton* newButton = _pmToolButton(lt,
                                "Add_Symbol.png", "Add Portfolio");
    QToolButton* deleteButton = _pmToolButton(lt,
                                "Trash_Delete.png", "Delete Portfolio");
    QToolButton* openButton = _pmToolButton(lt,
                    "Window_App_Splitscreen_3Columns.png", "Open Portfolio");
    lt->addStretch();


    headings << QStringLiteral("Title")
             << QStringLiteral("Description")
             << QStringLiteral("Currency")
             << QStringLiteral("Feed")
             << QStringLiteral("Data View")
             << QStringLiteral("ID");

    tableWidget = new QTableWidget;
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSortingEnabled(true);
    tableWidget->setColumnCount(NCOLUMNS);
    tableWidget->setHorizontalHeaderLabels(headings);
    tableWidget->setColumnHidden(4, true);
    tableWidget->setColumnHidden(5, true);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    lo->addWidget( tableWidget );


    QDialogButtonBox* bbox = new QDialogButtonBox( QDialogButtonBox::Close );
    connect( bbox, SIGNAL(rejected()), this, SLOT(reject()) );
    lo->addWidget( bbox );


    addportfoliodlg = new AddPortfolioDialog(this);

    // connect to signals
    connect(tableWidget, SIGNAL(doubleClicked(const QModelIndex &)),
            SLOT(openButton_clicked()));
    connect(openButton,   SIGNAL(clicked()), SLOT(openButton_clicked()));
    connect(newButton,    SIGNAL(clicked()), SLOT(newButton_clicked()));
    connect(editButton,   SIGNAL(clicked()), SLOT(editButton_clicked()));
    connect(deleteButton, SIGNAL(clicked()), SLOT(deleteSelected()));

    setMinimumSize(QSize(750, 250));


    ComboItems->transactiontypeList.clear();
    rc = selectfromdb(ComboItems->transactiontypes_query,
                      sqlcb_transactiontypes, NULL);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
        showMessage(errorMessage(CG_ERR_DBACCESS), this);
        hide();
    }

    ComboItems->commissiontypeList.clear();
    rc = selectfromdb(ComboItems->commissiontypes_query,
                      sqlcb_commissiontypes, NULL);
    if (rc != SQLITE_OK)
    {
        setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
        showMessage(errorMessage(CG_ERR_DBACCESS), this);
        hide();
    }
}


PortfolioManager::~PortfolioManager()
{
    cleartable();
}


void PortfolioManager::cleartable()
{
    int row, nrows, col, ncols = NCOLUMNS;

    nrows = tableWidget->rowCount();
    for (row = 0; row < nrows; row ++)
    {
        for (col = 0; col < ncols; col ++)
            delete tableWidget->takeItem(row,col);
    }

    tableWidget->setRowCount(0);
    tableWidget->setSortingEnabled(false);
}


// sqlite3_exec callback for retrieving portfolios
static int sqlcb_portfolios(void *classptr, int argc, char **argv,
                            char **column)
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
void PortfolioManager::reloadPortfolios()
{
  QString SQL;
  int rc;

  SQL = QStringLiteral("SELECT count (*) FROM portfolios");
  int count = selectcount(SQL);
  if (count == -1)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  cleartable();

  tableWidget->setRowCount(count);
  tableWidget->setColumnCount(NCOLUMNS);

  for (int row = 0; row < count; row ++)
    for (int col = 0; col < 8; col++)
  {
    QTableWidgetItem *item = new QTableWidgetItem(QTableWidgetItem::Type);
    item->setText("Empty");
    tableWidget->setItem(row, col, item);
  }

  SQL = "SELECT * FROM portfolios";
  rc = selectfromdb(SQL.toUtf8(), sqlcb_portfolios,
                    static_cast <void *> (tableWidget));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return;
  }

  tableWidget->resizeColumnToContents(0);
  tableWidget->resizeColumnToContents(1);
  tableWidget->resizeColumnToContents(2);
  tableWidget->resizeColumnToContents(3);
  tableWidget->resizeRowsToContents();
  tableWidget->setSortingEnabled(true);
  tableWidget->viewport()->update();
}


void PortfolioManager::showEvent(QShowEvent * event)
{
    if (! event->spontaneous())
        tableWidget->clearSelection();
    reloadPortfolios();
}


void PortfolioManager::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
        deleteSelected();
}


void PortfolioManager::openButton_clicked ()
{
    QString title, currency, feed;
    int pf_id = -1;

    for (qint32 row = 0; row < tableWidget->rowCount (); row ++)
    {
        if (tableWidget->item (row, 0)->isSelected ())
        {
            title    = tableWidget->item(row, 0)->text();
            currency = tableWidget->item(row, 2)->text();
            feed     = tableWidget->item(row, 3)->text();
            pf_id    = tableWidget->item(row, 5)->text().toInt();
        }
    }

    if (pf_id == -1)
    {
        showMessage("Select a portfolio first please.", this);
        return;
    }

    (qobject_cast<MainWindow*> (parent()))->addPortfolio(pf_id, title,
                                                         currency, feed);
    hide();
}


void PortfolioManager::newButton_clicked()
{
    addportfoliodlg->setAddMode();
    addportfoliodlg->show();
}


void PortfolioManager::editButton_clicked()
{
  int pf_id = -1;

  for (int row = 0; row < tableWidget->rowCount(); row ++)
  {
    if (tableWidget->item(row, 0)->isSelected())
      pf_id = tableWidget->item(row, 5)->text().toInt();
  }

  if (pf_id == -1)
  {
    showMessage("Select a portfolio first please.", this);
    return;
  }

  addportfoliodlg->setEditMode(pf_id);
  addportfoliodlg->show();
}


void PortfolioManager::deleteSelected()
{
  QString SQL = "", viewname;
  int pf_id = -1, rc;

  for (int row = 0; row < tableWidget->rowCount(); row ++)
  {
    if (tableWidget->item(row, 0)->isSelected())
      pf_id = tableWidget->item(row, 5)->text().toInt();
  }

  if (pf_id == -1)
  {
    showMessage("Select a portfolio first please.", this);
    return;
  }

  if (showOkCancel("Delete selected portfolio?", this) == false)
    return;

  viewname = "pftrans_" % QString::number(pf_id);
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
    "DELETE FROM portfolios WHERE pf_id = " %  QString::number(pf_id) % ";";
  SQL +=
    "DELETE FROM transactions WHERE pf_id = " %  QString::number(pf_id) % ";";
  SQL.replace("V_NAME", viewname);

  rc = updatedb(SQL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_TRANSACTION, __FILE__, __LINE__);
    showMessage(errorMessage(CG_ERR_TRANSACTION), this);
  }
}


/*
void PortfolioManager::exitButton_clicked()
{
    tableWidget->clearSelection();
    hide();
}
*/
