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
 
#include "ui_loadcsvdialog.h"
#include "common.h"
#include "feedyahoo.h"

// constructor
LoadCSVDialog::LoadCSVDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::LoadCSVDialog)
{
  QPalette pal;
  const QString
  stylesheet = QStringLiteral ("background: transparent;"),
  stylesheet2 = QStringLiteral ("background: transparent; background-color: white;"),
  stylesheet3 = QStringLiteral ("selection-background-color: blue");
  int rc;

  ui->setupUi (this);
  fileDialog = new QFileDialog (this);

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Ok);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  pal.setColor (backgroundRole (), Qt::white);
  setPalette (pal);
  ui->CSVFileLbl->setStyleSheet (stylesheet);
  ui->formatLbl->setStyleSheet (stylesheet);
  ui->symbolLbl->setStyleSheet (stylesheet);
  ui->nameLbl->setStyleSheet (stylesheet);
  ui->marketLbl->setStyleSheet (stylesheet);
  ui->currencyLbl->setStyleSheet (stylesheet);
  ui->timeframeLbl->setStyleSheet (stylesheet);
  ui->filenameEdit->setStyleSheet (stylesheet2);
  ui->symbolEdit->setStyleSheet (stylesheet2);
  ui->marketEdit->setStyleSheet (stylesheet2);
  ui->nameEdit->setStyleSheet (stylesheet2);
  ui->fileposLbl1->setStyleSheet (stylesheet);
  ui->fileposLbl2->setStyleSheet (stylesheet);
  ui->marketComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->timeframesComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->formatComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->currencyComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->filechooserButton->setStyleSheet (stylesheet2);
  ui->progressBar->setValue (0);

  // format list
  ComboItems->formatList.clear ();
  rc = selectfromdb (ComboItems->formats_query, sqlcb_formats, NULL);

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
  ComboItems->formatList.sort ();
  ui->formatComboBox->addItems (ComboItems->formatList);

  // timeframes
  ComboItems->timeframeList.clear ();
  rc = selectfromdb (ComboItems->timeframes_query, sqlcb_timeframes, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
  ui->timeframesComboBox->addItems (ComboItems->timeframeList);

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

  // market list
  ComboItems->marketList.clear ();
  rc = selectfromdb (ComboItems->markets_query, sqlcb_markets, NULL);

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
  }
  ui->marketComboBox->addItems (ComboItems->marketList);

  // connect to signals
  connect (ui->filechooserButton, SIGNAL (clicked ()), this,
           SLOT (filechooserButton_clicked ()));

  connect (ui->marketComboBox, SIGNAL (currentIndexChanged (const QString &)), this,
           SLOT (marketComboBox_changed (const QString &)));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
LoadCSVDialog::~LoadCSVDialog ()
{
  delete ui;
}

/// Functions
// enable/disable widgets during download
void
LoadCSVDialog::widgetsSetEnabled (bool boolean)
{
  ui->filenameEdit->setEnabled (boolean);
  ui->filechooserButton->setEnabled (boolean);
  ui->formatComboBox->setEnabled (boolean);
  ui->symbolEdit->setEnabled (boolean);
  ui->nameEdit->setEnabled (boolean);
  ui->marketEdit->setEnabled (boolean);
  ui->marketComboBox->setEnabled (boolean);
  // ui->timeframesComboBox->setEnabled (boolean);
  ui->currencyComboBox->setEnabled (boolean);
  ui->buttonBox->setEnabled (boolean);
}

/// Signals
///
// filechooserButton_clicked ()
void
LoadCSVDialog::filechooserButton_clicked ()
{
  QString fileName, title, wildcards;

  title = QString ("Select CSV file");
  wildcards = QString ("All files (*)");
  fileName = fileDialog->getOpenFileName(this, title, QDir::homePath (), wildcards);
  if (fileName != QLatin1String (""))
  {
    ui->filenameEdit->setText (fileName);
    if (fileName.endsWith (QStringLiteral (".xls"), Qt::CaseInsensitive))
    {
      int index;
      index = ui->formatComboBox->findText (QStringLiteral ("EXCEL"), Qt::MatchContains);
      if (index != -1)
        ui->formatComboBox->setCurrentIndex (index);
    }
  }
}

// marketComboBox_changed (const QString &)
void
LoadCSVDialog::marketComboBox_changed (const QString & str)
{
  ui->marketEdit->setText (str);
}

// accept ()
void
LoadCSVDialog::accept ()
{
  SymbolEntry symboldata;
  QString market;
  CG_ERR_RESULT err;

  ui->symbolEdit->setText (ui->symbolEdit->text ().toUpper ());
  ui->symbolEdit->setText (ui->symbolEdit->text ().replace ("'", " ").trimmed ());
  ui->nameEdit->setText (ui->nameEdit->text ().toUpper ());
  ui->nameEdit->setText (ui->nameEdit->text ().replace ("'", " ").trimmed ());
  GlobalProgressBar = ui->progressBar;
  widgetsSetEnabled (false);
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 25);

  appSetOverrideCursor (this, QCursor(Qt::WaitCursor));
  if (ui->filenameEdit->text () == "")
  {
    showMessage ("Please select a file to import.");
    widgetsSetEnabled (true);
    ui->progressBar->setValue (0);
    return;
  }

  if (ui->symbolEdit->text () == "")
  {
    showMessage ("No symbol.");
    widgetsSetEnabled (true);
    ui->progressBar->setValue (0);
    return;
  }

  if ("YAHOO CSV" == ui->formatComboBox->currentText () ||
      "MICROSOFT EXCEL" == ui->formatComboBox->currentText ())
    symboldata.adjust = true;
  else
    symboldata.adjust = false;

  if (ui->marketEdit->text () == "")
    symboldata.market = "NONE";
  else
    symboldata.market = ui->marketEdit->text ().replace ("'", " ").trimmed ();

  symboldata.csvfile =  ui->filenameEdit->text ();
  symboldata.symbol = ui->symbolEdit->text ().replace ("'", " ").trimmed ();
  symboldata.name = ui->nameEdit->text ().replace ("'", " ").trimmed ();
  symboldata.timeframe = ui->timeframesComboBox->currentText ();
  symboldata.currency = ui->currencyComboBox->currentText ();
  if (symboldata.csvfile.endsWith (".xls", Qt::CaseInsensitive))
    symboldata.source = "XLS";
  else
    symboldata.source = "CSV";
  symboldata.format = ui->formatComboBox->currentText ();
  symboldata.tablename =  symboldata.symbol % "_" %
                          symboldata.market % "_" %
                          symboldata.source % "_";

  /*
  if (symboldata.adjust)
    symboldata.tablename += "ADJUSTED_";
  else
    symboldata.tablename += "UNADJUSTED_";
  */

  symboldata.tablename += symboldata.timeframe;
  symboldata.tmptablename = "TMP_" % symboldata.tablename;

  if (ui->fileposCheckBox->checkState () == Qt::Checked)
    symboldata.dnlstring = symboldata.csvfile;
  else
    symboldata.dnlstring = "";

  err = csv2sqlite (&symboldata, "CREATE");
  if (err != CG_ERR_OK)
  {
    setGlobalError(err, __FILE__, __LINE__);
    showMessage (errorMessage (err));
    widgetsSetEnabled (true);
    ui->progressBar->setValue (0);
    return;
  }
  else
    showMessage ("Import complete.");

  appRestoreOverrideCursor (this);
  widgetsSetEnabled (true);
  ui->progressBar->setValue (0);
}
