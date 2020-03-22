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
 
// Full implementation of QTACData
#include "optsize.h"
#include <QFont>
#include <QScrollBar>
#include "qtachart_core.h"
#include "ui_qtacdata.h"
#include "qtachart_data.h"
#include "qtachart.h"
#include "defs.h"

// constructor
QTACData::QTACData (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACData)
{
  QFont font;

  ui->setupUi (this);
  ui->upToolButton->setToolTip (TOOLTIP % QStringLiteral ("Scroll Up</span>"));
  ui->downToolButton->setToolTip (TOOLTIP % QStringLiteral ("Scroll Down</span>"));

  font = ui->textEditData->font ();
  font.setPixelSize (14 + FONT_PIXELSIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setWeight (QFont::DemiBold);
  ui->textEditData->setFont (font);

  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));

  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);
}

// destructor
QTACData::~QTACData ()
{
  delete ui;
}

void
QTACData::setData (QString datatext)
{
  ui->textEditData->setPlainText (datatext);
}

// resize
void
QTACData::resizeEvent (QResizeEvent * event)
{
  if (event->oldSize () == event->size ())
    return;

  ui->textEditData->resize (500, height () - 34);
  ui->textEditData->move ((width () - 560) / 2,
                          ((height () - ui->textEditData->height ()) / 2) - 10);
  ui->upToolButton->move (((width () - 560) / 2) + 510,
                          ((height () - ui->textEditData->height ()) / 2) - 10);
  ui->downToolButton->move (((width () - 560) / 2) + 510,
                            (((height () - ui->textEditData->height ()) / 2) - 10) + 60);
}

// down
void
QTACData::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->textEditData->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->textEditData->setFocus (Qt::MouseFocusReason);
}

// up
void
QTACData::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->textEditData->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->textEditData->setFocus (Qt::MouseFocusReason);
}

