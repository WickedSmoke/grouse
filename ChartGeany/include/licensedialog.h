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

#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#include <QDialog>

namespace Ui
{
  class LicenseDialog;
}

class LicenseDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LicenseDialog(QWidget *parent = 0);
  ~LicenseDialog (void);

private:
  Ui::LicenseDialog *ui;

private slots:
  void close_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void changeEvent(QEvent * event);
  virtual void hideEvent(QHideEvent * event);

};

#endif // LICENSEDIALOG_H
