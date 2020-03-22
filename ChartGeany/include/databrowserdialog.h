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

#ifndef DATABROWSERDIALOG_H
#define DATABROWSERDIALOG_H

#include <QDialog>
#include "defs.h"

namespace Ui
{
  class DataBrowserDialog;
}

class DataBrowserDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DataBrowserDialog (QWidget *parent = 0);
  ~DataBrowserDialog (void);

  void setTableName (QString tname)
  {
    tablename = tname;
  }; // set table name

private:
  Ui::DataBrowserDialog * ui;

  QStringList cheadersList; // list of columns' headers
  QString tablename; // table name

private slots:
  void closeButton_clicked (void);
  void upButton_clicked (void);
  void downButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void showEvent (QShowEvent * event);
};

#endif // DATABROWSERDIALOG_H
