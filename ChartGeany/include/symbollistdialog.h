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

#ifndef SYMBOLLISTDIALOG_H
#define SYMBOLLISTDIALOG_H

#include <QStringList>
#include <QDialog>
#include "defs.h"

namespace Ui
{
  class SymbolListDialog;
}

class SymbolListDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SymbolListDialog(QWidget *parent = 0);
  ~SymbolListDialog();

  void setTableName (QString tname)
  {
    tablename = tname;
  }; // set table name

private:
  Ui::SymbolListDialog * ui;
  
  QString symFilter; // symbol filter;
  QStringList cheadersList; // list of columns' headers
  QString tablename; // symbol table name
  QString lasttableused; // last symbol table used
  
  void reloadSymbols (); // reload symbols

private slots:
  void closeButton_clicked (void);
  void upButton_clicked (void);
  void downButton_clicked (void);
  void selectButton_clicked (void);
  void symbol_double_clicked (void);
  void filter_combol_changed (const QString &);

protected:
  virtual void showEvent (QShowEvent * event);
};

#endif // SYMBOLLISTDIALOG_H
