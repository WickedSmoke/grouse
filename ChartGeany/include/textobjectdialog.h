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

#ifndef TEXTOBJECTDIALOG_H
#define TEXTOBJECTDIALOG_H

#include <QLabel>
#include <QGraphicsTextItem>
#include "appColorDialog.h"
#include "defs.h"

namespace Ui
{
  class TextObjectDialog;
}

class TextObjectDialog : public QDialog
{
  Q_OBJECT
public:
  explicit TextObjectDialog (QWidget *parent = 0);
  ~TextObjectDialog (void);

  QLabel *getLabel (void) NOEXCEPT;	// get the text's QLabel
  void create (void);		// create new
  bool modify (QGraphicsTextItem *); // modify or remove existing. returns true on modify, false on delete


private:
  Ui::TextObjectDialog *ui;
  QColor color;			// text's color
  QPixmap *pixmap;		// color button's pixmap
  QIcon *icon;			// color button's icon
  appColorDialog *colorDialog; // text's color dialog
  QGraphicsTextItem *textitem;  // text item to modify
  qint32 operation;	// Create or Modify

  enum
  {
    Create,
    Modify
  };

private slots:
  void text_changed(QString);
  void fontsize_changed (int);
  void fontfamily_changed(int);
  void fontweight_changed (bool);
  void text_accepted(void);
  void text_rejected(void);
  void color_clicked (void);
  void colorDialog_accepted (void);
  void colorDialog_rejected (void) NOEXCEPT;

protected:
  virtual void showEvent (QShowEvent * event);
};


#endif // TEXTOBJECTDIALOG_H
