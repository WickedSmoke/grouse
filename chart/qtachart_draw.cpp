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
 
// Full implementation of QTACProperties
#include "common.h"
#include "qtachart_core.h"
#include "ui_qtacdraw.h"
#include "qtachart_draw.h"

// constructor
QTACDraw::QTACDraw (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACDraw)
{
  QPushButton *btn;

  ui->setupUi (this);

  button_width = 200;
  button_height = 40;

  btn =  addButton (QStringLiteral ("Label"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (labelButton_clicked ()));

  btn = addButton (QStringLiteral ("Trailing Text"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (textButton_clicked ()));

  btn = addButton (QStringLiteral ("Horizontal Line"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (hlineButton_clicked ()));

  btn = addButton (QStringLiteral ("Vertical Line"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (vlineButton_clicked ()));

  btn = addButton (QStringLiteral ("Trend Line"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (slineButton_clicked ()));

  // btn = addButton ("Channel");
  // connect (btn, SIGNAL (clicked ()), this, SLOT (channelButton_clicked ()));

  btn = addButton (QStringLiteral ("Fibonacci"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (fiboButton_clicked ()));

  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

}

// destructor
QTACDraw::~QTACDraw ()
{
  delete ui;
}

// add a button
QPushButton *
QTACDraw::addButton (QString text)
{
  QFont fnt;
  QPushButton *btn;
  const QString
  stylesheet =
    QStringLiteral ("background: transparent; border: 1px solid transparent;border-color: darkgray;");

  btn = new QPushButton (text, this);
  btn->resize (button_width, button_height);
  fnt = btn->font ();
  fnt.setPixelSize (16);
  fnt.setBold (true);
  btn->setFont (fnt);
  btn->setStyleSheet (stylesheet);
  btn->setAutoFillBackground (false);
  btn->setFocusPolicy (Qt::NoFocus);
  Button += btn;
  return btn;
}

// set the reference chart
void
QTACDraw::setReferenceChart (void *chart)
{
  referencechart = static_cast <QTAChart*> (chart);
}

// resize
void
QTACDraw::resizeEvent (QResizeEvent * event)
{
  int max, w, h;

  if (event->oldSize () == event->size ())
    return;

  max = Button.size ();
  w = (button_width * 2) + 10;
  h = (height () - ((max/2 + 1) * (button_height + 10))) / 2;

  for (qint32 counter = 0; counter < max; counter ++)
  {
    if ((counter % 2) == 0)
      Button[counter]->move ((width () - w) / 2,
                             ((counter / 2) * (button_height + 10)) + h);
    else
      Button[counter]->move (((width () - w) / 2) + button_width + 10,
                             ((counter / 2) * (button_height + 10)) + h);
  }
}

void
QTACDraw::labelButton_clicked (void)
{
  referencechart->addMarkerLabel();
}

void
QTACDraw::textButton_clicked (void)
{
  referencechart->addMarkerTrailingText();
}

void
QTACDraw::hlineButton_clicked (void)
{
  referencechart->addMarkerHLine();
}

void
QTACDraw::vlineButton_clicked (void)
{
  referencechart->addMarkerVLine();
}

void
QTACDraw::slineButton_clicked (void)
{
  referencechart->addMarkerTrendLine();
}

/*
void
QTACDraw::channelButton_clicked (void)
{
}
*/

void
QTACDraw::fiboButton_clicked (void)
{
  referencechart->addMarkerFibonacci();
}
