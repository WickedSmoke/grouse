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
 
#include <QDateTime>
#include <QShowEvent>
#include <QScrollBar>
#include "ui_addportfoliodialog.h"
#include "addportfoliodialog.h"
#include "common.h"

// constructor
AddPortfolioDialog::AddPortfolioDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::AddPortfolioDialog)
{
  QPalette pal;
  const QString
  stylesheet = QStringLiteral ("background: transparent;"),
  stylesheet2 = QStringLiteral ("background: transparent; background-color: white;"),
  stylesheet3 = QStringLiteral ("selection-background-color: blue");
  QStringList feedlist;
  int rc;

  addmode = true;
  ui->setupUi (this);

  titleEdit = ui->titleEdit;
  descriptionEdit = ui->descriptionEdit;
  datafeedsComboBox = ui->datafeedsComboBox;
  currencyComboBox = ui->currencyComboBox;

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Save);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));

  pal.setColor (backgroundRole (), Qt::white);
  setPalette (pal);

  ui->titleLbl->setStyleSheet (stylesheet);
  ui->datafeedLbl->setStyleSheet (stylesheet);
  ui->descriptionLbl->setStyleSheet (stylesheet);
  ui->currencyLbl->setStyleSheet (stylesheet);
  ui->titleEdit->setStyleSheet (stylesheet2);
  ui->descriptionEdit->setStyleSheet (stylesheet2);
  ui->descriptionEdit->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->datafeedsComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->currencyComboBox->setStyleSheet (stylesheet2  + stylesheet3);

  feedlist << QStringLiteral ("NONE") << QStringLiteral ("ALPHAVANTAGE") 
           << QStringLiteral ("IEX") << QStringLiteral ("YAHOO");
  ui->datafeedsComboBox->addItems (feedlist);

  // currency list
  ComboItems->currencyList.clear ();
  rc = selectfromdb (ComboItems->currencies_query, sqlcb_currencies, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
  ui->currencyComboBox->addItems (ComboItems->currencyList);

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
AddPortfolioDialog::~AddPortfolioDialog ()
{
  delete ui;
}

/// Signals

// portfolio id call back
static int
sqlcb_portfolioid (void *data, int argc, char **argv, char **column)
{
  qint32 *pf_id = (qint32 *) data;
  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();
    if (colname == QLatin1String ("PF_ID"))
      *pf_id = QString (argv[counter]).toInt ();
  }

  return 0;
}

// save
void
AddPortfolioDialog::ok_clicked ()
{
  QString SQL = QStringLiteral ("");
  int rc;


  if (addmode)
  {
    if (ui->titleEdit->text ().replace ("'", " ").trimmed () == "")
    {
      showMessage ("Portfolio has no title.");
      return;
    }

    SQL  = "INSERT INTO portfolios (TITLE, DESCRIPTION, DATAFEED, CURRENCY) ";
    SQL += "VALUES ('" % ui->titleEdit->text ().replace ("'", " ").trimmed () % "', '";
    SQL +=               ui->descriptionEdit->toPlainText ().replace ("'", " ").trimmed () % "', '";
    SQL +=               ui->datafeedsComboBox->currentText() % "', '";
    SQL +=               ui->currencyComboBox->currentText() % "');";
    rc = updatedb (SQL);
    if (rc != SQLITE_OK)
    {
      if (rc == 2067) //  UNIQUE constraint failed
      {
        showMessage ("Portfolio already exists.");
        return;
      }
      setGlobalError(CG_ERR_INSERT_DATA, __FILE__, __LINE__);
      showMessage (errorMessage (CG_ERR_INSERT_DATA));

      return;
    }

    qint32 pf_id = -1;
    SQL = "SELECT pf_id FROM portfolios WHERE title = '" %
          ui->titleEdit->text ().replace ("'", " ").trimmed () % "';";
    rc = selectfromdb(SQL.toUtf8(), sqlcb_portfolioid,
                      static_cast <void*> (&pf_id));
    if (rc == SQLITE_OK)
    {
// portfolio commissions paid
      SQL = "";
// portfolio transactions
      SQL += "CREATE VIEW pftrans_" % QString::number(pf_id) % " AS " %
             "SELECT transactions.*, \
              (CASE \
				WHEN transactions.trtype = 'CASH IN' \
				  THEN transactions.quantity - commissionspaid.commpaid \
				WHEN transactions.trtype = 'CASH OUT' \
				  THEN (transactions.quantity + commissionspaid.commpaid) * -1 \
				WHEN transactions.trtype = 'BUY' \
				  THEN ((transactions.quantity * transactions.price) + commissionspaid.commpaid) * -1 \
				WHEN transactions.trtype = 'SELL' \
				  THEN (transactions.quantity * transactions.price) - commissionspaid.commpaid \
				WHEN transactions.trtype = 'DIVIDEND' \
				  THEN (transactions.quantity - commissionspaid.commpaid) \
				ELSE 0 \
			  END) AS AMOUNT, \
			  portfolios.datafeed AS DATAFEED \
			  FROM transactions, commissionspaid, portfolios \
			  WHERE transactions.pf_id = " % QString::number(pf_id) %
             " AND transactions.tr_id = commissionspaid.tr_id \
			    AND portfolios.pf_id = transactions.pf_id; " %
             "UPDATE portfolios SET dataview = 'pftrans_" % QString::number(pf_id) %
             "full' WHERE pf_id = " % QString::number(pf_id) % ";";
      SQL += createportfolioviews ("pftrans_" % QString::number(pf_id));
      rc = updatedb (SQL);
      if (rc != SQLITE_OK)
      {
        setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
        showMessage ("Portfolio creation failed. Please delete it and try again.");
        return;
      }
    }
    else
    {
      setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
      showMessage ("Portfolio creation failed. Please delete it and try again.");
      return;
    }

    this->hide ();
    return;
  }

  SQL = "UPDATE portfolios SET TITLE = '" % ui->titleEdit->text ().replace ("'", " ").trimmed () %
        "', DESCRIPTION = '" % ui->descriptionEdit->toPlainText ().replace ("'", " ").trimmed () %
        "', DATAFEED = '" % ui->datafeedsComboBox->currentText() %
        "', CURRENCY = '" % ui->currencyComboBox->currentText() %
        "' WHERE PF_ID = " % QString::number(pfid) % ";";
  rc = updatedb (SQL);
  if (rc != SQLITE_OK)
  {
    if (rc == 2067) //  UNIQUE constraint failed
    {
      showMessage ("Portfolio already exists.");
      return;
    }
    setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_WRITE_FILE));

    return;
  }

  this->hide ();
}

// close
void
AddPortfolioDialog::cancel_clicked ()
{

  this->hide ();
}

/// Events
// portfolio id call back
static int
sqlcb_portfolio (void *data, int argc, char **argv, char **column)
{
  AddPortfolioDialog *u = static_cast <AddPortfolioDialog *> (data);
  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == "TITLE")
      u->titleEdit->setText (QString (argv[counter]));

    if (colname == "DESCRIPTION")
      u->descriptionEdit->setPlainText (QString (argv[counter]));

    if (colname == "CURRENCY")
      u->currencyComboBox->setCurrentIndex
      (u->currencyComboBox->findText (QString (argv[counter])));

    if (colname == "DATAFEED")
      u->datafeedsComboBox->setCurrentIndex
      (u->datafeedsComboBox->findText (QString (argv[counter])));
  }

  return 0;
}

// show
void
AddPortfolioDialog::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())
    return;

// add
  if (addmode)
  {
    this->setWindowTitle ("Add New Portfolio");
    this->setWindowIcon (QIcon (QString (":/png/images/icons/PNG/Add_Symbol.png")));
    ui->titleEdit->setText ("");
    ui->descriptionEdit->setPlainText ("");
    ui->datafeedsComboBox->setCurrentIndex (0);
    ui->currencyComboBox->setCurrentIndex (0);
    return;
  }

// edit
  QString SQL;
  int rc;

  this->setWindowTitle ("Edit Portfolio");
  this->setWindowIcon (QIcon (QString (":/png/images/icons/PNG/Pencil_2.png")));
  SQL = "SELECT * FROM portfolios WHERE pf_id = " % QString::number(pfid) % ";";
  rc = selectfromdb(SQL.toUtf8(), sqlcb_portfolio, static_cast <void*> (this));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_ACCESS_DATA, __FILE__, __LINE__);
    showMessage ("Cannot retrieve portfolio information.");
    return;
  }
}


