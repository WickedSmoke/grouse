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
#ifndef DATAMANAGER_H
#define DATAMANAGER_H


#include <QDialog>
#include "idb.h"


class QToolButton;
class QTableWidget;
class QFrame;
class QLabel;
class QComboBox;
class DataBrowserDialog;
class DownloadDataDialog;

class DataManager : public QDialog
{
    Q_OBJECT

public:
    explicit DataManager (QWidget *parent = 0);
    ~DataManager (void);

    QStringList symbolList;	  // symbols column content
    QStringList descList;     // name column content
    QStringList marketList;   // markets column content
    QStringList sourceList;   // source column content
    QStringList timeframeList;// timeframe column content
    QStringList datefromList;  // from column content
    QStringList datetoList;    // to column content
    QStringList currencyList; // currency column content
    QStringList keyList; // key column content
    QStringList adjustedList; // adjusted column content
    QStringList baseList; // base column content
    QStringList pathList; // download/import path
    QStringList formatList; // format
    QStringList lastupdateList; // last update since epoch in secs

    QStringList sqlite_master_name; // name column of sqlite_master table
    QStringList sqlite_master_type; // type columnt of sqlite_master table
    TableDataVector TDVector; // vector of TableDataClass

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent * event);
    virtual void changeEvent(QEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);

private slots:
    void refreshButton_clicked();
    void trashButton_clicked();
    void downloadButton_clicked();
    void updateButton_clicked();
    void browserButton_clicked();
    void chartButton_clicked();
    void filter_combol_changed(const QString &);

private:
    void reloadSymbols();
    void fillcolumn(QStringList list, int col);
    void cleartable();
    QString formSQLDropSentence(QString table, qint32 *nentries);

    QString symFilter;              // symbol filter;
    DataBrowserDialog *browser;     // data browser
    DownloadDataDialog *downloadDialog;
    bool updateBeforeOpen;          // update data before open the chart

    QToolButton *importButton;
    QToolButton *downloadButton;
    QToolButton *trashButton;
    QToolButton *refreshButton;
    QToolButton *updateButton;
    QToolButton *chartButton;
    QToolButton *browserButton;

    QTableWidget *tableWidget;
    QFrame *filterFrame;
    QLabel *filterLabel;
    QComboBox *symbolFilterCombo;
};


#endif // DATAMANAGER_H
