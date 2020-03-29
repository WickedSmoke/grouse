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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include <QSessionManager>
#include <QMainWindow>
#include "common.h"
#include "optionsdialog.h"
#include "infodialog.h"
#include "templatemanagerdialog.h"
#include "portfoliomanagerdialog.h"
#include "qtachart_object.h"
#include "stockticker.h"


typedef class _ChartTab ChartTab;

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow (void);

  QTabWidget *tabWidget;  // tabwidget;
  DataManagerDialog *datamanagerdlg;    // data manager
  bool newversion;        // new version flag

  CG_ERR_RESULT addChart (TableDataVector & datavector);    // add a new chart
  // using tablename data
  CG_ERR_RESULT addPortfolio (int pf_id, QString title,
                              QString currency, QString feed);  // add a new portfolio
  // using tablename data
  QStringList getTabKeys (QString type); // get the keys of of open tabs. type: "Chart" or "Portfolio"
  void setExpandChart (bool);
  void setDevMode (bool devmodeflag);   // set developer's mode
  void checkNewVersion ();              // check if there is new version available
  void enableTickerButton ();           // enable ticker button
  void disableTickerButton ();          // disable ticker button
  inline StockTicker *getTicker () const NOEXCEPT
  {
    return ticker;
  }; // returns stock ticker
  bool expandedChart () const NOEXCEPT;

private:
  Ui::MainWindow *ui;
  AppOptions options;                   // application setting
  //   various comboboxes
  PortfolioManagerDialog *portfoliomanagerdlg;  // portfolio manager
  TemplateManagerDialog *templatemanagerdlg; // template manager
  ModuleManagerDialog *modulemanagerdlg; // module manager
  ProgressDialog *progressdlg;          // progress dialog
  DebugDialog *debugdlg;                // debug dialog
  WaitDialog *waitdlg;                  // wait dialog
  OptionsDialog *optionsdlg;            // options dialog
  InfoDialog *infodlg;                  // options dialog
  StockTicker *ticker;          // stock ticker;
  bool expandedChartFlag;       // expanded = true, normal = false;
  bool tickerVisible;           // visible = true, invisible = false;

  void loadFonts ();                    // load application fonts
private slots:
  void managerButton_clicked (void);
  void portfolioButton_clicked (void);
  void modulesButton_clicked (void);
  void developButton_clicked (void);
  void screenshotButton_clicked (void);
  void optionsButton_clicked (void);
  void tickerButton_clicked (void);
  void debugButton_clicked (void);
  void homeButton_clicked (void);
  void infoButton_clicked (void);
  void exitButton_clicked (void);
  void closeTab_clicked (int);
  void currentTab_changed (int);
  // void commitData(QSessionManager& manager);

protected:
  void closeEvent(QCloseEvent *event);
  virtual void resizeEvent (QResizeEvent * event);
  virtual void focusInEvent (QFocusEvent * event);
  virtual void showEvent (QShowEvent * event);
};

#endif // MAINWINDOW_H
