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

#ifndef MODULEMANAGERDIALOG_H
#define MODULEMANAGERDIALOG_H

#include <QDialog>
#include "defs.h"

namespace Ui
{
  class ModuleManagerDialog;
}

class ModuleManagerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ModuleManagerDialog (QWidget *parent = 0);
  ~ModuleManagerDialog (void);
  void *attachModule (void *data, QString modid); // attach a module on chart

private:
  Ui::ModuleManagerDialog * ui;

  void reloadModules ();					// reload the modules
  void cleartable ();						// clear QTableWidget

private slots:
  void addButton_clicked (void);
  void importButton_clicked (void);
  void deleteButton_clicked (void);
  void exitButton_clicked (void);
  void upButton_clicked (void);
  void downButton_clicked (void);
  void exportButton_clicked (void);
  void module_double_clicked (void);
  void runButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void showEvent (QShowEvent * event);
  virtual void changeEvent (QEvent * event);
  virtual void keyPressEvent (QKeyEvent * event);
};

#endif // MODULEMANAGERDIALOG_H
