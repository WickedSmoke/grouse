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

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QStyle>
#include "common.h"
#include "appColorDialog.h"
#include "ui_dynparamsdialog.h"
#include "dynparamsdialog.h"
#include "qtachart_functions.h"
#include "top.h"


// constructor
DPColorButton::DPColorButton (QWidget * parent, int pidx, int bidx)
{
  if (parent != NULL)
    setParent (parent);
  paramidx = pidx;
  buttonidx = bidx;

  // correctWidgetFonts (this);
}

// destructor
DPColorButton::~DPColorButton ()
{

}

// constructor: remove tick disabled
DynParamsDialog::DynParamsDialog (QString title, QWidget * parent):
  QDialog (parent), ui (new Ui::DynParamsDialog)
{
  DynParamsDialog_constructor_body ();

  if (title == QLatin1String (""))
    ui->title->setText ((((QPushButton *) parent)->text ()));
  else
    ui->title->setText (title);

  ui->removeLbl->setVisible (false);
  ui->removeCheckBox->setVisible (false);

  modify = false;
  setParent (parent);

  correctWidgetFonts (this);
}

// constructor for modify: remove tick enabled
DynParamsDialog::DynParamsDialog (ParamVector PVector, QString title):
  QDialog (), ui (new Ui::DynParamsDialog)
{
  DynParamsDialog_constructor_body ();
  ui->title->setText (title);
  ui->removeLbl->setVisible (true);
  ui->removeCheckBox->setVisible (true);
  modify = true;

  foreach (const DynParam *param, PVector)
    addParam (param->paramName, param->label, param->type, param->value,
              param->callback_var, param->show);

//  foreach (const DynParam *param, PVector)
//    delete param;

  // this->setWindowFlags(Qt::CustomizeWindowHint);
  correctWidgetFonts (this);
}

void
DynParamsDialog::DynParamsDialog_constructor_body ()
{
  const QString
  stylesheet = QStringLiteral ("background-color: lightgray"),
  boxstylesheet = QStringLiteral ("background-color: lightgray; color: black; border: 1px solid transparent;border-color: black;"),
  lblstylesheet = QStringLiteral ("background-color: lightgray; color: black; border: 1px solid transparent;border-color: lightgray;"),
  chkbxstylesheet = QStringLiteral ("border: 1px solid transparent; border-color: lightgray; color: black;");

  setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
  setModal(true);
  ui->setupUi (this);
  ui->buttonBox->setStyleSheet (boxstylesheet);
  ui->buttonBox->setFixedSize (QSize (380, 32));
  buttonBox = ui->buttonBox;
  removeCheckBox = ui->removeCheckBox;

  foreach (QAbstractButton *button, ui->buttonBox->buttons ())
  {
    button->setFixedSize (QSize (button->width (), button->height ()));
    button->setFocusPolicy (Qt::NoFocus);
  }

  ui->title->setStyleSheet (boxstylesheet);
  ui->removeLbl->setStyleSheet (lblstylesheet);
  ui->removeCheckBox->setStyleSheet (chkbxstylesheet);

  setStyleSheet (stylesheet);
  param_height = 40;
  ncolorbuttons = 0;
  colorDialog = NULL;
  connect (buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect (buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));
}

// destructor
DynParamsDialog::~DynParamsDialog ()
{
  foreach (const QIcon *icon, Icon)
    delete icon;

  foreach (const QPixmap *pixmap, Pixmap)
    delete pixmap;

  foreach (const DynParam *param, Param)
    delete param;

  if (colorDialog != NULL)
    delete colorDialog;

  delete ui;
}

// add a parameter
void
DynParamsDialog::addParam (QString paramName, QString label,
                           qint32 type, qreal defvalue)
{
  const QString
  stylesheet = QStringLiteral ("background-color: lightgray"),
  lblstylesheet = QStringLiteral ("background-color: lightgray; color: black; border: 1px solid transparent;border-color: lightgray;"),
  editstylesheet = QStringLiteral ("background-color: white; color: black; border: 1px solid transparent; border-color: black;");
  QLabel *lbl;
  QLineEdit *edit;
  DynParam *param;
  QFont font;
  qint32 h;

  param = addParameter( Param, paramName, type, defvalue );

  lbl = new QLabel (label, this, Qt::Widget);
  font = lbl->font ();
  font.setWeight (QFont::Bold);
  lbl->setFont (font);
  lbl->setGeometry (10, Param.size () * param_height, 190, 32);
  lbl->setStyleSheet (lblstylesheet);

  if (type == DPT_INT)
  {
    edit = new QLineEdit (QString::number (param->value, 'f', 0), this);
    font = edit->font ();
    font.setWeight (QFont::Bold);
    edit->setFont (font);
    edit->setGeometry (210, Param.size () * param_height, 50, 32);
    edit->setStyleSheet (editstylesheet);
    edit->setInputMask ("####");
    edit->setObjectName (label);
    edit->setFocusPolicy (Qt::StrongFocus);
    connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(text_changed(QString)));
  }

  if (type == DPT_REAL)
  {
    edit = new QLineEdit (QString::number (param->value, 'f', 4), this);
    font = edit->font ();
    font.setWeight (QFont::Bold);
    edit->setFont (font);
    edit->setGeometry (210, Param.size () * param_height, 120, 32);
    edit->setStyleSheet (editstylesheet);
    edit->setInputMask ("###9.9999");
    edit->setObjectName (label);
    edit->setFocusPolicy (Qt::StrongFocus);
    connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(text_changed(QString)));
  }

  if (type == DPT_COLOR)
  {
    QPixmap *pixmap;
    DPColorButton *colorbutton;
    QIcon *icon;
    colorbutton = new DPColorButton (this, Param.size () - 1, ncolorbuttons);
    colorbutton->setFocusPolicy (Qt::StrongFocus);
    pixmap = new QPixmap (24, 24);
    icon = new QIcon;
    pixmap->fill (Qt::blue);
    icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
    colorbutton->setIcon (*icon);
    colorbutton->setGeometry (210, Param.size () * param_height, 32, 32);
    colorbutton->setStyleSheet (editstylesheet);
    colorbutton->setObjectName (label);
    Pixmap += pixmap;
    Icon += icon;
    Button += colorbutton;
    ncolorbuttons ++;
    connect (colorbutton, SIGNAL(clicked (bool)), this, SLOT(color_clicked(void)));
  }

  h =  (Param.size () + 2) * param_height;
  resize (400, h);
  setMaximumSize (width (), h);
  setMinimumSize (width (), h);
}

// add a parameter with callback variable
void
DynParamsDialog::addParam (QString paramName, QString label,
                           qint32 type, qreal defvalue, void *cbvar, bool show)
{
  addParam (paramName, label, type, defvalue);
  DynParam *param = Param.last();

  param->callback_var = cbvar;
  param->show = show;
  if (cbvar != NULL)
  {
    if (param->type == DPT_INT)
      param->value = param->defvalue = *(int *)param->callback_var;
    else if (param->type == DPT_REAL)
      param->value = param->defvalue = *(qreal *)param->callback_var;
    else if (param->type == DPT_COLOR)
      param->value = param->defvalue = *(Color_t *)param->callback_var;
  }
}

// get title label
QString
DynParamsDialog::getTitle () const
{
  return ui->title->text ();
}

// get parameter data
qreal
DynParamsDialog::getParam (QString paramName) const
{
  foreach (const DynParam *param, Param)
  {
    if (param->paramName == paramName)
      return param->value;
  }
  return 0;
}

// get parameter vector
ParamVector
DynParamsDialog::getPVector () const
{
  return Param;
}

void
DynParamsDialog::setColorDialog (appColorDialog *dialog)
{
  colorDialog = dialog;
  colorDialog->setModal (true);
  connect (colorDialog, SIGNAL (accepted ()), this, SLOT (colorDialog_accepted ()));
  connect (colorDialog, SIGNAL (rejected ()), this, SLOT (colorDialog_rejected ()));
}

// slots
void
DynParamsDialog::color_clicked (void)
{
  DPColorButton *btn;

  btn = qobject_cast <DPColorButton *> (QObject::sender());
  colorDialog->setCurrentColor (Param[btn->paramidx]->value);

  colorDialog->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                           colorDialog->size(), qApp->desktop()->availableGeometry()));
  colorDialog->show ();
  colorDialog->open ();
  paramidx = btn->paramidx;
  cbidx = btn->buttonidx;
}

void
DynParamsDialog::colorDialog_accepted ()
{
  QColor color;

  color = colorDialog->currentColor ();
  Pixmap[cbidx]->fill (color);
  Icon[cbidx]->addPixmap (*Pixmap[cbidx], QIcon::Normal, QIcon::On);
  Param[paramidx]->value = (qreal) color.rgb ();
  Button[cbidx]->setIcon (*Icon[cbidx]);
  raise ();
}

void
DynParamsDialog::colorDialog_rejected ()
{
  raise ();
}

// function accepted
void
DynParamsDialog::function_accepted (void)
{
  foreach (DynParam *param, Param)
  {
    if (param->paramName == QLatin1String ("Period") && param->value < 2)
      param->value = 2;

    param->defvalue = param->value;
  }

  foreach (const DynParam *param, Param)
  {
    if (param->callback_var != NULL)
    {
      if (param->type == DPT_INT)
        *(int *)param->callback_var = (int) param->value;
      else if (param->type == DPT_REAL)
        *(qreal *)param->callback_var = (qreal) param->value;
      else
      {
        if (param->type == DPT_COLOR)
          *(Color_t *)param->callback_var = (Color_t) param->value;
      }
    }
  }
}

// function rejected
void
DynParamsDialog::function_rejected (void)
{

}

void
DynParamsDialog::text_changed(QString)
{
  DynParam *param;
  QLineEdit *edit;
  QString objname;
  bool ok;

  edit = (QLineEdit *) QObject::sender();
  objname = edit->objectName ();
  foreach (param, Param)
    if (param->paramName == objname)
      param->value = edit->text ().toFloat (&ok);
}

// show event
void
DynParamsDialog::showEvent (QShowEvent * event)
{
  DynParam *param;
  QLineEdit *edit;
  qint32 bidx = 0;
  qint32 h;

  if (event->spontaneous ())
    return;

  h =  (Param.size () + 2) * param_height;
  if (ui->removeLbl->isVisible ())
  {
    h += param_height;
    ui->removeLbl->move (10, (Param.size () + 1) * param_height);
    ui->removeCheckBox->move (210, (Param.size () + 1) * param_height + 5);
    ui->buttonBox->move (10, (Param.size () + 2) * param_height);
    ui->removeCheckBox->setCheckState (Qt::Unchecked);
  }
  else
    ui->buttonBox->move (10, (Param.size () + 1) * param_height);

  resize (400, h);
  setMaximumSize (width (), h);
  setMinimumSize (width (), h);

  foreach (param, Param)
  {
    if (param->type == DPT_INT)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(int *)param->callback_var;

      edit = findChild<QLineEdit *> (param->label);
      edit->setText (QString::number (param->defvalue, 'f', 0));
      edit->clearFocus ();
    }

    if (param->type == DPT_REAL)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(qreal *)param->callback_var;

      edit = findChild<QLineEdit *> (param->label);
      edit->setText (QString::number (param->defvalue, 'f', 4));
      edit->clearFocus ();
    }

    if (param->type == DPT_COLOR)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(Color_t *)param->callback_var;

      Pixmap[bidx]->fill (QColor ((QRgb) param->defvalue));
      Icon[bidx]->addPixmap (*Pixmap[bidx], QIcon::Normal, QIcon::On);
      Button[bidx]->setIcon (*Icon[bidx]);
      Button[bidx]->clearFocus ();
      bidx ++;
    }
  }

#ifdef Q_OS_MAC
  if (parent () != NULL)
  {
    QWidget *p = (qobject_cast <QWidget*> (parent ()->parent ()));
    move ((int) ((p->width () - width ()) / 2), 40);
  }
  else
#endif
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                    this->size(), qApp->desktop()->availableGeometry(this)));
}

// resize event
void
DynParamsDialog::resizeEvent (QResizeEvent * event)
{
  Q_UNUSED (event);
}
