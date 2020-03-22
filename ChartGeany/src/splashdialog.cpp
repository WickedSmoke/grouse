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
#include "ui_splashdialog.h"
#include "splashdialog.h"
#include "appdata.h"
#include "common.h"

// constructor
SplashDialog::SplashDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::SplashDialog)
{
  QString appname, version;
  ui->setupUi (this);

  appname = APPNAME;
  version =  QStringLiteral ("Version ") %
             QString::number(VERSION_MAJOR,10) % QStringLiteral (".") %
             QString::number(VERSION_MINOR,10) % QStringLiteral (".") %
             QString::number(VERSION_PATCH,10);
  setWindowFlags (Qt::SplashScreen);
  setWindowTitle (appname);
  ui->appnameLabel->setText (appname);
  ui->versionLabel->setText (version);
  ui->osLabel->setText (fullOperatingSystemVersion ());

  correctWidgetFonts (this);

  if (parent != NULL)
    setParent (parent);
}

// destructor
SplashDialog::~SplashDialog ()
{
  delete ui;
}
