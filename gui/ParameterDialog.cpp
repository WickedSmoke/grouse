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

#include <QBoxLayout>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QStyle>
#include "ParameterDialog.h"
#include "common.h"
#include "appColorDialog.h"
#include "qtachart_functions.h"
#include "top.h"


DPColorButton::DPColorButton (QWidget * parent, int pidx, int bidx)
{
    if( parent )
        setParent(parent);
    paramidx = pidx;
    buttonidx = bidx;
}


// constructor: remove tick disabled
DynParamsDialog::DynParamsDialog (QString title, QWidget * parent) :
  QDialog(parent)
{
    constructorSetup();

    if (title.isEmpty())
        setWindowTitle( (((QPushButton *) parent)->text()) );
    else
        setWindowTitle( title );

    removeLbl->setVisible(false);
    removeCheckBox->setVisible(false);
    modify = false;
}


// constructor for modify: remove tick enabled
DynParamsDialog::DynParamsDialog (ParamVector PVector, QString title) :
  QDialog()
{
    constructorSetup();

    setWindowTitle(title);

    removeLbl->setVisible(true);
    removeCheckBox->setVisible(true);
    modify = true;

    foreach(const DynParam *param, PVector)
    {
        addParam( param->paramName, param->label, param->type, param->value,
                  param->callback_var, param->show );
    }
}


void DynParamsDialog::constructorSetup()
{
    colorDialog = nullptr;
    param_height = 40;
    ncolorbuttons = 0;

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setModal(true);

    QBoxLayout* lo = new QVBoxLayout( this );
    _form = new QFormLayout;
    lo->addLayout( _form );

    QBoxLayout* lo2 = new QHBoxLayout;
    lo->addLayout( lo2 );

        removeLbl = new QLabel( "Remove" );
        lo2->addWidget( removeLbl );

        removeCheckBox = new QCheckBox;
        lo2->addWidget( removeCheckBox );

    buttonBox = new QDialogButtonBox;
    buttonBox->setStandardButtons( QDialogButtonBox::Cancel |
                                   QDialogButtonBox::Ok );
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect( buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(function_accepted()) );
    lo->addWidget( buttonBox );
}


DynParamsDialog::~DynParamsDialog ()
{
    foreach (const QIcon *icon, Icon)
        delete icon;
    foreach (const QPixmap *pixmap, Pixmap)
        delete pixmap;
    foreach (const DynParam *param, Param)
        delete param;
    delete colorDialog;
}


// add a parameter
void DynParamsDialog::addParam( QString paramName, QString label,
                                qint32 type, qreal defvalue )
{
    QWidget* field = nullptr;
    QLineEdit *edit;
    DynParam *param;

    param = addParameter( Param, paramName, type, defvalue );

    if (type == DPT_INT)
    {
        edit = new QLineEdit(QString::number(param->value, 'f', 0), this);
        connect(edit, SIGNAL(textChanged(const QString&)),
                this, SLOT(text_changed(QString)));
        field = edit;
    }
    else if (type == DPT_REAL)
    {
        edit = new QLineEdit(QString::number(param->value, 'f', 4), this);
        connect(edit, SIGNAL(textChanged(const QString&)),
                this, SLOT(text_changed(QString)));
        field = edit;
    }
    else if (type == DPT_COLOR)
    {
        DPColorButton *colorbutton;
        QPixmap *pixmap;
        QIcon *icon;

        colorbutton = new DPColorButton(this, Param.size() - 1, ncolorbuttons);
        pixmap = new QPixmap (24, 24);
        icon = new QIcon;
        pixmap->fill(Qt::blue);
        icon->addPixmap(*pixmap, QIcon::Normal, QIcon::On);
        colorbutton->setIcon(*icon);
        connect(colorbutton, SIGNAL(clicked(bool)),
                this, SLOT(color_clicked()));

        Pixmap += pixmap;
        Icon += icon;
        Button += colorbutton;

        ++ncolorbuttons;

        field = colorbutton;
    }

    if( field )
    {
        field->setObjectName(label);
        _form->addRow( label, field );
    }
}


// add a parameter with callback variable
void DynParamsDialog::addParam (QString paramName, QString label,
                           qint32 type, qreal defvalue, void *cbvar, bool show)
{
  addParam(paramName, label, type, defvalue);
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
QString DynParamsDialog::getTitle () const
{
    return windowTitle();
}


// get parameter data
qreal DynParamsDialog::getParam (QString paramName) const
{
    foreach (const DynParam *param, Param)
    {
        if( param->paramName == paramName )
            return param->value;
    }
    return 0;
}


// get parameter vector
ParamVector DynParamsDialog::getPVector () const
{
    return Param;
}


void DynParamsDialog::setColorDialog (appColorDialog *dialog)
{
    colorDialog = dialog;
    colorDialog->setModal(true);
    connect(colorDialog, SIGNAL (accepted ()),
            this, SLOT (colorDialog_accepted ()));
    connect(colorDialog, SIGNAL (rejected ()),
            this, SLOT (colorDialog_rejected ()));
}


void DynParamsDialog::color_clicked()
{
    DPColorButton *btn = qobject_cast <DPColorButton *> (QObject::sender());
    colorDialog->setCurrentColor(Param[btn->paramidx]->value);
    colorDialog->show();
    colorDialog->open();

    paramidx = btn->paramidx;
    cbidx = btn->buttonidx;
}


void DynParamsDialog::colorDialog_accepted()
{
    QColor color = colorDialog->currentColor();

    Pixmap[cbidx]->fill(color);
    Icon[cbidx]->addPixmap(*Pixmap[cbidx], QIcon::Normal, QIcon::On);
    Param[paramidx]->value = (qreal) color.rgb();
    Button[cbidx]->setIcon(*Icon[cbidx]);
    raise();
}


void DynParamsDialog::colorDialog_rejected()
{
    raise();
}


void DynParamsDialog::function_accepted()
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


void DynParamsDialog::text_changed(QString)
{
    DynParam *param;
    QLineEdit *edit;
    QString objname;
    bool ok;

    edit = (QLineEdit*) QObject::sender();
    objname = edit->objectName();
    foreach (param, Param)
    {
        if (param->paramName == objname)
            param->value = edit->text().toFloat(&ok);
    }
}


void DynParamsDialog::showEvent (QShowEvent * event)
{
  DynParam *param;
  QLineEdit *edit;
  qint32 bidx = 0;

  if (event->spontaneous ())
    return;

  if (removeLbl->isVisible ())
    removeCheckBox->setCheckState (Qt::Unchecked);

  foreach (param, Param)
  {
    if (param->type == DPT_INT)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(int *)param->callback_var;

      edit = findChild<QLineEdit *> (param->label);
      edit->setText( QString::number(param->defvalue, 'f', 0) );
      edit->clearFocus ();
    }
    else if (param->type == DPT_REAL)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(qreal *)param->callback_var;

      edit = findChild<QLineEdit *> (param->label);
      edit->setText( QString::number(param->defvalue, 'f', 4) );
      edit->clearFocus ();
    }
    else if (param->type == DPT_COLOR)
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
}
