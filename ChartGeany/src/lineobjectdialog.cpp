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
#include <QFontComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPen>
#include <QPushButton>
#include "lineobjectdialog.h"
#include "appColorDialog.h"


LineObjectDialog::LineObjectDialog (QWidget * parent):
    QDialog (parent), pixmap(nullptr), icon(nullptr), colorButton(nullptr),
    removed(false)
{
#ifdef GUI_DESKTOP
  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
#else
  setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
#endif
  setModal(true);

  QBoxLayout* lo = new QVBoxLayout( this );
  form = new QFormLayout;
  lo->addLayout( form );

  lo->addSpacing( 8 );

  bbox = new QDialogButtonBox;
  bbox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  removeButton = bbox->addButton("&Remove", QDialogButtonBox::DestructiveRole);
  connect(removeButton, SIGNAL(clicked(bool)), SLOT(removeClicked()));
  connect(bbox, SIGNAL(accepted()), SLOT(accept()));
  connect(bbox, SIGNAL(rejected()), SLOT(reject()));
  lo->addWidget( bbox );

  colorDialog = new appColorDialog(this);
  colorDialog->setModal (true);
  connect(colorDialog, SIGNAL(accepted()), SLOT(colorAccepted()));
  connect(colorDialog, SIGNAL(rejected()), SLOT(colorRejected()));

#ifndef GUI_DESKTOP
  QAbstractButton* btn;
  foreach (btn, bbox->buttons ())
    btn->setFocusPolicy (Qt::NoFocus);

  correctWidgetFonts (this);
#endif
}


void LineObjectDialog::createColorButton()
{
    pixmap = new QPixmap (24, 24);
    icon = new QIcon;
    colorButton = new QPushButton;
    colorButton->setIcon (*icon);
    connect(colorButton, SIGNAL(clicked(bool)), SLOT(color_clicked()));
    form->addRow( QStringLiteral("Color:"), colorButton );

#ifndef GUI_DESKTOP
    colorButton->setFixedSize(30, 30);
#endif

    setColor( Qt::white );
}


LineObjectDialog::~LineObjectDialog ()
{
  delete colorDialog;
  delete icon;
  delete pixmap;
}


void LineObjectDialog::enableRemove( bool on )
{
    removeButton->setVisible( on );
}


void LineObjectDialog::setColor( const QColor& col )
{
    color = col;
    pixmap->fill( col );
    icon->addPixmap( *pixmap, QIcon::Normal, QIcon::On );
    colorButton->setIcon( *icon );
}


// modify or remove existing. returns true on modify, false on delete
bool LineObjectDialog::modify (QTACObject *obj)
{
  if( ! colorButton )
      createColorButton();
  setColor( obj->hvline->pen().color() );

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
}


void LineObjectDialog::colorAccepted ()
{
  setColor( colorDialog->currentColor() );
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


//----------------------------------------------------------------------------


#define MIN_SIZE    7

TextObjectDialog::TextObjectDialog(QWidget * parent) :
    LineObjectDialog(parent)
{
    setWindowTitle("Text");

    QWidget* fontProp = new QWidget;
    QBoxLayout* hl = new QHBoxLayout(fontProp);
    hl->setContentsMargins( 0, 0, 0, 0 );

        familyCombo = new QFontComboBox;
        hl->addWidget( familyCombo );

        sizeCombo = new QComboBox;
        QStringList fontsizes;
        for( int i = MIN_SIZE; i < 19; ++i )
          fontsizes << QString::number( i );
        sizeCombo->addItems(fontsizes);
        hl->addWidget( sizeCombo );

        boldCheck = new QCheckBox("Bold");
        hl->addWidget( boldCheck );

        connect(sizeCombo, SIGNAL(currentIndexChanged(int)),
              SLOT(sizeChanged(int)));
        connect(familyCombo, SIGNAL(currentIndexChanged(int)),
              SLOT(familyChanged(int)));
        connect(boldCheck, SIGNAL(clicked (bool)),
              SLOT(weightChanged(bool)));

    form->addRow("Font:", fontProp);

    textEdit = new QLineEdit;
    textEdit->setPlaceholderText( "Your Text" );
    connect(textEdit, SIGNAL(textChanged(const QString&)),
            SLOT(textChanged(const QString&)));
    form->addRow("Text:", textEdit);

    createColorButton();

    preview = new QLabel;
    preview->setMinimumHeight(30);
    updatePreviewColor();
    QBoxLayout* lo = static_cast<QBoxLayout*>( layout() );
    lo->insertWidget( 1, preview );

    int ps = preview->font().pointSize();
    if( ps < MIN_SIZE )
        ps = MIN_SIZE;
    sizeCombo->setCurrentIndex( ps - MIN_SIZE );

    bbox->button(QDialogButtonBox::Ok)->setEnabled( false );
}


QLabel* TextObjectDialog::getLabel() NOEXCEPT
{
    return preview;
}


// Return true if modified or canceled, false on delete.
bool TextObjectDialog::modify(QGraphicsTextItem *text)
{
    enableRemove( true );

    textEdit->setText( text->toPlainText() );
    familyCombo->setCurrentFont(text->font());
    sizeCombo->setCurrentIndex(text->font().pointSize() - MIN_SIZE);
    if (text->font().weight() == QFont::Bold)
        boldCheck->setChecked(true);
    preview->setFont(text->font());

    setColor( text->defaultTextColor() );
    updatePreviewColor();

    removed = false;
    if( exec() == QDialog::Rejected )
        return true;

    if (removed)
        return false;

    text->setPlainText( textEdit->text() );
    QFont fnt( preview->font() );
    if( boldCheck->isChecked() )
        fnt.setBold(true);
    text->setFont(fnt);
    text->setDefaultTextColor(color);
    return true;
}


void TextObjectDialog::updatePreviewColor()
{
    preview->setStyleSheet(
        QString::fromLatin1("background-color: black; color: %1;").arg(
            color.name()) );
}


void TextObjectDialog::colorAccepted()
{
    LineObjectDialog::colorAccepted();
    updatePreviewColor();
}


void TextObjectDialog::textChanged(const QString& str)
{
    preview->setText(str);
    bbox->button(QDialogButtonBox::Ok)->setEnabled( ! str.isEmpty() );
}


void TextObjectDialog::sizeChanged(int n)
{
    QFont fnt( preview->font() );
    fnt.setPointSize(n + MIN_SIZE);
    preview->setFont(fnt);
}


void TextObjectDialog::familyChanged(int n)
{
    QFont fnt( preview->font() );
    fnt.setFamily(familyCombo->itemText( n ));
    preview->setFont(fnt);
}


void TextObjectDialog::weightChanged(bool)
{
    QFont fnt( preview->font() );
    fnt.setBold( boldCheck->isChecked() );
    preview->setFont(fnt);
}
