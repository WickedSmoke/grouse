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

#ifndef DOWNLOADDATADIALOG_H
#define DOWNLOADDATADIALOG_H

#include <QDialog>
#include "defs.h"
#include "symbollistdialog.h"

namespace Ui
{
  class DownloadDataDialog;
}

class DownloadDataDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DownloadDataDialog (QWidget *parent = 0);
  ~DownloadDataDialog (void);

  void setSelectedSymbol (QString symbol,  // set the symbol selected from
                          QString name,    // symbol list
                          QString market,
                          QString url1,
                          QString url2);

private:
  Ui::DownloadDataDialog *ui;

  bool exists;                         // symbol exists: true, false
  QString datafeed;                    // the datafeed
  QString symlist;                     // the symlist
  QString symlisturl;                  // the symlisturl
  QString URL1, URL2;                  // url1 and url2
  SymbolListDialog *symlistdlg;        // the symlistdlg

  /// Common functions
  void widgetsSetEnabled (bool boolean); // enable/disable widgets during download
  void setDataFeed (QString feed);       // sets datafeed, symlist and symlisturl

  /// Yahoo finance related function
  void checkYahoosymbolExistence (void);     // check if symbol exists
  void downloadYahooControl (void);        // control data download from yahoo

  /// IEX related functions
  void downloadIEXControl (void);         // control data download from IEX
  void checkIEXsymbolExistence (void);    // check if symbol exists

  /// Alpha Vantage functions
  void downloadAlphaVantageControl (void);        // control data download from IEX
  void checkAlphaVantagesymbolExistence (void);   // check if symbol exists

private slots:
  void accept (void);
  void checkButton_clicked (void);
  void datafeed_changed (const QString & text);
};

#endif // DOWNLOADDATADIALOG_H
