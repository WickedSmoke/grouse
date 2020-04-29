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


class QBoxLayout;
class QFormLayout;
class QPushButton;
class appColorDialog;

class LineObjectDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit LineObjectDialog (QWidget *parent = 0);
  ~LineObjectDialog (void);

  void enableRemove( bool on );
  void setColor( const QColor& );

  bool modify (QTACObject *); // modify or remove existing. returns true on modify, false on delete

protected:
  void createColorButton();

protected slots:
  virtual void colorAccepted();

private:
  QPixmap *pixmap;		// color button's pixmap
  QIcon *icon;			// color button's icon
  QPushButton* colorButton;
  appColorDialog *colorDialog; // text's color dialog
  QPushButton* removeButton;

protected:
  QFormLayout* form;
  QDialogButtonBox* bbox;
  QColor color;
  bool removed;

private slots:
  void color_clicked();
  void colorRejected();
  void removeClicked();
};


class QCheckBox;
class QComboBox;
class QFontComboBox;
class QLabel;
class QLineEdit;

class TextObjectDialog : public LineObjectDialog
{
  Q_OBJECT

public:
  TextObjectDialog(QWidget *parent = 0);

  QLabel* getLabel() NOEXCEPT;	    // get the text's QLabel
  bool modify(QGraphicsTextItem *); // modify or remove existing

protected slots:
  virtual void colorAccepted();

private slots:
  void textChanged(const QString&);
  void sizeChanged(int);
  void familyChanged(int);
  void weightChanged(bool);

private:
  void updatePreviewColor();

  QFontComboBox *familyCombo;
  QComboBox *sizeCombo;
  QCheckBox *boldCheck;
  QLineEdit* textEdit;
  QLabel* preview;
};


#endif // LINEOBJECTDIALOG_H
