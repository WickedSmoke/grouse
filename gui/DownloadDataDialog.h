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


class QComboBox;
class QCheckBox;
class QDialogButtonBox;
class QLineEdit;
class QProgressBar;
class QPushButton;

class DownloadDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDataDialog(QWidget *parent = 0);

    void setSelectedSymbol(QString symbol, QString name, QString market,
                           QString url1, QString url2);

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void download();
    void checkButton_clicked();
    void datafeed_changed(const QString & text);

private:
    void widgetsSetEnabled(bool boolean); // enable/disable widgets during download
    void setDataFeed(const QString& feed);// sets datafeed, symlist and symlisturl

    /// Yahoo finance related function
    void checkYahoosymbolExistence();   // check if symbol exists
    void downloadYahooControl();        // control data download from yahoo

    /// IEX related functions
    void downloadIEXControl();          // control data download from IEX
    void checkIEXsymbolExistence();     // check if symbol exists

    /// Alpha Vantage functions
    void downloadAlphaVantageControl();      // control data download from IEX
    void checkAlphaVantagesymbolExistence(); // check if symbol exists

    QString datafeed;                    // the datafeed
    QString symlist;                     // the symlist
    QString symlisturl;                  // the symlisturl
    QString URL1, URL2;                  // url1 and url2
    SymbolListDialog *symlistdlg;        // the symlistdlg
    bool symExists;                         // symbol exists: true, false

    QDialogButtonBox *buttonBox;
    QLineEdit *symbolEdit;
    QLineEdit *nameEdit;
    QPushButton *checkButton;
    QProgressBar *progressBar;
    QComboBox *datafeedsCombo;
    QLineEdit *marketEdit;
    QComboBox *currencyCombo;
    QCheckBox *adjustCheckBox;
};

#endif // DOWNLOADDATADIALOG_H
