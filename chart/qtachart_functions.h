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

#ifndef QTACHART_FUNCTIONS_H
#define QTACHART_FUNCTIONS_H

#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include "qtachart.h"
#include "dynparamsdialog.h"

namespace Ui
{
  class QTACFunctions;
}

class QTACFunctions:public QWidget
{
  Q_OBJECT
public:
  explicit QTACFunctions (QWidget * parent = 0);
  ~QTACFunctions (void);

  void setReferenceChart (void *chart);     // sets the reference chart
  void *getReferenceChart (void);           // get the reference chart
  void addIndicator (DynParamsDialog *paramDialog); // add indicator on chart

private:
  Ui::QTACFunctions * ui;

  // variables
  QTAChart *parentchart;
  QGridLayout *layout;
  QList < QPushButton *> Button;            // the list of buttons
  QLabel *textLbl;                          // QLabel that holds label/text
  int button_width;                         // width of button
  int button_height;                        // height of button
  bool init;                                // true initialized, otherwise false
  void *referencechart;                     // reference chart

  // functions
  QPushButton *addButton (QString text);    // add a button
  void createButtons (void);

private slots:
  void button_clicked (void);
  void function_accepted (void);
  void function_rejected (void);

protected:
  virtual void resizeEvent (QResizeEvent * event);
};

#endif // QTACHART_FUNCTIONS_H
