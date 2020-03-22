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
 
#include "optsize.h"
#include <clocale>
#include <QResizeEvent>
#include <QScrollBar>
#include "ui_databrowserdialog.h"
#include "databrowserdialog.h"
#include "common.h"

struct Quote
{
  QString date;
  QString open;
  QString high;
  QString low;
  QString close;
  QString volume;
};

Q_DECLARE_TYPEINFO (Quote, Q_MOVABLE_TYPE);
typedef QVector < Quote > QuoteVector;

static QuoteVector qvector;
static const int NCOLUMNS = 6;

// constructor
DataBrowserDialog::DataBrowserDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::DataBrowserDialog)
{
  const QString
  stylesheet = "background: transparent; background-color: white;",
  buttonstylesheet = "background: transparent; background-color: white; color:black";
  int colwidth = 0;

  cheadersList << " Date "
               << " Open "
               << " High "
               << " Low  "
               << " Close "
               << " Volume ";


  ui->setupUi (this);
  this->setStyleSheet (stylesheet);
  ui->tableWidget->setColumnCount (NCOLUMNS);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tableWidget->sortByColumn (0, Qt::AscendingOrder);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet ("background: transparent; background-color:lightgray;");
  ui->tableWidget->setColumnWidth(0, 100);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  ui->upToolButton->setStyleSheet (buttonstylesheet);
  ui->downToolButton->setStyleSheet (buttonstylesheet);

  for (qint32 counter = 0; counter < NCOLUMNS - 1; counter ++)
    colwidth += ui->tableWidget->columnWidth (counter);

  this->resize (colwidth + 40, height ());

  // connect to signals
  connect (ui->closeButton, SIGNAL (clicked ()), this,
           SLOT (closeButton_clicked ()));
  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
DataBrowserDialog::~DataBrowserDialog ()
{
  qvector.clear ();
  delete ui;
}

/// Events
///
// resize
void
DataBrowserDialog::resizeEvent (QResizeEvent * event)
{
  QSize newsize;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  ui->tableWidget->resize (newsize.width () - 60, newsize.height () - 60);
  ui->upToolButton->move (newsize.width () - 50, ui->upToolButton->y ());
  ui->downToolButton->move (newsize.width () - 50, ui->downToolButton->y ());
  ui->closeButton->move ((width () - 90) / 2, height () - 42);
}

// callback for sqlite3_exec
static int
sqlcb_table_data (void *dummy, int argc, char **argv, char **column)
{
  Quote q;
  double f;

  if (dummy != NULL)
    return 1;

  setlocale(LC_ALL, "C");

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();

    if (colname == QLatin1String ("DATE"))
      q.date = QString (argv[counter]);
    else if (colname == QLatin1String ("OPEN"))
    {
      f = strtod (argv[counter], NULL);
      q.open =   QString::number (f, 'f', fracdig (f));
    }
    else if (colname == QLatin1String ("HIGH"))
    {
      f = strtod (argv[counter], NULL);
      q.high =   QString::number (f, 'f', fracdig (f));
    }
    else if (colname == QLatin1String ("LOW"))
    {
      f = strtod (argv[counter], NULL);
      q.low = QString::number (f, 'f', fracdig (f));
    }
    else if (colname == QLatin1String ("CLOSE"))
    {
      f = strtod (argv[counter], NULL);
      q.close =  QString::number (f, 'f', fracdig (f));
    }
    else if (colname == QLatin1String ("VOLUME"))
      q.volume = QString (argv[counter]);
  }

  qvector += q;
  return 0;
}

// show
void
DataBrowserDialog::showEvent (QShowEvent * event)
{
  QString query;
  int rc;

  if (event->spontaneous ())
    return;

  ui->tableWidget->clearContents ();
  qvector.clear ();
  query = QStringLiteral ("SELECT DATE, OPEN, HIGH, LOW, CLOSE, VOLUME FROM ") %
          tablename % QStringLiteral (" ORDER BY DATE DESC;");

  rc = selectfromdb(query.toUtf8(), sqlcb_table_data, NULL);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    showMessage (errorMessage (CG_ERR_DBACCESS));
    this->hide ();
    return;
  }

  ui->tableWidget->setSortingEnabled (false);
  ui->tableWidget->setRowCount (qvector.size ());
  for (qint32 counter = 0; counter < qvector.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).date);
    ui->tableWidget->setItem(counter,0,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).open);
    ui->tableWidget->setItem(counter,1,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).high);
    ui->tableWidget->setItem(counter,2,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).low);
    ui->tableWidget->setItem(counter,3,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).close);
    ui->tableWidget->setItem(counter,4,item);

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (qvector.at (counter).volume);
    ui->tableWidget->setItem(counter,5,item);
  }

  ui->tableWidget->viewport()->update();
}

// public slots
void
DataBrowserDialog::closeButton_clicked ()
{
  this->hide ();
}

// down
void
DataBrowserDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
DataBrowserDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

