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
 
#include "optsize.h"
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QShowEvent>
#include "appColorDialog.h"
#include "common.h"

// constructor
appColorDialog::appColorDialog (QWidget * parent)
    : QColorDialog(parent)
{
  setOption (QColorDialog::DontUseNativeDialog, true);

  connect (this, SIGNAL (accepted ()), this, SLOT (color_accepted ()));
  connect (this, SIGNAL (rejected ()), this, SLOT (color_rejected ()));
  connect (this, SIGNAL (finished (int)), this, SLOT (dialog_finished (int)));

#ifndef GUI_DESKTOP
  setWindowIcon (QIcon (QStringLiteral (":/png/images/icons/PNG/cglogo.png")));
  setStyleSheet (QStringLiteral ("background: transparent; background-color:white;"));
  correctWidgetFonts (this);
#endif
}


// selected color
QColor
appColorDialog::appSelectedColor (bool *ok) const
{
  *ok = okflag;
  if (okflag)
    appSetOverrideCursor (this, QCursor (Qt::PointingHandCursor));
  return selectedColor ();
}

// color accepted
void
appColorDialog::color_accepted(void) NOEXCEPT
{
  okflag = true;
}

// color rejected
void
appColorDialog::color_rejected (void) NOEXCEPT
{
  okflag = false;

}

// dialog finished
void
appColorDialog::dialog_finished (int result)
{
  if (result == 0)
    setCurrentColor (keepCurrentColor);
}

// show event
void
appColorDialog::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())
    return;

  keepCurrentColor = currentColor ();
  okflag = false;
}
