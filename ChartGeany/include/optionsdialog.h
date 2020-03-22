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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QCheckBox>
#include "appColorDialog.h"

namespace Ui
{
  class OptionsDialog;
}

class OptionsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OptionsDialog(QWidget *parent = 0);
  ~OptionsDialog (void);

private:
  Ui::OptionsDialog *ui;

  QColor linecolor;
  QColor barcolor;
  QColor forecolor;
  QColor backcolor;
  QButtonGroup *styleGroup;
  QCheckBox *showVolumes;
  QCheckBox *showGrid;
  QCheckBox *lineChart;
  QCheckBox *candleChart;
  QCheckBox *barChart;
  QCheckBox *heikinAshi;
  QCheckBox *linearScale;
  QCheckBox *onlinePrice;
  QPushButton *lineColorButton;
  QPushButton *barColorButton;
  QPushButton *foreColorButton;
  QPushButton *backColorButton;
  QPixmap *linepixmap, *barpixmap, *forepixmap, *backpixmap;
  QIcon *lineicon, *baricon, *foreicon, *backicon;
  appColorDialog *lineColorDialog;
  appColorDialog *barColorDialog;
  appColorDialog *foreColorDialog;
  appColorDialog *backColorDialog;
  ToolchainVector toolchains;	// array of supported toolchains
  
  void loadPlatforms (void);    // load supported os/compiler combination for cgscript
  void loadOptions (void);		// load application options
  void saveOptions (void);		// save application options

private slots:
  void ok_clicked (void);
  void cancel_clicked (void);
  void newVersionBtn_clicked (void);
  void addBtn_clicked (void);
  void minusBtn_clicked (void);
  void symboladd_accepted (void);
  void symboladd_rejected (void);
  void yahoofeed_clicked (int state);
  void googlefeed_clicked (int state);
  void downButton_clicked (void);
  void upButton_clicked (void);

  void lineColorButton_clicked (void);
  void barColorButton_clicked (void);
  void lineColorDialog_finished (void);
  void barColorDialog_finished (void);
  void foreColorButton_clicked (void);
  void backColorButton_clicked (void);
  void foreColorDialog_finished (void);
  void backColorDialog_finished (void);
  void applyPlatformBtn_clicked (void);
  
  void setChartStyle (int style);
  int ChartStyle (void);	// return's chart style

  void setGrid (bool);		// set grid on/off
  bool Grid (void);		// returns grid state

  void setLinearScale (bool);		// set price scale to linear on/off
  bool LinearScale (void);	// return linear price scale state

  void setOnlinePrice (bool);		// set online price on/off
  bool OnlinePrice (void);	// return online price state

  QColor lineColor (void); // return the line color
  void setLineColor (QColor); // set line color

  QColor barColor (void); // return the bar color
  void setBarColor (QColor); // set bar color

  QColor foreColor (void); // return the foreground color
  void setForeColor (QColor); // set foreground color

  QColor backColor (void); // return the background color
  void setBackColor (QColor); // set background color

  void setVolumes (bool);	// set volumes on off
  bool Volumes (void);		// returns volumes state

protected:
  virtual void closeEvent (QCloseEvent * event);
  virtual void showEvent (QShowEvent * event);
  virtual void keyPressEvent (QKeyEvent * event);
};

#endif // OPTIONSDIALOG_H
