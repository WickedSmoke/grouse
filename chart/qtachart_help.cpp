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
 
// Full implementation of QTACHelp
#include "optsize.h"
#include <QScrollBar>
#include <QResizeEvent>
#include "ui_qtachelp.h"
#include "qtachart_help.h"
#include "qtachart.h"
#include "defs.h"
#include "common.h"

// constructor
QTACHelp::QTACHelp (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACHelp)
{
  ui->setupUi (this);
  ui->upToolButton->setToolTip (TOOLTIP % QStringLiteral ("Scroll Up</span>"));
  ui->downToolButton->setToolTip (TOOLTIP % QStringLiteral ("Scroll Down</span>"));

  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));

  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);
}

// destructor
QTACHelp::~QTACHelp ()
{
  delete ui;
}

// resize
void
QTACHelp::resizeEvent (QResizeEvent * event)
{
  if (event->oldSize () == event->size ())
    return;

  ui->textEditHelp->resize (500, height () - 34);
  ui->textEditHelp->move ((width () - 560) / 2,
                          ((height () - ui->textEditHelp->height ()) / 2) - 10);
  ui->upToolButton->move (((width () - 560) / 2) + 510,
                          ((height () - ui->textEditHelp->height ()) / 2) - 10);
  ui->downToolButton->move (((width () - 560) / 2) + 510,
                            (((height () - ui->textEditHelp->height ()) / 2) - 10) + 60);
}

// down
void
QTACHelp::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->textEditHelp->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->textEditHelp->setFocus (Qt::MouseFocusReason);
}

// up
void
QTACHelp::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->textEditHelp->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->textEditHelp->setFocus (Qt::MouseFocusReason);
}

