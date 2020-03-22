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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QProgressBar>

namespace Ui
{
  class ProgressDialog;
}

class ProgressDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ProgressDialog (QWidget *parent = 0);
  ~ProgressDialog (void);

  QProgressBar *getProgressBar (void);
  void setMessage (QString message);
  bool getCancelRequestFlag () const 
  {
    return cancelrequest;
  }; // get the cancel request flag;
private:
  Ui::ProgressDialog *ui;
  bool cancelrequest;		// cancel request flag

private slots:
  void cancelRequestButton_clicked (void);
};


#endif // PROGRESSDIALOG_H
