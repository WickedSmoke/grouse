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

#ifndef QTACHART_OBJECTS_H
#define QTACHART_OBJECTS_H

#include <QWidget>
#include "defs.h"
#include "qtachart_object.h"

namespace Ui
{
  class QTACObjects;
}

class QTACObjects:public QWidget
{
Q_OBJECT public:
  explicit QTACObjects (QWidget * parent = 0);
  ~QTACObjects ();

  void setReferenceChart (void *chart);		// sets the reference chart

private:
  Ui::QTACObjects * ui;

  void *referencechart;						// reference chart
  ObjectVector Object;						// vector of objects

  void loadObjectVector ();					// load the Object Vector
private slots:
  void downButton_clicked (void);
  void upButton_clicked (void);
  void editButton_clicked (void);
  void clearButton_clicked (void);
  void loadButton_clicked (void);
  void saveButton_clicked (void);
  void listWidget_doubleClicked ( const QModelIndex & index );
protected:
  virtual void resizeEvent (QResizeEvent * event);
  virtual void showEvent (QShowEvent * event);

};

#endif // QTACHART_OBJECTS_H
