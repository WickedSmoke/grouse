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

// Full implementation of QTACProperties

#include <QResizeEvent>
#include <QButtonGroup>
#include "qtachart_core.h"
#include "qtachart.h"
#include "qtachart_properties.h"
#include "ui_qtacproperties.h"
#include "common.h"

// constructor
QTACProperties::QTACProperties (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACProperties)
{
  QString checkbstylesheet, colorbstylesheet;
  ui->setupUi (this);
  showVolumes = ui->showVolumes;
  showGrid = ui->showGrid;
  lineChart = ui->lineChart;
  candleChart = ui->candleChart;
  barChart = ui->barChart;
  heikinAshi = ui->heikinAshi;
  lineColorButton = ui->lineColorButton;
  barColorButton = ui->barColorButton;
  foreColorButton = ui->foreColorButton;
  backColorButton = ui->backColorButton;
  linearScale = ui->linearScale;
  onlinePrice = ui->onlinePrice;
  frame = ui->frame;

  linecolor = Qt::green;
  barcolor = Qt::green;
  forecolor = Qt::white;
  backcolor = Qt::black;
  lineColorDialog = NULL;
  barColorDialog = NULL;
  foreColorDialog = NULL;
  backColorDialog = NULL;
  lineicon = NULL;
  baricon = NULL;
  foreicon = NULL;
  backicon = NULL;
  linepixmap = NULL;
  barpixmap = NULL;
  forepixmap = NULL;
  backpixmap = NULL;

  checkbstylesheet =
    "background: transparent; border: 1px solid transparent;border-color: darkgray; padding-left: 5px";
  colorbstylesheet =
    "background: transparent; border: 1px solid transparent;border-color: darkgray;";
  showVolumes->setStyleSheet (checkbstylesheet);
  lineChart->setStyleSheet (checkbstylesheet);
  candleChart->setStyleSheet (checkbstylesheet);
  barChart->setStyleSheet (checkbstylesheet);
  heikinAshi->setStyleSheet (checkbstylesheet);
  showGrid->setStyleSheet (checkbstylesheet);
  lineColorButton->setStyleSheet (colorbstylesheet);
  barColorButton->setStyleSheet (colorbstylesheet);
  foreColorButton->setStyleSheet (colorbstylesheet);
  backColorButton->setStyleSheet (colorbstylesheet);
  linearScale->setStyleSheet (checkbstylesheet);
  onlinePrice->setStyleSheet (checkbstylesheet);
  ui->forecolorLabel->setStyleSheet (checkbstylesheet);
  ui->backcolorLabel->setStyleSheet (checkbstylesheet);

  styleGroup = new QButtonGroup (this);
  styleGroup->addButton (candleChart);
  styleGroup->addButton (lineChart);
  styleGroup->addButton (heikinAshi);
  styleGroup->addButton (barChart);
  styleGroup->setExclusive (true);

  frame->move ((width () - frame->width ()) / 2,
               ((height () - frame->height ()) / 2));

  connect (lineColorButton, SIGNAL (clicked ()), this,
           SLOT (lineColorButton_clicked ()));
  connect (barColorButton, SIGNAL (clicked ()), this,
           SLOT (barColorButton_clicked ()));
  connect (foreColorButton, SIGNAL (clicked ()), this,
           SLOT (foreColorButton_clicked ()));
  connect (backColorButton, SIGNAL (clicked ()), this,
           SLOT (backColorButton_clicked ()));

  correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);
}

// destructor
QTACProperties::~QTACProperties ()
{
  delete styleGroup;

  if (lineicon != NULL)
    delete lineicon;

  if (linepixmap != NULL)
    delete linepixmap;

  if (baricon != NULL)
    delete baricon;

  if (barpixmap != NULL)
    delete barpixmap;

  if (forepixmap != NULL)
    delete forepixmap;

  if (backpixmap != NULL)
    delete backpixmap;

  if (lineColorDialog != NULL)
    delete lineColorDialog;

  if (barColorDialog != NULL)
    delete barColorDialog;

  if (foreColorDialog != NULL)
    delete foreColorDialog;

  if (backColorDialog != NULL)
    delete backColorDialog;

  if (foreicon != NULL)
    delete foreicon;

  if (backicon != NULL)
    delete backicon;

  delete ui;
}

// set chart's style
void
QTACProperties::setChartStyle (int style)
{
  if (style == QTACHART_CANDLE)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Checked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_HEIKINASHI)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Checked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_LINE)
  {
    lineChart->setCheckState (Qt::Checked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Unchecked);
  }

  if (style == QTACHART_BAR)
  {
    lineChart->setCheckState (Qt::Unchecked);
    candleChart->setCheckState (Qt::Unchecked);
    heikinAshi->setCheckState (Qt::Unchecked);
    barChart->setCheckState (Qt::Checked);
  }
}

// get chart style
int
QTACProperties::ChartStyle (void) const
{
  if (lineChart->checkState () == Qt::Checked)
    return QTACHART_LINE;

  if (candleChart->checkState () == Qt::Checked)
    return QTACHART_CANDLE;

  if (heikinAshi->checkState () == Qt::Checked)
    return QTACHART_HEIKINASHI;

  if (barChart->checkState () == Qt::Checked)
    return QTACHART_BAR;

  return QTACHART_CANDLE;

}

// set grid on/off
void
QTACProperties::setGrid (bool boolean)
{
  if (boolean)
    showGrid->setCheckState (Qt::Checked);
  else
    showGrid->setCheckState (Qt::Unchecked);
}

// get grid
bool QTACProperties::Grid (void) const
{
  if (showGrid->checkState () == Qt::Checked)
    return true;

  return false;
}

// get line color
QColor QTACProperties::lineColor (void) const
{
  return linecolor;
}

// set line color
void
QTACProperties::setLineColor (QColor color)
{
  linecolor = color;
}

// get bar color
QColor QTACProperties::barColor (void) const
{
  return barcolor;
}

// set bar color
void
QTACProperties::setBarColor (QColor color)
{
  barcolor = color;
}

// get foreground color
QColor QTACProperties::foreColor (void) const
{
  return forecolor;
}

// set foreground color
void
QTACProperties::setForeColor (QColor color)
{
  forecolor = color;
}

// get background color
QColor QTACProperties::backColor (void) const
{
  return backcolor;
}

// set background color
void
QTACProperties::setBackColor (QColor color)
{
  backcolor = color;
}

// set volumes on/off
void
QTACProperties::setVolumes (bool boolean)
{
  if (boolean)
    showVolumes->setCheckState (Qt::Checked);
  else
    showVolumes->setCheckState (Qt::Unchecked);
}

// set linear scale
void
QTACProperties::setLinearScale (bool boolean)
{
  if (boolean)
    linearScale->setCheckState (Qt::Checked);
  else
    linearScale->setCheckState (Qt::Unchecked);
}

// get linear scale
bool
QTACProperties::LinearScale (void) const
{
  if (linearScale->checkState () == Qt::Checked)
    return true;

  return false;
}

// set online price
void
QTACProperties::setOnlinePrice (bool boolean)
{
  if (boolean)
    onlinePrice->setCheckState (Qt::Checked);
  else
    onlinePrice->setCheckState (Qt::Unchecked);
}

// get online price
bool
QTACProperties::OnlinePrice (void) const
{
  if (onlinePrice->checkState () == Qt::Checked)
    return true;

  return false;
}

// set the reference chart
void
QTACProperties::setReferenceChart (void *chart)
{
  referencechart = chart;
}

// get volumes
bool
QTACProperties::Volumes (void) const
{
  if (showVolumes->checkState () == Qt::Checked)
    return true;

  return false;
}

// line color button clicked
void
QTACProperties::lineColorButton_clicked (void)
{
  lineColorDialog->setCurrentColor (linecolor);
  lineColorDialog->move ((width () - lineColorDialog->width ()) / 2,
                         (height () - lineColorDialog->height ()) / 2);
  lineColorDialog->show ();
  lineColorDialog->open ();
  linecolor = lineColorDialog->selectedColor ();
  linepixmap->fill (linecolor);
}

// bar color button clicked
void
QTACProperties::barColorButton_clicked (void)
{
  barColorDialog->setCurrentColor (barcolor);
  barColorDialog->move ((width () - barColorDialog->width ()) / 2,
                        (height () - barColorDialog->height ()) / 2);
  barColorDialog->show ();
  barColorDialog->open ();
  barcolor = barColorDialog->selectedColor ();
  barpixmap->fill (barcolor);
}

// foreground color button clicked
void
QTACProperties::foreColorButton_clicked (void)
{
  foreColorDialog->setCurrentColor (forecolor);
  foreColorDialog->move ((width () - foreColorDialog->width ()) / 2,
                         (height () - foreColorDialog->height ()) / 2);
  foreColorDialog->show ();
  foreColorDialog->open ();
  forecolor = foreColorDialog->selectedColor ();
  forepixmap->fill (forecolor);
}

// background color button clicked
void
QTACProperties::backColorButton_clicked (void)
{
  backColorDialog->setCurrentColor (backcolor);
  backColorDialog->move ((width () - backColorDialog->width ()) / 2,
                         (height () - backColorDialog->height ()) / 2);
  backColorDialog->show ();
  backColorDialog->open ();
  backcolor = backColorDialog->selectedColor ();
  backpixmap->fill (backcolor);
}

// line color dialog done
void
QTACProperties::lineColorDialog_finished ()
{
  linecolor = lineColorDialog->currentColor ();
  linepixmap->fill (linecolor);
  lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
  lineColorButton->setIcon (*lineicon);
}

// bar color dialog done
void
QTACProperties::barColorDialog_finished ()
{
  barcolor = barColorDialog->currentColor ();
  barpixmap->fill (barcolor);
  baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
  barColorButton->setIcon (*baricon);
}

// foreground color dialog done
void
QTACProperties::foreColorDialog_finished ()
{
  forecolor = foreColorDialog->currentColor ();
  forepixmap->fill (forecolor);
  foreicon->addPixmap (*forepixmap, QIcon::Normal, QIcon::On);
  foreColorButton->setIcon (*foreicon);
}

// background color dialog done
void
QTACProperties::backColorDialog_finished ()
{
  backcolor = backColorDialog->currentColor ();
  backpixmap->fill (backcolor);
  backicon->addPixmap (*backpixmap, QIcon::Normal, QIcon::On);
  backColorButton->setIcon (*backicon);
}

// resize
void
QTACProperties::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  frame->move ((newsize.width () - frame->width ()) / 2,
               ((newsize.height () - frame->height ()) / 2));

  if (barColorDialog != NULL)
  {
    barColorDialog->move ((newsize.width () - barColorDialog->width ()) / 2,
                          (newsize.height () - barColorDialog->height ()) / 2);
  }

  if (lineColorDialog != NULL)
  {
    lineColorDialog->move ((newsize.width () - lineColorDialog->width ()) / 2,
                           (newsize.height () - lineColorDialog->height ()) / 2);
  }

  if (foreColorDialog != NULL)
  {
    foreColorDialog->move ((newsize.width () - foreColorDialog->width ()) / 2,
                           (newsize.height () - foreColorDialog->height ()) / 2);
  }

  if (backColorDialog != NULL)
  {
    backColorDialog->move ((newsize.width () - backColorDialog->width ()) / 2,
                           (newsize.height () - backColorDialog->height ()) / 2);
  }
}

// show
void
QTACProperties::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())
    return;

  if (lineColorDialog == NULL)
  {
    lineColorDialog = new appColorDialog;
    lineColorDialog->setModal (true);
    linepixmap = new QPixmap (16, 16);
    lineicon = new QIcon;
    linepixmap->fill (linecolor);
    lineicon->addPixmap (*linepixmap, QIcon::Normal, QIcon::On);
    lineColorButton->setIcon (*lineicon);
    // lineColorDialog->setStyleSheet ("background-color: lightgray; color: black");
    connect (lineColorDialog, SIGNAL (finished (int)), this,
             SLOT (lineColorDialog_finished ()));
  }

  if (barColorDialog == NULL)
  {
    barColorDialog = new appColorDialog;
    barColorDialog->setModal (true);
    barpixmap = new QPixmap (16, 16);
    baricon = new QIcon;
    barpixmap->fill (barcolor);
    baricon->addPixmap (*barpixmap, QIcon::Normal, QIcon::On);
    barColorButton->setIcon (*baricon);
    // barColorDialog->setStyleSheet ("background-color: lightgray; color: black");
    connect (barColorDialog, SIGNAL (finished (int)), this,
             SLOT (barColorDialog_finished ()));
  }

  if (foreColorDialog == NULL)
  {
    foreColorDialog = new appColorDialog;
    foreColorDialog->setModal (true);
    forepixmap = new QPixmap (16, 16);
    foreicon = new QIcon;
    forepixmap->fill (forecolor);
    foreicon->addPixmap (*forepixmap, QIcon::Normal, QIcon::On);
    foreColorButton->setIcon (*foreicon);
    // foreColorDialog->setStyleSheet ("background-color: lightgray; color: black");
    connect (foreColorDialog, SIGNAL (finished (int)), this,
             SLOT (foreColorDialog_finished ()));
  }

  if (backColorDialog == NULL)
  {
    backColorDialog = new appColorDialog;
    backColorDialog->setModal (true);
    backpixmap = new QPixmap (16, 16);
    backicon = new QIcon;
    backpixmap->fill (backcolor);
    backicon->addPixmap (*backpixmap, QIcon::Normal, QIcon::On);
    backColorButton->setIcon (*backicon);
    // backColorDialog->setStyleSheet ("background-color: lightgray; color: black");
    connect (backColorDialog, SIGNAL (finished (int)), this,
             SLOT (backColorDialog_finished ()));
  }

}
