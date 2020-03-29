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
#include "downloaddatadialog.h"
#include "ui_downloaddatadialog.h"
#include "common.h"
#include "feedyahoo.h"
#include "feediex.h"
#include "feedav.h"

// constructor
DownloadDataDialog::DownloadDataDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::DownloadDataDialog)
{
  QPalette pal;
  const QString
  stylesheet = QStringLiteral ("background: transparent;"),
  stylesheet2 = QStringLiteral ("background: transparent; background-color: white;"),
  stylesheet3 = QStringLiteral ("selection-background-color: blue");
  int rc;

  ui->setupUi (this);

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Ok);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  pal.setColor (backgroundRole (), Qt::white);
  setPalette (pal);

  ui->progressBar->setValue (0);
  ui->symbolLbl->setStyleSheet (stylesheet);
  ui->nameLbl->setStyleSheet (stylesheet);
  ui->datafeedLbl->setStyleSheet (stylesheet);
  ui->marketLbl->setStyleSheet (stylesheet);
  ui->currencyLbl->setStyleSheet (stylesheet);
  ui->symbolEdit->setStyleSheet (stylesheet2);
  ui->nameEdit->setStyleSheet (stylesheet2);
  ui->adjustLbl->setStyleSheet (stylesheet);
  ui->datafeedsComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->currencyComboBox->setStyleSheet (stylesheet2  + stylesheet3);

  // datafeeds list
  ComboItems->datafeedsList.clear ();
  ComboItems->symlistList.clear ();
  ComboItems->realtimeList.clear ();
  ComboItems->symlisturlList.clear ();
  rc = selectfromdb (ComboItems->datafeeds_query, sqlcb_datafeeds, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
  ComboItems->datafeedsList.sort ();
  ui->datafeedsComboBox->addItems (ComboItems->datafeedsList);

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

  symlistdlg = new SymbolListDialog (this);

  // connect to signals
  connect (ui->checkButton, SIGNAL (clicked ()), this,
           SLOT (checkButton_clicked ()));

  connect (ui->datafeedsComboBox,
           SIGNAL (currentIndexChanged (const QString &)), this,
           SLOT (datafeed_changed (const QString &)));

  if (ui->datafeedsComboBox->currentText ().contains(QStringLiteral ("IEX"), Qt::CaseInsensitive))
  {
    ui->checkButton->setText (QStringLiteral ("List"));
    ui->symbolEdit->setReadOnly (true);
    return;
  }

  if (ui->datafeedsComboBox->currentText ().contains(QStringLiteral ("ALPHAVANTAGE"), Qt::CaseInsensitive))
    ui->currencyComboBox->setEnabled (true);

  // correctWidgetFonts (this);
  setDataFeed (ui->datafeedsComboBox->currentText ());

  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
DownloadDataDialog::~DownloadDataDialog ()
{
  delete ui;
}

// set the data feed
void
DownloadDataDialog::setDataFeed (QString feed)
{
  datafeed = feed;
  for (qint32 counter = 0; counter < ComboItems->datafeedsList.size (); counter ++)
  {
    if (feed == ComboItems->datafeedsList [counter])
    {
      symlist = ComboItems->symlistList [counter];
      symlisturl = ComboItems->symlisturlList [counter];
    }
  }
}

// set the symbol selected from symbol list
void
DownloadDataDialog::setSelectedSymbol (QString symbol,
                                       QString name,
                                       QString market,
                                       QString url1,
                                       QString url2)
{
  ui->symbolEdit->setText (symbol);
  ui->nameEdit->setText (name);
  ui->marketEdit->setText (market);
  URL1 = url1;
  URL2 = url2;
}

/// Yahoo Finance related functions
// check if symbol exists
void
DownloadDataDialog::checkYahoosymbolExistence ()
{
  YahooFeed YF;
  QString
  name = QStringLiteral (""),
  market = QStringLiteral (""), symbol,
  currency = QStringLiteral ("");

  ui->nameEdit->setText ("");
  symbol = ui->symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  ui->checkButton->setText ("Checking");
  exists = YF.symbolExistence (symbol, name, market, currency);
  ui->symbolEdit->setText (symbol);
  ui->checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (exists)
  {
    ui->nameEdit->setText (name);
    ui->marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)));
  else
    showMessage ("Symbol not found.");
}

// download data from yahoo
void
DownloadDataDialog::downloadYahooControl ()
{
  YahooFeed YF;
  CG_ERR_RESULT result;

  result = YF.downloadData (ui->symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed (),
                            QStringLiteral ("DAY"),
                            ui->currencyComboBox->currentText (),
                            QStringLiteral ("DOWNLOAD"),
                            ui->adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage (QStringLiteral ("Import complete."));
  else
    showMessage (errorMessage (result));
}

/// IEX related functions
// check if symbol exists
void
DownloadDataDialog::checkIEXsymbolExistence ()
{
  IEXFeed EF;
  QString name = "", market = "", symbol;

  GlobalError = CG_ERR_OK;
  ui->nameEdit->setText ("");
  symbol = ui->symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  ui->checkButton->setText ("Checking");
  exists = EF.symbolExistence (symbol, name, market);
  ui->symbolEdit->setText (symbol);
  ui->checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (exists)
  {
    ui->nameEdit->setText (name);
    ui->marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)));
  else
    showMessage ("Symbol not found.");
}

// download data from iex
void
DownloadDataDialog::downloadIEXControl ()
{
  IEXFeed EF;
  CG_ERR_RESULT result;

  result = EF.downloadData (ui->symbolEdit->text ().replace ("'", " ").trimmed (),
                            "DAY",
                            ui->currencyComboBox->currentText (),
                            "DOWNLOAD",
                            ui->adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage ("Import complete.");
  else
    showMessage (errorMessage (result));
}

/// Alpha Vantage related functions
// check if symbol exists
void
DownloadDataDialog::checkAlphaVantagesymbolExistence ()
{
  AlphaVantageFeed AF;
  QString
  name = QStringLiteral (""),
  market = QStringLiteral (""), symbol,
  currency = QStringLiteral ("");

  ui->nameEdit->setText ("");
  symbol = ui->symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  ui->checkButton->setText ("Checking");
  exists = AF.symbolExistence (symbol, name, market, currency);
  ui->symbolEdit->setText (symbol);
  ui->checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (exists)
  {
    ui->nameEdit->setText (name);
    ui->marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)));
  else
    showMessage ("Symbol not found.");
}

// download data from alpha vantage
void
DownloadDataDialog::downloadAlphaVantageControl ()
{
  AlphaVantageFeed AF;
  CG_ERR_RESULT result;

  result = AF.downloadData (ui->symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed (),
                            QStringLiteral ("DAY"),
                            ui->currencyComboBox->currentText (),
                            QStringLiteral ("DOWNLOAD"),
                            ui->adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage (QStringLiteral ("Import complete."));
  else
    showMessage (errorMessage (result));
}


/// Common functions
// enable/disable widgets during download
void
DownloadDataDialog::widgetsSetEnabled (bool boolean)
{
  ui->datafeedsComboBox->setEnabled (boolean);
  ui->symbolEdit->setEnabled (boolean);
  ui->checkButton->setEnabled (boolean);
  ui->buttonBox->setEnabled (boolean);
  if (ui->datafeedsComboBox->currentText ().contains (QStringLiteral ("IEX"), Qt::CaseInsensitive))
    ui->currencyComboBox->setEnabled (boolean);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 25);
}

/// Signals
///
// callback for sqlite3_exec
static int
sqlcb_netfondslastupdate (void *dataptr, int argc, char **argv, char **column)
{
  Q_UNUSED (argc);
  Q_UNUSED (column);
  QString *lastupdate = static_cast <QString *> (dataptr);
  *lastupdate = QString (argv[0]);
  return 0;
}

// checkButton_clicked ()
void
DownloadDataDialog::checkButton_clicked ()
{
  // check if there is at least one data feed
  if (ComboItems->datafeedsList.size () == 0)
    return;

  appSetOverrideCursor (this, QCursor(Qt::WaitCursor));
  ui->symbolEdit->setText (ui->symbolEdit->text ().toUpper ().replace ("'", " ").trimmed ());
  ui->marketEdit->setText ("");
  if (ui->datafeedsComboBox->currentText () == QLatin1String ("YAHOO"))
    checkYahoosymbolExistence ();
  if (ui->datafeedsComboBox->currentText () == QLatin1String ("ALPHAVANTAGE"))
    checkAlphaVantagesymbolExistence ();
  if (ui->datafeedsComboBox->currentText ().contains ("IEX"))
  {

    if (ui->datafeedsComboBox->currentText () == QLatin1String ("IEX"))
    {
      IEXFeed EF;
      QString
      query = QStringLiteral ("SELECT IEXUPDATE FROM VERSION;"),
      lastupdate;
      int rc;
      rc = selectfromdb (query.toUtf8(), sqlcb_netfondslastupdate,
                         static_cast <void*> (&lastupdate));
      if (rc == SQLITE_OK)
      {
        if (qAbs (lastupdate.toLongLong () -
                  (QDateTime::currentMSecsSinceEpoch() / 1000)) > 7200)
          EF.populateSymlist ();
      }
    }

    symlistdlg->setTableName (ComboItems->symlistList [ComboItems->datafeedsList.indexOf (ui->datafeedsComboBox->currentText ())]);
    symlistdlg->setWindowTitle (ui->datafeedsComboBox->currentText ());
    symlistdlg->show ();
  }

  widgetsSetEnabled (true);
  appRestoreOverrideCursor (this);
}

// accept ()
void
DownloadDataDialog::accept ()
{
  // check if there is at least one data feed
  if (ComboItems->datafeedsList.size () == 0)
    return;

  if (ui->symbolEdit->text ().replace ("'", " ").trimmed () == QLatin1String (""))
    return;

  GlobalProgressBar = ui->progressBar;

  if (ui->datafeedsComboBox->currentText ().contains (QStringLiteral ("YAHOO")) ||
      ui->datafeedsComboBox->currentText ().contains (QStringLiteral ("ALPHAVANTAGE")))
  {
    checkButton_clicked ();
    if (!exists)
      return;
  }

  widgetsSetEnabled (false);
  appSetOverrideCursor (this, QCursor(Qt::WaitCursor));

  if (ui->datafeedsComboBox->currentText () == QLatin1String ("YAHOO"))
  {
    downloadYahooControl ();
    // currency list
    int rc;
    ComboItems->currencyList.clear ();
    ui->currencyComboBox->clear ();
    rc = selectfromdb (ComboItems->currencies_query, sqlcb_currencies, NULL);
    Q_UNUSED (rc);
    ui->currencyComboBox->addItems (ComboItems->currencyList);
  }
  
  if (ui->datafeedsComboBox->currentText () == QLatin1String ("IEX"))
    downloadIEXControl ();
  if (ui->datafeedsComboBox->currentText () == QLatin1String ("ALPHAVANTAGE"))
    downloadAlphaVantageControl ();

  appRestoreOverrideCursor (this);
  widgetsSetEnabled (true);
  GlobalProgressBar->setValue (0);
}

// datafeed_changed ()
void
DownloadDataDialog::datafeed_changed (const QString & feed)
{
  setDataFeed (feed);
  ui->symbolEdit->setText ("");
  ui->nameEdit->setText ("");
  ui->marketEdit->setText ("");

  if (feed.contains("ALPHAVANTAGE", Qt::CaseInsensitive))
    ui->currencyComboBox->setEnabled (true);
  else
    ui->currencyComboBox->setEnabled (false);

  if (feed.contains("IEX", Qt::CaseInsensitive))
  {
    ui->checkButton->setText ("List");
    ui->symbolEdit->setReadOnly (true);
    return;
  }

  ui->checkButton->setText ("Check");
  ui->symbolEdit->setReadOnly (false);

}
