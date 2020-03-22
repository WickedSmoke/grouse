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
#include <QCalendarWidget>
#include "ui_addtransactiondialog.h"
#include "addtransactiondialog.h"
#include "portfolio.h"
#include "common.h"

// constructor
AddTransactionDialog::AddTransactionDialog (int pfid, QWidget * parent):
  QDialog (parent), ui (new Ui::AddTransactionDialog)
{
  QPalette pal;
  const QString
  stylesheet = "background: transparent;",
  stylesheet2 = "background: transparent; background-color: white;",
  stylesheet3 = "selection-background-color: blue";
  QStringList feedlist;

  pf_id = pfid;
  addmode = true;
  ui->setupUi (this);

  typeComboBox = ui->typeComboBox;
  dateEdit = ui->dateEdit;
  symbolEdit = ui->symbolEdit;
  quantityEdit = ui->quantityEdit;
  priceEdit = ui->priceEdit;
  commissionEdit = ui->commissionEdit;
  commtypeComboBox = ui->commtypeComboBox;
  notesEdit = ui->notesEdit;

  ui->typeLbl->setStyleSheet (stylesheet);
  ui->symbolLbl->setStyleSheet (stylesheet);
  ui->dateLbl->setStyleSheet (stylesheet);
  ui->quantityLbl->setStyleSheet (stylesheet);
  ui->priceLbl->setStyleSheet (stylesheet);
  ui->commissionLbl->setStyleSheet (stylesheet);
  ui->notesLbl->setStyleSheet (stylesheet);
  ui->symbolEdit->setStyleSheet (stylesheet2);

  ui->dateEdit->setStyleSheet (stylesheet2);
  ui->quantityEdit->setStyleSheet (stylesheet2);
  ui->quantityEdit->setValidator( new QDoubleValidator(0, 999999999, 2, this));

  ui->priceEdit->setStyleSheet (stylesheet2);
  ui->priceEdit->setValidator( new QDoubleValidator(0, 999999999, 2, this));

  ui->commissionEdit->setStyleSheet (stylesheet2);
  ui->commissionEdit->setValidator( new QDoubleValidator(0, 999999999, 2, this));

  ui->notesEdit->setStyleSheet (stylesheet2);
  ui->typeComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->commtypeComboBox->setStyleSheet (stylesheet2 + stylesheet3);
  ui->typeComboBox->addItems (ComboItems->transactiontypeList);
  ui->commtypeComboBox->addItems (ComboItems->commissiontypeList);
  ui->dateEdit->setDate(QDate::currentDate());
  ui->dateEdit->calendarWidget()->setStyleSheet ("background-color: lightslategray;");

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Save);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));
  connect (ui->typeComboBox,
           SIGNAL (currentIndexChanged (const QString &)), this,
           SLOT (type_changed (const QString &)));

  pal.setColor (backgroundRole (), Qt::white);
  setPalette (pal);

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
AddTransactionDialog::~AddTransactionDialog ()
{
  delete ui;
}

/// Signals

// save
void
AddTransactionDialog::ok_clicked ()
{
  QString SQL;
  int rc;

  if (ui->symbolEdit->text ().replace ("'", " ").trimmed ().size () == 0 &&
      (ui->typeComboBox->currentText () == "BUY" ||
       ui->typeComboBox->currentText () == "SELL" ||
       ui->typeComboBox->currentText () == "DIVIDEND" ||
       ui->typeComboBox->currentText () == "DIVIDEND PER SHARE"))
  {
    showMessage ("No symbol.");
    return;
  }

  if (ui->quantityEdit->text().contains (QStringLiteral (",")) ||
      ui->priceEdit->text ().contains (QStringLiteral (",")) ||
      ui->commissionEdit->text().contains (QStringLiteral (",")))
  {
    showMessage ("Commas not allowed in numbers.");
    return;
  }

  if (ui->quantityEdit->text().toDouble () <= 0)
  {
    showMessage ("No quantity or amount.");
    return;
  }

// add
  if (addmode)
  {
    SQL = "INSERT INTO transactions \
           (PF_ID, TRTYPE, SYMBOL, TR_DATE, QUANTITY, NOTES, \
            PRICE, COMMISSION,  COMMTYPE) VALUES (" +
          QString::number (pf_id) + ", " +
          QStringLiteral ("'") + ui->typeComboBox->currentText () + "', " +
          QStringLiteral ("'") + ui->symbolEdit->text ().replace ("'", " ").trimmed ().toUpper () + "', " +
          QStringLiteral ("'") + ui->dateEdit->date ().toString ("yyyy-MM-dd") + "', " +
          ""  + ui->quantityEdit->text().replace ("'", " ").trimmed () + ", " +
          QStringLiteral ("'") + ui->notesEdit->toPlainText ().replace ("'", " ").trimmed () + "', " +
          "" + ui->priceEdit->text ().replace ("'", " ").trimmed () + ", " +
          ""  + ui->commissionEdit->text().replace ("'", " ").trimmed () + ", " +
          QStringLiteral ("'") + ui->commtypeComboBox->currentText () + "'); ";

    rc = updatedb (SQL);
    if (rc != SQLITE_OK)
    {
      setGlobalError(CG_ERR_TRANSACTION, __FILE__, __LINE__);
      showMessage (errorMessage (CG_ERR_TRANSACTION));
      return;
    }

    (qobject_cast <Portfolio *> (parent ()))->reloadTransaction (-1);
    this->hide ();
    return;
  }

// edit mode
  SQL = "UPDATE transactions SET \
         SYMBOL = '"  + ui->symbolEdit->text ().replace ("'", " ").trimmed ().toUpper () + "', "  +
        "TR_DATE = '" + ui->dateEdit->date ().toString ("yyyy-MM-dd") + "', " +
        "QUANTITY = " + ui->quantityEdit->text().replace ("'", " ").trimmed ().toUpper () + ", " +
        "PRICE = " + ui->priceEdit->text ().replace ("'", " ").trimmed ().toUpper () + ", " +
        "COMMISSION = " + ui->commissionEdit->text().replace ("'", " ").trimmed ().toUpper () + ", " +
        "NOTES = '" + ui->notesEdit->toPlainText ().replace ("'", " ").trimmed () + "', " +
        "COMMTYPE = '" + ui->commtypeComboBox->currentText () + "', " +
        "TRTYPE = '" + ui->typeComboBox->currentText () + "' " +
        "WHERE TR_ID = " + QString::number (tr_id) + ";";

  rc = updatedb (SQL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_WRITE_FILE));

    return;
  }

  (qobject_cast <Portfolio *> (parent ()))->reloadTransaction (tr_id);
  this->hide ();
}

// close
void
AddTransactionDialog::cancel_clicked ()
{

  this->hide ();
}

// type changed
void
AddTransactionDialog::type_changed (const QString & type)
{
  if (type == "BUY" || type == "SELL")
  {
    ui->symbolEdit->setEnabled (true);
    ui->priceEdit->setEnabled (true);
    ui->symbolLbl->setEnabled (true);
    ui->priceLbl->setEnabled (true);
    ui->quantityLbl->setText ("Quantity");
  }

  if (type == "CASH IN" || type == "CASH OUT")
  {
    ui->symbolEdit->setEnabled (false);
    ui->priceEdit->setEnabled (false);
    ui->symbolLbl->setEnabled (false);
    ui->priceLbl->setEnabled (false);
    ui->quantityLbl->setText ("Amount");
  }

  if (type == "DIVIDEND" || type == "DIVIDEND PER SHARE")
  {
    ui->symbolEdit->setEnabled (true);
    ui->priceEdit->setEnabled (false);
    ui->symbolLbl->setEnabled (true);
    ui->priceLbl->setEnabled (false);
    ui->quantityLbl->setText ("Amount");
  }

}

/// Events
// transaction id call back
static int
sqlcb_transactions (void *data, int argc, char **argv, char **column)
{
  AddTransactionDialog *u = static_cast <AddTransactionDialog *> (data);

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == "SYMBOL")
      u->symbolEdit->setText (QString (argv[counter]));

    if (colname == "QUANTITY")
      u->quantityEdit->setText (QString (argv[counter]));

    if (colname == "PRICE")
      u->priceEdit->setText (QString (argv[counter]));

    if (colname == "COMMISSION")
      u->commissionEdit->setText (QString (argv[counter]));

    if (colname == "NOTES")
      u->notesEdit->setPlainText (QString (argv[counter]));

    if (colname == "TRTYPE")
      u->typeComboBox->setCurrentIndex
      (u->typeComboBox->findText (QString (argv[counter])));

    if (colname == "COMMTYPE")
      u->commtypeComboBox->setCurrentIndex
      (u->commtypeComboBox->findText (QString (argv[counter])));

    if (colname == "TR_DATE")
      u->dateEdit->setDate (QDate::fromString (QString (argv[counter]),
                            QString ("yyyy-MM-dd")));
  }

  return 0;
}

// show
void
AddTransactionDialog::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())
    return;

// add
  if (addmode)
  {
    this->setWindowTitle ("Add New Transaction");
    this->setWindowIcon (QIcon (QString (":/png/images/icons/PNG/Add_Symbol.png")));
    ui->symbolEdit->setText ("");
    ui->dateEdit->setDate(QDate::currentDate());
    ui->quantityEdit->setText ("0");
    ui->priceEdit->setText ("0");
    ui->commissionEdit->setText ("0");
    return;
  }

// edit
  QString SQL;
  int rc;

  this->setWindowTitle ("Edit Transaction");
  this->setWindowIcon (QIcon (QString (":/png/images/icons/PNG/Pencil_2.png")));
  SQL = "SELECT * FROM transactions WHERE tr_id = " % QString::number(tr_id) % ";";
  rc = selectfromdb(SQL.toUtf8(), sqlcb_transactions, static_cast <void*> (this));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_ACCESS_DATA, __FILE__, __LINE__);
    showMessage ("Cannot retrieve transaction information.");
    return;
  }
}
