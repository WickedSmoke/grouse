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
 
#include "optsize.h"
#include "ui_progressdialog.h"
#include "common.h"

// constructor
ProgressDialog::ProgressDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::ProgressDialog)
{
  ui->setupUi (this);
  cancelrequest = false;
  setWindowTitle (QStringLiteral ("Please Wait"));
  setWindowFlags (((Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint) & ~Qt::WindowMinimizeButtonHint));
  connect (ui->cancelRequestButton, SIGNAL (clicked ()), this,
           SLOT (cancelRequestButton_clicked ()));
  ui->progressBar->setValue (0);
  // correctWidgetFonts (this);
}

// destructor
ProgressDialog::~ProgressDialog ()
{
  delete ui;
}


// set message
void
ProgressDialog::setMessage (QString message)
{
  cancelrequest = false;
  ui->cancelRequestButton->setEnabled (true);
  ui->messageLabel->setText (message);
}


QProgressBar *
ProgressDialog::getProgressBar ()
{

  return ui->progressBar;
}

/// slots
// cancel request button
void
ProgressDialog::cancelRequestButton_clicked (void)
{
  cancelrequest = true;
  ui->cancelRequestButton->setEnabled (false);
}
