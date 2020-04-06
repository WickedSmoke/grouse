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
#include <QBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include "DownloadDataDialog.h"
#include "common.h"
#include "feedyahoo.h"
#include "feediex.h"
#include "feedav.h"


DownloadDataDialog::DownloadDataDialog(QWidget* parent) :
    QDialog (parent)
{
    int err;
    int row = 0;

    setWindowTitle("Download Data");

    QBoxLayout* lo = new QVBoxLayout(this);
    QGridLayout* grid = new QGridLayout;
    lo->addLayout( grid );

    grid->addWidget( new QLabel("Data Feed:"), row, 0, Qt::AlignRight );
    datafeedsCombo = new QComboBox;
    grid->addWidget( datafeedsCombo, row, 1 );
    ++row;

    grid->addWidget( new QLabel("Symbol:"), row, 0, Qt::AlignRight );
    symbolEdit = new QLineEdit;
    grid->addWidget( symbolEdit, row, 1 );

    checkButton = new QPushButton("Check");
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/icons/Search.png"),
                  QSize(), QIcon::Normal, QIcon::Off);
    checkButton->setIcon(icon1);
    grid->addWidget( checkButton, row, 2 );
    ++row;

    grid->addWidget( new QLabel("Name:"), row, 0, Qt::AlignRight );
    nameEdit = new QLineEdit;
    nameEdit->setReadOnly(true);
    grid->addWidget( nameEdit, row, 1, 1, 2 );
    ++row;

    grid->addWidget( new QLabel("Market:"), row, 0, Qt::AlignRight );
    marketEdit = new QLineEdit;
    marketEdit->setReadOnly(true);
    grid->addWidget( marketEdit, row, 1 );
    ++row;

    grid->addWidget( new QLabel("Currency:"), row, 0, Qt::AlignRight );
    currencyCombo = new QComboBox;
    grid->addWidget( currencyCombo, row, 1 );
    ++row;

    grid->addWidget( new QLabel("Adjust Values:"), row, 0, Qt::AlignRight );
    adjustCheckBox = new QCheckBox;
    grid->addWidget( adjustCheckBox, row, 1 );
    ++row;

    progressBar = new QProgressBar;
    lo->addWidget( progressBar );

    lo->addSpacing( 8 );

    buttonBox = new QDialogButtonBox;
    buttonBox->setStandardButtons( QDialogButtonBox::Close |
                                   QDialogButtonBox::Ok );
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    lo->addWidget( buttonBox );


    // datafeeds list
    err = gDatabase->loadDatafeeds( *ComboItems );
    if (err != CG_ERR_OK)
    {
        showMessage(errorMessage(err), this);
        hide();
    }
    else
    {
        datafeedsCombo->addItems(ComboItems->datafeedsList);
    }

    // currency list
    err = gDatabase->loadCurrencies( *ComboItems );
    if (err != CG_ERR_OK)
    {
        showMessage(errorMessage(err), this);
        hide();
    }
    else
    {
        currencyCombo->addItems(ComboItems->currencyList);
    }


    symlistdlg = new SymbolListDialog(this);

    // connect to signals
    connect(checkButton, SIGNAL(clicked ()),
            this, SLOT(checkButton_clicked ()));

    connect(datafeedsCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(datafeed_changed(const QString &)));

    if (datafeedsCombo->currentText().contains(QStringLiteral("IEX"),
                                               Qt::CaseInsensitive))
    {
        checkButton->setText(QStringLiteral("List"));
        symbolEdit->setReadOnly(true);
        return;
    }

    if (datafeedsCombo->currentText().contains(QStringLiteral("ALPHAVANTAGE"),
                                               Qt::CaseInsensitive))
        currencyCombo->setEnabled(true);

    setDataFeed(datafeedsCombo->currentText());
}


// set the data feed
void DownloadDataDialog::setDataFeed(const QString& feed)
{
    datafeed = feed;
    for (qint32 i = 0; i < ComboItems->datafeedsList.size (); ++i)
    {
        if (feed == ComboItems->datafeedsList [i])
        {
            symlist = ComboItems->symlistList [i];
            symlisturl = ComboItems->symlisturlList [i];
        }
    }
}


// set the symbol selected from symbol list
void DownloadDataDialog::setSelectedSymbol(QString symbol,
                                           QString name, QString market,
                                           QString url1, QString url2)
{
    symbolEdit->setText(symbol);
    nameEdit->setText(name);
    marketEdit->setText(market);
    URL1 = url1;
    URL2 = url2;
}


/// Yahoo Finance related functions
// check if symbol exists
void DownloadDataDialog::checkYahoosymbolExistence()
{
  YahooFeed YF;
  QString
  name = QStringLiteral (""),
  market = QStringLiteral (""), symbol,
  currency = QStringLiteral ("");

  nameEdit->setText ("");
  symbol = symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  checkButton->setText ("Checking");
  symExists = YF.symbolExistence (symbol, name, market, currency);
  symbolEdit->setText (symbol);
  checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (symExists)
  {
    nameEdit->setText (name);
    marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)), this);
  else
    showMessage ("Symbol not found.", this);
}


// download data from yahoo
void DownloadDataDialog::downloadYahooControl ()
{
  YahooFeed YF;
  CG_ERR_RESULT result;

  result = YF.downloadData (symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed (),
                            QStringLiteral ("DAY"),
                            currencyCombo->currentText (),
                            QStringLiteral ("DOWNLOAD"),
                            adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage (QStringLiteral ("Import complete."), this);
  else
    showMessage (errorMessage (result), this);
}


/// IEX related functions
// check if symbol exists
void DownloadDataDialog::checkIEXsymbolExistence ()
{
  IEXFeed EF;
  QString name = "", market = "", symbol;

  GlobalError = CG_ERR_OK;
  nameEdit->setText ("");
  symbol = symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  checkButton->setText ("Checking");
  symExists = EF.symbolExistence (symbol, name, market);
  symbolEdit->setText (symbol);
  checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (symExists)
  {
    nameEdit->setText (name);
    marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)), this);
  else
    showMessage ("Symbol not found.", this);
}


// download data from iex
void DownloadDataDialog::downloadIEXControl ()
{
  IEXFeed EF;
  CG_ERR_RESULT result;

  result = EF.downloadData (symbolEdit->text ().replace ("'", " ").trimmed (),
                            "DAY",
                            currencyCombo->currentText (),
                            "DOWNLOAD",
                            adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage ("Import complete.", this);
  else
    showMessage (errorMessage (result), this);
}


/// Alpha Vantage related functions
// check if symbol exists
void DownloadDataDialog::checkAlphaVantagesymbolExistence ()
{
  AlphaVantageFeed AF;
  QString
  name = QStringLiteral (""),
  market = QStringLiteral (""), symbol,
  currency = QStringLiteral ("");

  nameEdit->setText ("");
  symbol = symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed ();
  if (symbol.size () == 0)
    return;

  widgetsSetEnabled (false);
  checkButton->setText ("Checking");
  symExists = AF.symbolExistence (symbol, name, market, currency);
  symbolEdit->setText (symbol);
  checkButton->setText ("Check");
  widgetsSetEnabled (true);

  if (symExists)
  {
    nameEdit->setText (name);
    marketEdit->setText (market);
    return;
  }

  if (GlobalError.fetchAndAddAcquire (0)!= CG_ERR_OK)
    showMessage (errorMessage (GlobalError.fetchAndAddAcquire (0)), this);
  else
    showMessage ("Symbol not found.", this);
}


// download data from alpha vantage
void DownloadDataDialog::downloadAlphaVantageControl ()
{
  AlphaVantageFeed AF;
  CG_ERR_RESULT result;

  result = AF.downloadData (symbolEdit->text ().replace (QStringLiteral ("'"), QStringLiteral (" ")).trimmed (),
                            QStringLiteral ("DAY"),
                            currencyCombo->currentText (),
                            QStringLiteral ("DOWNLOAD"),
                            adjustCheckBox->isChecked ());

  if (result == CG_ERR_OK)
    showMessage (QStringLiteral ("Import complete."), this);
  else
    showMessage (errorMessage (result), this);
}


/// Common functions
// enable/disable widgets during download
void DownloadDataDialog::widgetsSetEnabled (bool boolean)
{
  datafeedsCombo->setEnabled (boolean);
  symbolEdit->setEnabled (boolean);
  checkButton->setEnabled (boolean);
  buttonBox->setEnabled (boolean);
  if (datafeedsCombo->currentText ().contains (QStringLiteral ("IEX"), Qt::CaseInsensitive))
    currencyCombo->setEnabled (boolean);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 25);
}


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


void DownloadDataDialog::checkButton_clicked ()
{
  // check if there is at least one data feed
  if (ComboItems->datafeedsList.size () == 0)
    return;

  appSetOverrideCursor (this, QCursor(Qt::WaitCursor));
  symbolEdit->setText (symbolEdit->text ().toUpper ().replace ("'", " ").trimmed ());
  marketEdit->setText ("");
  if (datafeedsCombo->currentText () == QLatin1String ("YAHOO"))
    checkYahoosymbolExistence ();
  if (datafeedsCombo->currentText () == QLatin1String ("ALPHAVANTAGE"))
    checkAlphaVantagesymbolExistence ();
  if (datafeedsCombo->currentText ().contains ("IEX"))
  {

    if (datafeedsCombo->currentText () == QLatin1String ("IEX"))
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

    symlistdlg->setTableName (ComboItems->symlistList [ComboItems->datafeedsList.indexOf (datafeedsCombo->currentText ())]);
    symlistdlg->setWindowTitle (datafeedsCombo->currentText ());
    symlistdlg->show ();
  }

  widgetsSetEnabled (true);
  appRestoreOverrideCursor (this);
}


void DownloadDataDialog::accept ()
{
  // check if there is at least one data feed
  if (ComboItems->datafeedsList.size () == 0)
    return;

  if (symbolEdit->text ().replace ("'", " ").trimmed () == QLatin1String (""))
    return;

  GlobalProgressBar = progressBar;

  if (datafeedsCombo->currentText ().contains (QStringLiteral ("YAHOO")) ||
      datafeedsCombo->currentText ().contains (QStringLiteral ("ALPHAVANTAGE")))
  {
    checkButton_clicked ();
    if (!symExists)
      return;
  }

  widgetsSetEnabled (false);
  appSetOverrideCursor (this, QCursor(Qt::WaitCursor));

  if (datafeedsCombo->currentText () == QLatin1String ("YAHOO"))
  {
    downloadYahooControl ();
    // currency list
    int rc;
    ComboItems->currencyList.clear ();
    currencyCombo->clear ();
    rc = selectfromdb (ComboItems->currencies_query, sqlcb_currencies, NULL);
    Q_UNUSED (rc);
    currencyCombo->addItems (ComboItems->currencyList);
  }

  if (datafeedsCombo->currentText () == QLatin1String ("IEX"))
    downloadIEXControl ();
  if (datafeedsCombo->currentText () == QLatin1String ("ALPHAVANTAGE"))
    downloadAlphaVantageControl ();

  appRestoreOverrideCursor (this);
  widgetsSetEnabled (true);
  GlobalProgressBar->setValue (0);
}


void DownloadDataDialog::datafeed_changed(const QString& feed)
{
    setDataFeed(feed);
    symbolEdit->setText("");
    nameEdit->setText("");
    marketEdit->setText("");

    if (feed.contains("ALPHAVANTAGE", Qt::CaseInsensitive))
        currencyCombo->setEnabled(true);
    else
        currencyCombo->setEnabled(false);

    if (feed.contains("IEX", Qt::CaseInsensitive))
    {
        checkButton->setText("List");
        symbolEdit->setReadOnly(true);
        return;
    }

    checkButton->setText("Check");
    symbolEdit->setReadOnly(false);
}
