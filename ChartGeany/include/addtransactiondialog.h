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

#ifndef ADDTRANSACTIONDIALOG_H
#define ADDTRANSACTIONDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include "defs.h"

namespace Ui
{
  class AddTransactionDialog;
}

class AddTransactionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddTransactionDialog (int pfid, QWidget *parent = 0);
  ~AddTransactionDialog (void);

  QComboBox *typeComboBox;
  QDateEdit *dateEdit;
  QLineEdit *symbolEdit;
  QLineEdit *quantityEdit;
  QLineEdit *priceEdit;
  QLineEdit *commissionEdit;
  QComboBox *commtypeComboBox;
  QPlainTextEdit *notesEdit;

  void setAddMode ()
  {
    addmode = true;
  };		// set the dialog to add mode
  void setEditMode (int id)
  {
    tr_id = id;
    addmode = false;
  };	// set the dialog to edit mode

private:
  Ui::AddTransactionDialog *ui;
  bool addmode;				// true is add mode, false is edit mode
  int pf_id;	         	// portfolio id
  int tr_id;				// transaction id

private slots:
  void ok_clicked (void);
  void cancel_clicked (void);
  void type_changed (const QString & text);

protected:
  virtual void showEvent (QShowEvent * event);
};

#endif // ADDTRANSACTIONDIALOG_H
