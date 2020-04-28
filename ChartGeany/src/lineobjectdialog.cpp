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
#include <QFormLayout>
#include <QLabel>
#include <QPen>
#include <QPushButton>
#include "lineobjectdialog.h"
#include "appColorDialog.h"


LineObjectDialog::LineObjectDialog (QWidget * parent):
  QDialog (parent), removed(false)
{
#ifdef GUI_DESKTOP
  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#else
  setWindowFlags(Qt::CustomizeWindowHint);
#endif
  setModal(true);

  QBoxLayout* lo = new QVBoxLayout( this );
  QFormLayout* form = new QFormLayout;
  lo->addLayout( form );

      color = Qt::white;
      pixmap = new QPixmap (24, 24);
      icon = new QIcon;
      pixmap->fill (color);
      icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
      colorButton = new QPushButton;
      colorButton->setIcon (*icon);
      connect(colorButton, SIGNAL(clicked(bool)), SLOT(color_clicked()));
      form->addRow( QStringLiteral("Color:"), colorButton );

  lo->addSpacing( 8 );

  QDialogButtonBox* bbox = new QDialogButtonBox;
  bbox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  QPushButton* remove =
              bbox->addButton("&Remove", QDialogButtonBox::DestructiveRole);
  connect(remove, SIGNAL(clicked(bool)), SLOT(removeClicked()));
  connect(bbox, SIGNAL(accepted()), SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), SLOT(reject()));
  lo->addWidget( bbox );

  colorDialog = new appColorDialog(this);
  colorDialog->setModal (true);
  connect(colorDialog, SIGNAL(accepted()), SLOT(colorAccepted()));
  connect(colorDialog, SIGNAL(rejected()), SLOT(colorRejected()));

#ifndef GUI_DESKTOP
  colorButton->setFixedSize(30, 30);

  QAbstractButton* btn;
  foreach (btn, bbox->buttons ())
    btn->setFocusPolicy (Qt::NoFocus);

  correctWidgetFonts (this);
#endif
}


LineObjectDialog::~LineObjectDialog ()
{
  delete colorDialog;
  delete icon;
  delete pixmap;
}


// modify or remove existing. returns true on modify, false on delete
bool LineObjectDialog::modify (QTACObject *obj)
{
  colorDialog->setCurrentColor (obj->hvline->pen().color ());
  pixmap->fill (obj->hvline->pen().color ());
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  colorButton->setIcon (*icon);

  if( obj->type == QTACHART_OBJ_FIBO )
      setWindowTitle("Fibonacci");
  else
      setWindowTitle("Line");

  removed = false;
  if( exec() == QDialog::Rejected )
    return true;

  if (removed)
    return false;

  switch( obj->type )
  {
    case QTACHART_OBJ_HLINE:
    case QTACHART_OBJ_VLINE:
      obj->forecolor = color;
      obj->hvline->setPen (QPen (color));
      obj->title->setDefaultTextColor (color);
      break;

    case QTACHART_OBJ_FIBO:
      for (qint32 i = 0; i < obj->FiboLevelPrc.size (); i ++)
      {
        ((QGraphicsLineItem *)obj->FiboLevel[i])->setPen (QPen (color));
        obj->FiboLevelLbl[i].setDefaultTextColor (color);
        obj->FiboLevelPrcLbl[i].setDefaultTextColor (color);
      }
      // Fall through...

    case QTACHART_OBJ_LINE:
      obj->forecolor = color;
      obj->hvline->setPen (QPen (color));
      obj->Edge[0]->pricetxt->setDefaultTextColor (color);
      obj->Edge[1]->pricetxt->setDefaultTextColor (color);
      break;
  }
  return true;
}


void LineObjectDialog::color_clicked (void)
{
  colorDialog->setCurrentColor (color);
  colorDialog->show ();
  colorDialog->open ();
  color = colorDialog->selectedColor ();
  pixmap->fill (color);
}


void LineObjectDialog::colorAccepted ()
{
  color = colorDialog->currentColor ();
  pixmap->fill (color);
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  colorButton->setIcon (*icon);
  raise ();
}


void LineObjectDialog::colorRejected ()
{
  raise ();
}


void LineObjectDialog::removeClicked()
{
    removed = true;
    accept();
}
