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

#ifndef APPCOLORDIALOG_H
#define APPCOLORDIALOG_H

#include <QColorDialog>
#include "defs.h"

class appColorDialog : public QColorDialog
{
  Q_OBJECT 
  
public:
  explicit appColorDialog (QWidget * parent = 0);

  QColor	appSelectedColor (bool *ok) const;	 // return the selected color

private:
  QColor keepCurrentColor;	// keep the current color here
  bool okflag;

private slots:
  void color_accepted (void) NOEXCEPT;
  void color_rejected (void) NOEXCEPT;
  void dialog_finished (int);

protected:
  virtual void showEvent (QShowEvent * event);
};

#endif // APPCOLORDIALOG_H
