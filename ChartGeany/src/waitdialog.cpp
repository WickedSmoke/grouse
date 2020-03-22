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
#include "ui_waitdialog.h"
#include "common.h"

// constructor
WaitDialog::WaitDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::WaitDialog)
{
  ui->setupUi (this);
  this->setWindowTitle (QStringLiteral ("Please Wait"));
  this->setWindowFlags(((this->windowFlags() | Qt::CustomizeWindowHint)
                        & ~Qt::WindowCloseButtonHint));
  // correctWidgetFonts (this);
}

// destructor
WaitDialog::~WaitDialog ()
{
  delete ui;
}

// set message
void
WaitDialog::setMessage (QString message)
{
  ui->messageLabel->setText (message);
}

