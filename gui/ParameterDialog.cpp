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
#include <QSpinBox>
#include <QStyle>
#include "ParameterDialog.h"
#include "common.h"
#include "appColorDialog.h"
#include "qtachart_functions.h"
#include "top.h"


DPColorButton::DPColorButton (QWidget * parent, int pidx, int bidx)
    : QPushButton(parent)
{
    paramidx = pidx;
    buttonidx = bidx;
}


// constructor: No remove button
DynParamsDialog::DynParamsDialog (QString title, QWidget * parent) :
  QDialog(parent), modify(false)
{
    constructorSetup();
    setWindowTitle( title );
}


// constructor for modify: Remove button present
DynParamsDialog::DynParamsDialog (const ParamVector& PVector, QString title) :
  QDialog(), modify(true)
{
    constructorSetup();
    setWindowTitle( title );

    ParamVector::const_iterator it;
    FOREACH_PARAM( it, PVector )
    {
        const DynParam *param = *it;
        addParam( param->paramName, param->label, param->type, param->value,
                  param->callback_var, param->show );
    }
}


void DynParamsDialog::constructorSetup()
{
    _removed = false;
    colorDialog = nullptr;
    param_height = 40;
    ncolorbuttons = 0;

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setModal(true);

    QBoxLayout* lo = new QVBoxLayout( this );
    _form = new QFormLayout;
    lo->addLayout( _form );

    lo->addSpacing( 8 );

    buttonBox = new QDialogButtonBox;
    buttonBox->setStandardButtons( QDialogButtonBox::Cancel |
                                   QDialogButtonBox::Ok );
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(function_accepted()) );
    connect( buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    lo->addWidget( buttonBox );

    if( modify )
    {
        QPushButton* btn = buttonBox->addButton("&Remove",
                                        QDialogButtonBox::DestructiveRole);
        connect( btn, SIGNAL(clicked(bool)), SLOT(removeClicked()) );
    }
}


DynParamsDialog::~DynParamsDialog ()
{
    foreach (const QIcon *icon, Icon)
        delete icon;
    foreach (const QPixmap *pixmap, Pixmap)
        delete pixmap;
}


// add a parameter
void DynParamsDialog::addParam( QString paramName, QString label,
                                qint32 type, qreal defvalue )
{
    QWidget* field = nullptr;
    DynParam *param;

    param = Param.addParameter( paramName, type, defvalue );

    if (type == DPT_INT)
    {
        QSpinBox* spin = new QSpinBox(this);
        spin->setRange( 0, 200 );
        connect(spin, SIGNAL(valueChanged(int)),
                this, SLOT(intChanged(int)));
        field = spin;
    }
    else if (type == DPT_REAL)
    {
        QLineEdit *edit;
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
        QString labelC(label);
        _form->addRow( labelC.append(':'), field );
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
qreal DynParamsDialog::getParam(const QString& name) const
{
    const DynParam* par = Param.constParameter( name );
    if( par )
        return par->value;
    return 0;
}


void DynParamsDialog::makeColorDialog()
{
    if( colorDialog )
        return;

    colorDialog = new appColorDialog(this);
    colorDialog->setModal(true);
    connect(colorDialog, SIGNAL(accepted()), this, SLOT(colorAccepted()));
    connect(colorDialog, SIGNAL(rejected()), this, SLOT(colorRejected()));
}


void DynParamsDialog::color_clicked()
{
    makeColorDialog();

    DPColorButton *btn = qobject_cast <DPColorButton *> (QObject::sender());
    colorDialog->setCurrentColor(Param[btn->paramidx]->value);
    colorDialog->show();
    colorDialog->open();

    paramidx = btn->paramidx;
    cbidx = btn->buttonidx;
}


void DynParamsDialog::colorAccepted()
{
    QColor color = colorDialog->currentColor();

    Pixmap[cbidx]->fill(color);
    Icon[cbidx]->addPixmap(*Pixmap[cbidx], QIcon::Normal, QIcon::On);
    Param[paramidx]->value = (qreal) color.rgb();
    Button[cbidx]->setIcon(*Icon[cbidx]);
    raise();
}


void DynParamsDialog::colorRejected()
{
    raise();
}


void DynParamsDialog::function_accepted()
{
  ParamVector::iterator it;
  FOREACH_PARAM( it, Param )
  {
    DynParam* param = *it;
    if (param->paramName == QLatin1String ("Period") && param->value < 2)
      param->value = 2;

    param->defvalue = param->value;
  }

  ParamVector::const_iterator ci;
  FOREACH_PARAM( ci, Param )
  {
    const DynParam* param = *ci;
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


void DynParamsDialog::removeClicked()
{
    _removed = true;
    accept();
}


void DynParamsDialog::intChanged(int n)
{
    DynParam* par = Param.parameter( QObject::sender()->objectName() );
    if( par )
        par->value = n;
}


void DynParamsDialog::text_changed(QString)
{
    QLineEdit *edit = (QLineEdit*) QObject::sender();
    DynParam* par = Param.parameter( edit->objectName() );
    if( par )
        par->value = edit->text().toFloat();
}


void DynParamsDialog::showEvent (QShowEvent * event)
{
  DynParam *param;
  QLineEdit *edit;
  QSpinBox *spin;
  qint32 bidx = 0;

  if (event->spontaneous ())
    return;

  _removed = false;

  ParamVector::iterator it;
  FOREACH_PARAM( it, Param )
  {
    param = *it;
    if (param->type == DPT_INT)
    {
      if (param->callback_var != NULL)
        param->value = param->defvalue = *(int *)param->callback_var;

      spin = findChild<QSpinBox *> (param->label);
      spin->setValue( (int) param->defvalue );
      spin->clearFocus ();
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
