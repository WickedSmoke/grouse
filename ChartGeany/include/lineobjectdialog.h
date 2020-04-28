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

#ifndef LINEOBJECTDIALOG_H
#define LINEOBJECTDIALOG_H

#include "qtachart_object.h"


class QPushButton;
class appColorDialog;

class LineObjectDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit LineObjectDialog (QWidget *parent = 0);
  ~LineObjectDialog (void);

  bool modify (QTACObject *); // modify or remove existing. returns true on modify, false on delete

private:
  QColor color;			// text's color
  QPixmap *pixmap;		// color button's pixmap
  QIcon *icon;			// color button's icon
  QPushButton* colorButton;
  appColorDialog *colorDialog; // text's color dialog
  bool removed;

private slots:
  void color_clicked();
  void colorAccepted();
  void colorRejected();
  void removeClicked();
};


#endif // LINEOBJECTDIALOG_H
