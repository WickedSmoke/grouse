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

#ifndef DYNPARAMSDIALOG_H
#define DYNPARAMSDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QIcon>
#include "appColorDialog.h"
#include "DynParam.h"

class DPColorButton : public QPushButton
{
  Q_OBJECT

public:
  explicit DPColorButton (QWidget *parent, int paramidx, int buttonidx);
  ~DPColorButton (void);

  int paramidx;
  int buttonidx;
};

typedef QVector <QPixmap *> PixmapVector;
typedef QVector <QIcon *> IconVector;
typedef QVector <DPColorButton *> ButtonVector;

namespace Ui
{
  class DynParamsDialog;
}

class DynParamsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DynParamsDialog (QString title, QWidget *parent = 0); // new indicator constructor
  DynParamsDialog (const ParamVector& PVector, QString title); // modify constructor
  ~DynParamsDialog(void);

  QDialogButtonBox *buttonBox;
  QCheckBox *removeCheckBox;

  // add a parameter
  void addParam (QString paramName, QString title, qint32 type, qreal defvalue);

  // add a parameter with callback variable
  void addParam (QString paramName, QString label, qint32 type, qreal defvalue,
                 void *cbvar, bool show);

  // get the title label
  QString getTitle (void) const;

  // get parameter data
  qreal getParam (const QString& paramName) const;

  // get parameter vector
  const ParamVector& parameters() const { return Param; };
  ParamVector& parameters() { return Param; };

private:
  void DynParamsDialog_constructor_body (void); // constructor body
  void makeColorDialog();

  Ui::DynParamsDialog * ui;
  appColorDialog *colorDialog; // color dialog

  ParamVector Param;    // parameters' vector
  PixmapVector Pixmap;  // pixmaps' vector
  IconVector Icon;      // icons' vector
  ButtonVector Button;  // buttons' vector
  int param_height;     // height of each parameter
  int Height;           // dialog's height
  int ncolorbuttons;    // number of color buttons
  int paramidx;         // parameter index
  int cbidx;            // color button index
  bool modify;          // create = false, modify = true

private slots:
  void color_clicked (void);
  void colorDialog_accepted (void);
  void colorDialog_rejected (void);
  void text_changed(QString);
  void function_accepted (void);
  void function_rejected (void);

protected:
  virtual void showEvent (QShowEvent * event);
  virtual void resizeEvent (QResizeEvent * event);
};

#endif // DYNPARAMSDIALOG_H
