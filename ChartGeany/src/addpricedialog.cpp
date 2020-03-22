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
#include "ui_addpricedialog.h"
#include "addpricedialog.h"
#include "portfolio.h"
#include "common.h"

// constructor
AddPriceDialog::AddPriceDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::AddPriceDialog)
{
  QPalette pal;
  const QString
  stylesheet = "background: transparent;",
  stylesheet2 = "background: transparent; background-color: white;",
  stylesheet3 = "selection-background-color: blue";
  QStringList feedlist;

  ui->setupUi (this);

  ui->symbolLbl->setStyleSheet (stylesheet);
  ui->dateLbl->setStyleSheet (stylesheet);
  ui->volumeLbl->setStyleSheet (stylesheet);
  ui->priceLbl->setStyleSheet (stylesheet);
  ui->changeLbl->setStyleSheet (stylesheet);

  ui->symbolEdit->setStyleSheet (stylesheet2);
  ui->dateEdit->setStyleSheet (stylesheet2);
  ui->volumeEdit->setStyleSheet (stylesheet2);
  ui->priceEdit->setStyleSheet (stylesheet2);
  ui->changeEdit->setStyleSheet (stylesheet2);

  ui->volumeEdit->setValidator (new QDoubleValidator(0, 999999999, 2, this));
  ui->priceEdit->setValidator (new QDoubleValidator(0, 999999999, 2, this));
  ui->changeEdit->setValidator (new QDoubleValidator(0, 999999999, 2, this));

  ui->dateEdit->setDate(QDate::currentDate());
  ui->dateEdit->calendarWidget()->setStyleSheet ("background-color: lightslategray;");

  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Save);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));

  pal.setColor (backgroundRole (), Qt::white);
  setPalette (pal);

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
AddPriceDialog::~AddPriceDialog ()
{
  delete ui;
}


// set the defaults
void
AddPriceDialog::setDefaults (QString symbol, QString price, QString change, QString volume)
{
  ui->symbolEdit->setText (symbol);
  ui->priceEdit->setText (price);
  ui->changeEdit->setText (change);
  ui->volumeEdit->setText (volume);
}

/// Signals

// save
void
AddPriceDialog::ok_clicked ()
{
  QString SQL = "";
  qreal cprc = 0;
  int rc;

  if (ui->changeEdit->text ().trimmed ().toFloat () != 0 ||
      ui->priceEdit->text ().trimmed ().toFloat () != 0)
  {
    cprc = (ui->changeEdit->text ().trimmed ().toFloat () /
            (ui->priceEdit->text ().trimmed ().toFloat () +
             (ui->changeEdit->text ().trimmed ().toFloat () * -1))) * 100;
  }

  SQL += "DELETE FROM prices WHERE symbol = '" % ui->symbolEdit->text ().trimmed () +
         "' AND feed = 'NONE';";
  SQL += "INSERT INTO prices (symbol, price, change, volume, prcchange, date) VALUES ('" %
         ui->symbolEdit->text ().trimmed () % "','" %
         ui->priceEdit->text ().trimmed () % "','" %
         ui->changeEdit->text ().trimmed () % "','" %
         ui->volumeEdit->text ().trimmed () % "','" %
         QString::number ((qreal) cprc, 'f', 2) % "%','" %
         ui->dateEdit->date ().toString ("yyyy-MM-dd") % "');";
  rc = updatedb (SQL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_WRITE_FILE, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_WRITE_FILE));
    return;
  }

  (qobject_cast <Portfolio *> (parent ()))->reloadPositions ();
  this->hide ();

}

// close
void
AddPriceDialog::cancel_clicked ()
{

  this->hide ();
}

// show
void
AddPriceDialog::showEvent (QShowEvent * event)
{
  Q_UNUSED (event);
}
