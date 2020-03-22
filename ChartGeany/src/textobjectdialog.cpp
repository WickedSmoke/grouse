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
 
#include <QtGlobal>
#include <QApplication>
#include <QDesktopWidget>
#include <QPalette>
#include <QShowEvent>
#include "ui_textobjectdialog.h"
#include "textobjectdialog.h"
#include "common.h"

// constructor
TextObjectDialog::TextObjectDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::TextObjectDialog)
{
  QStringList fontsizes;
  	
  ui->setupUi (this);
  fontsizes
      << QStringLiteral ("7")
      << QStringLiteral ("8")
      << QStringLiteral ("9")
      << QStringLiteral ("10")
      << QStringLiteral ("11")
      << QStringLiteral ("12")
      << QStringLiteral ("13")
      << QStringLiteral ("14")
      << QStringLiteral ("15")
      << QStringLiteral ("16")
      << QStringLiteral ("17")
      << QStringLiteral ("18");

  ui->sizeComboBox->addItems (fontsizes);
  this->setWindowFlags(Qt::CustomizeWindowHint);
  ui->previewLabel->setStyleSheet (QStringLiteral ("background-color : black; color : white;"));

  color = Qt::white;
  pixmap = new QPixmap (16, 16);
  icon = new QIcon;
  pixmap->fill (color);
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
  colorDialog = new appColorDialog;
  colorDialog->setModal (true);

  // connect to signals
  connect(ui->textEdit, SIGNAL(textChanged(const QString&)), this, SLOT(text_changed(QString)));
  connect(ui->sizeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fontsize_changed(int)));
  connect(ui->fontComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fontfamily_changed(int)));
  connect(ui->boldRadioButton, SIGNAL(clicked (bool)), this, SLOT(fontweight_changed(bool)));
  connect(ui->colorButton, SIGNAL(clicked (bool)), this, SLOT(color_clicked(void)));
  connect (colorDialog, SIGNAL (accepted ()), this, SLOT (colorDialog_accepted ()));
  connect (colorDialog, SIGNAL (rejected ()), this, SLOT (colorDialog_rejected ()));
  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(text_accepted()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(text_rejected()));

  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);
}

// destructor
TextObjectDialog::~TextObjectDialog ()
{
  delete colorDialog;
  delete icon;
  delete pixmap;
  delete ui;
}

// get the text's QLabel
QLabel *
TextObjectDialog::getLabel () NOEXCEPT
{
  return ui->previewLabel;
}

// create new
void
TextObjectDialog::create ()
{
  operation = Create;
  ui->removeLbl->setVisible (false);
  ui->removeCheckBox->setVisible (false);

  this->exec ();
}

// create new
bool
TextObjectDialog::modify (QGraphicsTextItem *text)
{
  operation = Modify;
  ui->removeLbl->setVisible (true);
  ui->removeCheckBox->setVisible (true);
  ui->textEdit->setText (text->toPlainText ());
  ui->fontComboBox->setCurrentFont (text->font ());
  ui->sizeComboBox->setCurrentIndex (text->font ().pointSize () - 7);
  if (text->font ().weight () == QFont::Bold)
    ui->boldRadioButton->setChecked (true);
  ui->previewLabel->setStyleSheet
  (QString::fromLatin1
   ("background-color : black; color: %1 ;").arg(text->defaultTextColor().name()));

  colorDialog->setCurrentColor (text->defaultTextColor());
  pixmap->fill (text->defaultTextColor());
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
  textitem = text;
  this->exec ();

  if (ui->removeCheckBox->isChecked ())
  {
    textitem->setVisible (false);
    return false;
  }

  return true;
}

// signals
void
TextObjectDialog::text_changed(QString str)
{
  ui->previewLabel->setText (str);
}

void
TextObjectDialog::fontsize_changed (int idx)
{
  QFont previewfnt;
  previewfnt = ui->previewLabel->font ();
  previewfnt.setPointSize (idx + 7);
  ui->previewLabel->setFont (previewfnt);
}

void
TextObjectDialog::fontfamily_changed (int idx)
{
  QFont previewfnt;
  previewfnt = ui->previewLabel->font ();
  previewfnt.setFamily (ui->fontComboBox->itemText ( idx ));
  ui->previewLabel->setFont (previewfnt);
}

void
TextObjectDialog::fontweight_changed (bool checked)
{
  Q_UNUSED (checked);
  QFont previewfnt;
  previewfnt = ui->previewLabel->font ();
  if (ui->boldRadioButton->isChecked () == true)
    previewfnt.setBold (true);
  else
    previewfnt.setBold (false);
  ui->previewLabel->setFont (previewfnt);
}

void
TextObjectDialog::color_clicked (void)
{
  colorDialog->setCurrentColor (color);
  colorDialog->show ();
#ifndef Q_OS_MAC
  int x1, y1;
  x1 = x() + ((width () - colorDialog->width ()) / 2);
  if (x1 < 0)
    x1 = 0;
  y1 = y () - ((colorDialog->height () - height ()) / 2);
  if (y1 < 0)
    y1 = 0;
  colorDialog->move (x1, y1);
#endif
  colorDialog->open ();
}

void
TextObjectDialog::colorDialog_accepted ()
{
  color = colorDialog->currentColor ();
  pixmap->fill (color);
  icon->addPixmap (*pixmap, QIcon::Normal, QIcon::On);
  ui->colorButton->setIcon (*icon);
  ui->previewLabel->setStyleSheet
  (QString::fromLatin1
   ("background-color : black; color: %1 ;").arg(color.name()));
  raise ();
}

void
TextObjectDialog::colorDialog_rejected () NOEXCEPT
{
  raise ();
}

void
TextObjectDialog::text_accepted ()
{
  if (operation == Create) // create
  {
    if (ui->previewLabel->text ().size () == 0)
      return;

    appSetOverrideCursor (this, QCursor (Qt::PointingHandCursor));
    return;
  }

  // modify
  QFont fnt;
  fnt = ui->previewLabel->font();

  if (ui->boldRadioButton->isChecked () == true)
    fnt.setBold(true);

  textitem->setPlainText (ui->previewLabel->text ());
  textitem->setFont (fnt);
  textitem->setDefaultTextColor
  (ui->previewLabel->palette ().color (QPalette::WindowText));
}

void
TextObjectDialog::text_rejected ()
{
  ui->previewLabel->setText (QStringLiteral (""));
  ui->textEdit->setText (QStringLiteral (""));
  ui->removeCheckBox->setChecked (false);
}

// events
void
TextObjectDialog::showEvent (QShowEvent * event)
{
  QFont previewfnt;
  bool ok;
  int max, counter;

  if (event->spontaneous ())
    return;

  ui->removeCheckBox->setChecked (false);
  this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        this->size(), qApp->desktop()->availableGeometry()));
  previewfnt = ui->previewLabel->font ();

  max = ui->fontComboBox->count ();
  for (counter = 0; counter < max; counter ++)
    if (ui->fontComboBox->itemText ( counter ) == previewfnt.family ())
      ui->fontComboBox->setCurrentIndex (counter);

  max = ui->sizeComboBox->count ();
  for (counter = 0; counter < max; counter ++)
    if (ui->sizeComboBox->itemText ( counter ).toInt(&ok, 10) == previewfnt.pointSize ())
      ui->sizeComboBox->setCurrentIndex (counter);

  if (previewfnt.bold ())
    ui->boldRadioButton->setChecked(true);

  ui->textEdit->setFocus (Qt::OtherFocusReason);
}
