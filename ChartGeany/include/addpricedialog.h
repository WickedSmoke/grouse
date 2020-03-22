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

#ifndef ADDPRICEDIALOG_H
#define ADDPRICEDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include "defs.h"

namespace Ui
{
  class AddPriceDialog;
}

class AddPriceDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddPriceDialog (QWidget *parent = 0);
  ~AddPriceDialog (void);

  void setDefaults (QString symbol,
                    QString price,
                    QString change,
                    QString volume);	// set the symbol

private:
  Ui::AddPriceDialog *ui;

private slots:
  void ok_clicked (void);
  void cancel_clicked (void);

protected:
  virtual void showEvent (QShowEvent * event);
};

#endif // ADDPRICEDIALOG_H
