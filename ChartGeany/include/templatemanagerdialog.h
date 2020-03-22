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

#ifndef TEMPLATEMANAGERDIALOG_H
#define TEMPLATEMANAGERDIALOG_H

#include <QDialog>
#include "defs.h"

namespace Ui
{
  class TemplateManagerDialog;
}

class TemplateManagerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TemplateManagerDialog (QWidget *parent = 0);
  ~TemplateManagerDialog(void);

  QStringList	description;	// template descriptions
  QStringList	tablename;		// template tablenames
  QStringList	id;				// template id;
  QStringList	sqlstatement;	// template creation sql statement

  void loadtemplate (void *chart);		// load template
  void savetemplate (void *chart);		// save template
  void setReferenceChart (void *chart); // set the reference chart
  QString qtachart2sql (QString tableKey); // sql from object list
  CG_ERRORS attachtemplate (QString tablename);	// attach template

private:
  Ui::TemplateManagerDialog * ui;
  bool savemode;					// true if in save mode, false otherwise
  void *referencechart;				// reference chart
  QString selectedtable;			// table for selected template

  CG_ERRORS newtemplate (void);	// new template
  CG_ERRORS updatetemplate (QString tablename);	// update existing template
  CG_ERRORS deletetemplate (int id, QString tablename);	// delete template
  CG_ERRORS loaddir (void);	    // load template directory
  void cleartable (void);		// clear the table
  void fillcolumn (QStringList list, int col); // fill column with values

private slots:
  void ok_clicked (void);
  void cancel_clicked (void);
  void cell_clicked (int row, int column);
  void del_clicked (void);
  void downButton_clicked (void);
  void upButton_clicked (void);
  void importButton_clicked (void);
  void exportButton_clicked (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void showEvent (QShowEvent * event);
};

#endif // TEMPLATEMANAGERDIALOG_H
