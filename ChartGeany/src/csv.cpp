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

#include <QDate>
#include <QTextStream>
#include <QTemporaryFile>
#include "common.h"
#ifdef Q_OS_LINUX
#include <xls.h>
#else
#include "libxls/xls.h"
#endif
#include <cstdio>
#include <cstdlib>

#define underscore QStringLiteral ("_")
#define comma      QStringLiteral (",")
#define dash       QStringLiteral ("-")
#define dot        QStringLiteral (".")

// create csv file from raw YAHOO finance csv file
static CG_ERR_RESULT
yahoo2csv_old (const QString & namein, const QString & nameout)
{
  QFile rawcsv, csv;
  QString inputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  inputline.reserve (512);
  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    inputline = in.readLine (0);    // header
    if (!in.atEnd ())
      inputline = in.readLine (0);  // first line
    while (!in.atEnd ())
    {
      inputline += QStringLiteral (",00:00.00");
      out << inputline << QStringLiteral ("\n");
      inputline = in.readLine (0);
    }
    if (inputline.size() > 0)
      out << inputline << QStringLiteral ("\n");
  }
  else
    return CG_ERR_OPEN_FILE;

  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from raw YAHOO finance csv file
static CG_ERR_RESULT
yahoo2csv (const QString & namein, const QString & nameout)
{
  QFile rawcsv, csv;
  QString inputline, outputline;
  QStringList values, column;
  CG_ERR_RESULT retval = CG_ERR_OK;

  inputline.reserve (512);
  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    inputline = in.readLine (0);    // header
    if (!in.atEnd ())
      inputline = in.readLine (0);  // first line
    while (!in.atEnd ())
    {
      inputline += QStringLiteral (",00:00.00");
      column = inputline.split (QStringLiteral(","), QString::KeepEmptyParts);

      if (column.size () >= 8)
      {
        outputline =
          column[0] % QStringLiteral (",") % // date
          column[1] % QStringLiteral (",") % // open
          column[2] % QStringLiteral (",") % // high
          column[3] % QStringLiteral (",") % // low
          column[4] % QStringLiteral (",") % // close
          column[6] % QStringLiteral (",") % // volume
          column[5] % QStringLiteral (",") % // adj close
          column[7]; // time
        out << outputline << QStringLiteral ("\n");
      }
      else
      {
        retval = CG_ERR_INVALID_DATA;
        goto yahoo2csv_end;
      }

      inputline = in.readLine (0);
    }

    if (inputline.size() > 0)
    {
      inputline += QStringLiteral (",00:00.00");
      column = inputline.split (QStringLiteral(","), QString::KeepEmptyParts);\

      if (column.size () >= 8)
      {
        outputline =
          column[0] % QStringLiteral (",") % // date
          column[1] % QStringLiteral (",") % // open
          column[2] % QStringLiteral (",") % // high
          column[3] % QStringLiteral (",") % // low
          column[4] % QStringLiteral (",") % // close
          column[6] % QStringLiteral (",") % // volume
          column[5] % QStringLiteral (",") % // adj close
          column[7]; // time
        out << outputline << QStringLiteral ("\n");
      }
      else
      {
        retval = CG_ERR_INVALID_DATA;
        goto yahoo2csv_end;
      }
    }
  }
  else
    retval = CG_ERR_OPEN_FILE;

yahoo2csv_end:
  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from raw GOOGLE finance csv file
static CG_ERR_RESULT
google2csv (const QString & namein, const QString & nameout)
{
  QFile rawcsv, csv;
  QString inputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  inputline.reserve (512);
  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    inputline = in.readLine (0);    // header
    if (!in.atEnd ())
      do
      {
        QStringList column, datetok;
        QString outputline, datestr;

        datestr.reserve (32);
        outputline.reserve (512);
        inputline = in.readLine (0);
        inputline += QStringLiteral(",00:00.00");
        column = inputline.split (QStringLiteral(","), QString::KeepEmptyParts);
        datetok = column[0].split (QStringLiteral("-"), QString::KeepEmptyParts);
        if (datetok.size () < 3)
        {
          retval = CG_ERR_INVALID_DATA;
          goto google2csv_end;
        }
        if (datetok[2].toInt () < 100)
        {
          datestr = (datetok[2].toInt () > 50?
                     QString::number(datetok[2].toInt () + 1900):
                     QString::number(datetok[2].toInt () + 2000));
        }
        else
          datestr = QString::number(datetok[2].toInt ());
        datestr.append (QStringLiteral ("-"));
        if (datetok.at (1) == QLatin1String ("Jan")) datestr.append (QStringLiteral("01"));
        if (datetok.at (1) == QLatin1String ("Feb")) datestr.append (QStringLiteral("02"));
        if (datetok.at (1) == QLatin1String ("Mar")) datestr.append (QStringLiteral("03"));
        if (datetok.at (1) == QLatin1String ("Apr")) datestr.append (QStringLiteral("04"));
        if (datetok.at (1) == QLatin1String ("May")) datestr.append (QStringLiteral("05"));
        if (datetok.at (1) == QLatin1String ("Jun")) datestr.append (QStringLiteral("06"));
        if (datetok.at (1) == QLatin1String ("Jul")) datestr.append (QStringLiteral("07"));
        if (datetok.at (1) == QLatin1String ("Aug")) datestr.append (QStringLiteral("08"));
        if (datetok.at (1) == QLatin1String ("Sep")) datestr.append (QStringLiteral("09"));
        if (datetok.at (1) == QLatin1String ("Oct")) datestr.append (QStringLiteral("10"));
        if (datetok.at (1) == QLatin1String ("Nov")) datestr.append (QStringLiteral("11"));
        if (datetok.at (1) == QLatin1String ("Dec")) datestr.append (QStringLiteral("12"));
        datestr += QStringLiteral ("-");
        datestr += QString().sprintf("%02d", datetok[0].toInt ());
        outputline = datestr % QStringLiteral (",");

        if (column[4] == QLatin1String ("-")) column[4] = QStringLiteral("0");
        if (column[1] == QLatin1String ("-")|| column[1].toFloat () <= 0) column[1] = column[4];
        if (column[2] == QLatin1String ("-") || column[2].toFloat () <= 0) column[2] = column[4];
        if (column[3] == QLatin1String ("-") || column[3].toFloat () <= 0) column[3] = column[4];
        if (column[5] == QLatin1String ("-")) column[5] = QStringLiteral("0");
        outputline.append (QString (column[1] % QStringLiteral (",") %
                                    column[2] % QStringLiteral (",") %
                                    column[3] % QStringLiteral (",") %
                                    column[4] % QStringLiteral (",") %
                                    column[5] % QStringLiteral (",") %
                                    column[4] % QStringLiteral (",") %
                                    column[6]));
        out << outputline << QStringLiteral("\n");
      }
      while (!in.atEnd ());
  }
  else
    return CG_ERR_OPEN_FILE;

google2csv_end:
  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from raw IEX json file
static CG_ERR_RESULT
iex2csv (const QString & namein, const QString & nameout)
{
  QFile rawjson, csv;
  QString line, outputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  rawjson.setFileName (namein);
  csv.setFileName (nameout);
  if (rawjson.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawjson.close();
      return CG_ERR_OPEN_FILE;
    }

    QTextStream in (&rawjson);
    QTextStream out (&csv);
    in.seek (0);
    line = in.readAll ();   // json is just one line
    if (line.size () != 0)
    {
      QStringList node, value, jsondata;
      QString date, open, high, low, close, volume, adjclose,time = QStringLiteral("00:00.00");

      line.remove ("[");
      line.remove ("]");
      jsondata = line.split(QStringLiteral ("},"));

      foreach (QString data, jsondata)
      {
        data += QStringLiteral ("}");
        data.replace ("}}", "}");
        if (json_parse (data, &node, &value, NULL))
        {
          const qint32 n = node.size ();
          date = open = high = low = close = volume = QStringLiteral ("");

          for (qint32 counter = 0; counter < n; counter ++)
          {
            if (node.at (counter) == QLatin1String ("date"))
              date = value.at (counter);
            else if (node.at (counter) == QLatin1String ("open"))
              open = value.at (counter);
            else if (node.at (counter) == QLatin1String ("high"))
              high = value.at (counter);
            else if (node.at (counter) == QLatin1String ("low"))
              low = value.at (counter);
            else if (node.at (counter) == QLatin1String ("close"))
              close = value.at (counter);
            else if (node.at (counter) == QLatin1String ("volume"))
              volume = value.at (counter);
          }
          adjclose = close;
          outputline = date % QStringLiteral (",") %
                       open % QStringLiteral (",") %
                       high % QStringLiteral (",") %
                       low % QStringLiteral (",") %
                       close % QStringLiteral (",") %
                       volume % QStringLiteral (",") %
                       adjclose % QStringLiteral (",") %
                       time;
          out << outputline << QStringLiteral("\n");
        }
      }
    }
  }
  else
    return CG_ERR_OPEN_FILE;

  rawjson.close ();
  csv.close ();
  return retval;
}

// create csv file from raw STANDARD csv file
static CG_ERR_RESULT
standard2csv (const QString & namein, const QString & nameout, QString & symbolname)
{
  QFile rawcsv, csv;
  QString inputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  inputline.reserve (512);
  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    if (!in.atEnd ())
      inputline = in.readLine (0);  // first line
    while (!in.atEnd ())
    {
      QStringList column, datetok;
      QString outputline, datestr;
      inputline += QStringLiteral (",00:00.00");
      column = inputline.split (QStringLiteral (","), QString::KeepEmptyParts);

      if (column.size () < 7)
        return  CG_ERR_INVALID_DATA;

      symbolname = column[0];
      datetok = column[1].split (QStringLiteral ("-"), QString::KeepEmptyParts);
      if (datetok.size () != 3)
        return  CG_ERR_INVALID_DATA;

      datestr =  QString::number(datetok[2].toInt ()) % QStringLiteral ("-");
      if (datetok.at (1) == QLatin1String ("Jan")) datestr += QStringLiteral ("01");
      else if (datetok.at (1) == QLatin1String ("Feb")) datestr += QStringLiteral ("02");
      else if (datetok.at (1) == QLatin1String ("Mar")) datestr += QStringLiteral ("03");
      else if (datetok.at (1) == QLatin1String ("Apr")) datestr += QStringLiteral ("04");
      else if (datetok.at (1) == QLatin1String ("May")) datestr += QStringLiteral ("05");
      else if (datetok.at (1) == QLatin1String ("Jun")) datestr += QStringLiteral ("06");
      else if (datetok.at (1) == QLatin1String ("Jul")) datestr += QStringLiteral ("07");
      else if (datetok.at (1) == QLatin1String ("Aug")) datestr += QStringLiteral ("08");
      else if (datetok.at (1) == QLatin1String ("Sep")) datestr += QStringLiteral ("09");
      else if (datetok.at (1) == QLatin1String ("Oct")) datestr += QStringLiteral ("10");
      else if (datetok.at (1) == QLatin1String ("Nov")) datestr += QStringLiteral ("11");
      else if (datetok.at (1) == QLatin1String ("Dec")) datestr += QStringLiteral ("12");
      else
        return  CG_ERR_INVALID_DATA;

      datestr += QStringLiteral ("-") % QString().sprintf("%02d", datetok[0].toInt ());
      outputline = datestr % QStringLiteral (",");
      outputline += column[2] % QStringLiteral (",");
      outputline += column[3] % QStringLiteral (",");
      outputline += column[4] % QStringLiteral (",");
      outputline += column[5] % QStringLiteral (",");
      outputline += column[6] % QStringLiteral (",");
      outputline += column[5] % QStringLiteral (",");
      outputline += column[7];
      out << outputline << QStringLiteral ("\n");
      inputline = in.readLine (0);
    }
    if (inputline.size() > 0)
      out << inputline << QStringLiteral ("\n");
  }
  else
    return CG_ERR_OPEN_FILE;

  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from AMI BROKER or METASTOCK7 csv file
static CG_ERR_RESULT
ami2csv (const QString & namein, const QString & nameout, QString & symbolname)
{
  QFile rawcsv, csv;
  QString inputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  inputline.reserve (512);
  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    do
    {
      QStringList column, datetok;
      QString outputline, datestr;

      datestr.reserve (32);
      outputline.reserve (512);

      inputline = in.readLine (0);
      if (inputline.size() > 0)
      {
        inputline += ",00:00.00";
        column = inputline.split (",", QString::KeepEmptyParts);
        symbolname = column[0];
        datetok << column[1].right (2);
        datetok << column[1].mid (4,2);
        datetok << column[1].left (4);
        datestr =  datetok[2] % underscore % datetok.at (1) % underscore % datetok[0];
        outputline = datestr   % QStringLiteral (",") %
                     column[2] % QStringLiteral (",") %
                     column[3] % QStringLiteral (",") %
                     column[4] % QStringLiteral (",") %
                     column[5] % QStringLiteral (",") %
                     column[6] % QStringLiteral (",") %
                     column[5] % QStringLiteral (",") %
                     column[7];
        out << outputline << "\n";
      }
    }
    while (!in.atEnd ());
  }
  else
    return CG_ERR_OPEN_FILE;

  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from AMI BROKER or METASTOCK8 csv file
static CG_ERR_RESULT
metastock8csv (const QString & namein, const QString & nameout, QString & symbolname)
{
  QFile rawcsv, csv;
  QString inputline;
  QStringList values;
  CG_ERR_RESULT retval = CG_ERR_OK;

  rawcsv.setFileName (namein);
  csv.setFileName (nameout);
  if (rawcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
      rawcsv.close();
      return CG_ERR_OPEN_FILE;
    }
    QTextStream in (&rawcsv);
    QTextStream out (&csv);
    in.seek (0);
    if (!in.atEnd ())
      inputline = in.readLine (0);  // first line
    while (!in.atEnd ())
    {
      QStringList column, datetok;
      QString outputline, datestr;
      inputline += ",00:00.00";
      column = inputline.split (",", QString::KeepEmptyParts);
      symbolname = column[0];
      datetok << column[2].right (2);
      datetok << column[2].mid (4,2);
      datetok << column[2].left (4);
      datestr =  datetok[2];
      datestr += "-";
      datestr += datetok.at (1);
      datestr += "-";
      datestr += datetok[0];
      outputline = datestr % QStringLiteral (",");
      outputline += column[3] % QStringLiteral (",");
      outputline += column[4] % QStringLiteral (",");
      outputline += column[5] % QStringLiteral (",");
      outputline += column[6] % QStringLiteral (",");
      outputline += column[7] % QStringLiteral (",");
      outputline += column[6] % QStringLiteral (",");
      outputline += column[8];
      out << outputline << "\n";
      inputline = in.readLine (0);
    }
    if (inputline.size() > 0)
      out << inputline << "\n";
  }
  else
    return CG_ERR_OPEN_FILE;

  rawcsv.close ();
  csv.close ();
  return retval;
}

// create csv file from Microsoft Excel file
struct xlsdata
{
  QString Open;
  QString High;
  QString Low;
  QString Close;
  QString AdjClose;
  QString Volume;
  QString Date;
  QString Time;
};
Q_DECLARE_TYPEINFO (xlsdata, Q_MOVABLE_TYPE);
typedef QList <xlsdata> XLSData;

typedef enum
{
  XLSOPEN,
  XLSHIGH,
  XLSLOW,
  XLSCLOSE,
  XLSVOLUME,
  XLSDATE,
  XLSTIME,
  XLSADJCLOSE
} xlscolumn;

// convert excel serial date to day, month and year
static void
ExcelSerialDateToDMY(int nSerialDate, int &nDay,
                     int &nMonth, int &nYear)
{
  // Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a
  // leap year, but Excel/Lotus 123 think it is...
  if (nSerialDate == 60)
  {
    nDay    = 29;
    nMonth    = 2;
    nYear    = 1900;

    return;
  }
  else if (nSerialDate < 60)
  {
    // Because of the 29-02-1900 bug, any serial date
    // under 60 is one off... Compensate.
    nSerialDate++;
  }

  // Modified Julian to DMY calculation with an addition of 2415019
  int l = nSerialDate + 68569 + 2415019;
  int n = int(( 4 * l ) / 146097);
  l = l - int(( 146097 * n + 3 ) / 4);
  int i = int(( 4000 * ( l + 1 ) ) / 1461001);
  l = l - int(( 1461 * i ) / 4) + 31;
  int j = int(( 80 * l ) / 2447);
  nDay = l - int(( 2447 * j ) / 80);
  l = int(j / 11);
  nMonth = j + 2 - ( 12 * l );
  nYear = 100 * ( n - 49 ) + i + l;
}

// fill excel data from raw data
static void
fillxlsdata (QStringList &rawdata, XLSData &exceldata, int start, int pivot, int col)
{
  int maxcounter = rawdata.size ();
  for (int counter = start, dcounter = 1;
       counter < maxcounter;
       counter += pivot, dcounter ++)
  {
    switch (col)
    {
    case XLSOPEN:
      exceldata[dcounter].Open = rawdata[counter];
      break;
    case XLSCLOSE:
      exceldata[dcounter].Close = rawdata[counter];
      break;
    case XLSHIGH:
      exceldata[dcounter].High = rawdata[counter];
      break;
    case XLSLOW:
      exceldata[dcounter].Low = rawdata[counter];
      break;
    case XLSADJCLOSE:
      exceldata[dcounter].AdjClose = rawdata[counter];
      break;
    case XLSVOLUME:
      exceldata[dcounter].Volume = rawdata[counter];
      break;
    case XLSDATE:
      rawdata[counter] = rawdata[counter].remove ("'");
      if (rawdata[counter].contains ('-', Qt::CaseInsensitive))
        exceldata[dcounter].Date = rawdata[counter];
      else
      {
        QString datestr;
        int day, month, year, serial;
        serial = (int) rawdata[counter].toFloat ();
        ExcelSerialDateToDMY (serial, day, month, year);
        exceldata[dcounter].Date = datestr.sprintf ("%04d-%02d-%02d", year, month, day);
      }
      break;
    case XLSTIME:
      exceldata[dcounter].Time = rawdata[counter];
      break;
    }
  }
}

static CG_ERR_RESULT
excel2csv (const QString & namein, const QString & nameout, QString & symbolname)
{
  Q_UNUSED (symbolname)

  QFile csv;
  QStringList rawdata;
  xls::xlsWorkBook *pWB;
  xls::xlsWorkSheet *pWS;
  xls::WORD maxcol = 0, maxrow = 0, rowcounter = 0;
  CG_ERR_RESULT result = CG_ERR_OK;

  if(!QFile::exists(namein))
    return CG_ERR_OPEN_FILE;

  // parse xls sheet
  pWB = xls::xls_open (namein.toLocal8Bit ().data (), "UTF-8");

  // process all sheets
  // for (quint32 i = 0; i < pWB->sheets.count; i++)
  // {
  // open and parse the sheet
  pWS = xls_getWorkSheet(pWB, 0);
  xls_parseWorkSheet(pWS);
  maxrow = pWS->rows.lastrow + 1;
  maxcol = pWS->rows.lastcol;

  // process all rows of the sheet
  for (xls::WORD cellRow = 0; cellRow < maxrow; cellRow++)
  {
    for (xls::WORD cellCol = 0; cellCol < maxcol; cellCol++)
    {
      xls::xlsCell* cell = xls::xls_cell(pWS, cellRow, cellCol);

      if (cell->str != NULL)
      {
        char *xstr;
        xstr = (char *) malloc (strlen ((const char *) cell->str) + 32);
        if (xstr != NULL)
        {
          strcpy (xstr, (const char *) cell->str);
          if (strtod ((const char *) xstr, NULL) == 0 && cell->d != 0)
            sprintf ((char *) xstr, "%.4f", cell->d);
          rawdata += QString ((QString ((const char *) xstr).toUtf8 ())).toUpper ();
          free (xstr);
        }
        else
          rawdata += "";
      }
      else
        rawdata += "";
    }
    rowcounter ++;
  }
  xls_close_WS(pWS);
  if (GlobalProgressBar != NULL)
    if (GlobalProgressBar->value () < 101)
      GlobalProgressBar->setValue (GlobalProgressBar->value () + 5);
  // }
  xls_close(pWB);

  // create csv file
  csv.setFileName (nameout);
  if (!csv.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    return CG_ERR_OPEN_FILE;

  QTextStream out (&csv);
  XLSData data;
  struct xlsdata dummy;
  dummy.Open = "OPEN";
  dummy.Close = "CLOSE";
  dummy.High = "HIGH";
  dummy.Low = "LOW";
  dummy.AdjClose = "ADJ CLOSE";
  dummy.Volume = "VOLUME";
  dummy.Date = "DATE";
  dummy.Time = "TIME";
  data += dummy;

  dummy.Open = dummy.Close = "0.0";
  dummy.High = dummy.Low = dummy.AdjClose = "0.0";
  dummy.Volume = "0";
  dummy.Date = "0000-00-00";
  dummy.Time = "00:00.00";
  maxrow = rowcounter;

  for (xls::WORD counter = 1; counter < maxrow; counter ++)
    data += dummy;

  for (xls::WORD counter = 0; counter < maxcol; counter ++)
  {
    if (rawdata[counter] == "OPEN")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSOPEN);
    else if (rawdata[counter] == "CLOSE")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSCLOSE);
    else if (rawdata[counter] == "HIGH")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSHIGH);
    else if (rawdata[counter] == "LOW")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSLOW);
    else if (rawdata[counter] == "ADJ CLOSE")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSADJCLOSE);
    else if (rawdata[counter] == "DATE")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSDATE);
    else if (rawdata[counter] == "TIME")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSTIME);
    else if (rawdata[counter] == "VOLUME")
      fillxlsdata (rawdata, data, counter + maxcol, maxcol, XLSVOLUME);
  }

  if (GlobalProgressBar != NULL)
    if (GlobalProgressBar->value () < 101)
      GlobalProgressBar->setValue (GlobalProgressBar->value () + 5);

  QString outputline;
  outputline.reserve (1048576);
  for (xls::WORD counter = 1; counter < maxrow; counter ++)
  {
    outputline = data[counter].Date;
    outputline += QStringLiteral (",") % data[counter].Open %
     QStringLiteral (",") % data[counter].High %
     QStringLiteral (",") % data[counter].Low %
     QStringLiteral (",") % data[counter].Close %
     QStringLiteral (",") % data[counter].Volume %
     QStringLiteral (",") % data[counter].AdjClose %
     QStringLiteral (",") % data[counter].Time;
    out << outputline << "\n";
  }
  if (GlobalProgressBar != NULL)
    if (GlobalProgressBar->value () < 101)
      GlobalProgressBar->setValue (GlobalProgressBar->value () + 5);

  csv.close ();
  return result;
}

/// General purpose csv functions
// normalize table names
static void
tablename_normal (QString & tname)
{
  QStringList column, oldc, substc;

  oldc   <<"."<<"="<<"/"<<"!"<<"@"<<"#"<<"$"<<"%"<<"^"<<"&"<<"*"<<"("<<")"<<
         "+"<<"-"<<","<< " "<<":" << "[" << "]" << "{" << "}" << "|"<<"<"<<">";
  substc <<"_"<<"a"<<"b"<<"c"<<"d"<<"e"<<"f"<<"g"<<"h"<<"i"<<"j"<<"k"<<"l"<<
         "m"<<"n" << "o"<<"_"<<"q"<<"r"<<"s"<<"t"<<"u"<<"v"<<"w"<<"x"<<"y"<<"z";

  for (qint32 counter = 0, maxcounter = oldc.size ();
       counter < maxcounter; counter ++)
    tname.replace (oldc[counter], substc[counter]);

  if (tname.indexOf(QRegExp("[0-9]"), 0) == 0)
    tname.insert(0, underscore);

  tname = tname.toUpper ();
}

// normalize table names
/*
static void
tablename_normal_old (QString & tname)
{
  QStringList column, oldc, substc;

  oldc   <<"."<<"="<<"/"<<"!"<<"@"<<"#"<<"$"<<"%"<<"^"<<"&"<<"*"<<"("<<")"<<
         "+"<<"-"<<","<< " "<<":" << "[" << "]" << "{" << "}" << "|"<<"<"<<">";
  substc <<"_"<<"a"<<"b"<<"c"<<"d"<<"e"<<"f"<<"g"<<"h"<<"i"<<"j"<<"k"<<"l"<<
         "m"<<"n" << "o"<<"p"<<"q"<<"r"<<"s"<<"t"<<"u"<<"v"<<"w"<<"x"<<"y"<<"z";

  for (qint32 counter = 0, maxcounter = oldc.size ();
       counter < maxcounter; counter ++)
    tname.replace (oldc[counter], substc[counter]);

  if (tname.indexOf(QRegExp("[0-9]"), 0) == 0)
    tname.insert(0, underscore);

  tname = tname.toUpper ();
}
*/

// Return and SQL statement to drop a view if exists
static QString
dropview (QString viewname)
{
  return QStringLiteral ("DROP VIEW IF EXISTS ") % viewname % QStringLiteral (";");
}

// Returns an SQL statement that creates the timeframe views
static QString
tfview (QString tablename, QString tf, SymbolEntry *data, QString operation, bool adjust)
{
  QString SQLCommand = "",
          vname,
          viewclose, viewopen, viewhigh, viewlow, viewmindate,
          viewmaxdate, viewvolume;

  vname = tablename % underscore % tf;

  if (operation == QLatin1String ("UPDATE"))
    goto tfview_end;

  viewclose = vname % underscore % QStringLiteral ("_CLOSE");
  viewopen = vname % underscore % QStringLiteral ("_OPEN");
  viewhigh = vname % underscore % QStringLiteral ("_HIGH");
  viewlow = vname % underscore % QStringLiteral ("_LOW");
  viewmindate = vname % underscore % QStringLiteral ("_MINDATE");
  viewmaxdate = vname % underscore % QStringLiteral ("_MAXDATE");
  viewvolume = vname % underscore % QStringLiteral ("_VOLUME");

  // drop all views
  SQLCommand.reserve (1048576);
  SQLCommand.append
  (dropview (vname) %
   dropview (viewclose) %
   dropview (viewopen) %
   dropview (viewhigh) %
   dropview (viewlow) %
   dropview (viewmindate) %
   dropview (viewmaxdate) %
   dropview (viewvolume));
  SQLCommand.append ('\n');

  // maxdate
  SQLCommand.append (QStringLiteral ("CREATE VIEW ") % viewmaxdate %
                     QStringLiteral (" AS SELECT MAX (DATE) AS DATE, ") % tf % QStringLiteral (" FROM ") %
                     tablename % QStringLiteral (" GROUP BY ") % tf % QStringLiteral (";"));
  SQLCommand.append ('\n');

  // mindate
  SQLCommand.append (QStringLiteral ("CREATE VIEW ") % viewmindate %
                     QStringLiteral (" AS SELECT MIN (DATE) AS DATE, ") % tf %
                     QStringLiteral (" FROM ") % tablename % QStringLiteral (" GROUP BY ") %
                     tf % QStringLiteral (";"));
  SQLCommand.append ('\n');

  // tf low
  SQLCommand += QStringLiteral ("CREATE VIEW ") % viewlow %
                QStringLiteral (" AS SELECT MIN (LOW) AS LOW, ") % tf % QStringLiteral (" FROM ") %
                tablename % QStringLiteral (" GROUP BY ") % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // tf high
  SQLCommand += QStringLiteral ("CREATE VIEW ") % viewhigh %
                QStringLiteral (" AS SELECT MAX (HIGH) AS HIGH, ") % tf % QStringLiteral (" FROM ") %
                tablename % QStringLiteral (" GROUP BY ") % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // tf open
  SQLCommand += QStringLiteral ("CREATE VIEW ") % viewopen %
                QStringLiteral (" AS SELECT ") % tablename % QStringLiteral (".OPEN, ") %
                tablename % dot % tf % QStringLiteral (", ") %
                tablename % QStringLiteral (".DATE, ") %
                tablename % QStringLiteral (".TIME FROM ") %
                tablename % QStringLiteral (", ") % viewmindate % QStringLiteral (" WHERE ") %
                tablename % QStringLiteral (".DATE = ") % viewmindate % QStringLiteral (".DATE AND ") %
                tablename % dot % tf % QStringLiteral (" =  ") % viewmindate %
                dot % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // tf close
  SQLCommand += QStringLiteral ("CREATE VIEW ") % viewclose %
                QStringLiteral (" AS SELECT ") % tablename % QStringLiteral (".CLOSE, ") %
                tablename % QStringLiteral (".CLOSE AS ADJCLOSE, ") %
                tablename % dot % tf % QStringLiteral (" FROM ") % tablename %
                QStringLiteral (", ") % viewmaxdate % QStringLiteral (" WHERE ") %
                tablename % QStringLiteral (".DATE = ") % viewmaxdate % QStringLiteral (".DATE AND ") %
                tablename % dot % tf % QStringLiteral (" =  ") % viewmaxdate %
                dot % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // tf volume
  SQLCommand += QStringLiteral ("CREATE VIEW ") % viewvolume %
                QStringLiteral (" AS SELECT SUM (") % tablename % QStringLiteral (".VOLUME ) AS VOLUME, ") %
                tablename % dot % tf % QStringLiteral (" FROM ") %
                tablename % QStringLiteral (" GROUP BY ") % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // final tf view
  SQLCommand += QStringLiteral ("CREATE VIEW ") % vname %
                QStringLiteral (" AS SELECT ") %
                viewhigh % QStringLiteral (".HIGH AS HIGH, ") %
                viewlow % QStringLiteral (".LOW AS LOW, ") %
                viewopen % QStringLiteral (".OPEN AS OPEN, ") %
                viewclose % QStringLiteral (".CLOSE AS CLOSE, ") %
                viewvolume % QStringLiteral (".VOLUME AS VOLUME, ") %
                viewopen % QStringLiteral (".DATE AS DATE, ") %
                viewopen % QStringLiteral (".TIME AS TIME, ") %
                viewclose % QStringLiteral (".ADJCLOSE AS ADJCLOSE FROM ") %
                viewhigh % QStringLiteral (", ") % viewlow % QStringLiteral (", ") % viewopen % QStringLiteral (", ") %
                viewclose % QStringLiteral (", ") % viewvolume % QStringLiteral (" WHERE ") %
                viewhigh % dot % tf % QStringLiteral (" = ") % viewlow % dot % tf % QStringLiteral (" AND ") %
                viewhigh % dot % tf % QStringLiteral (" = ") % viewopen % dot % tf % QStringLiteral (" AND ") %
                viewhigh % dot % tf % QStringLiteral (" = ") % viewclose % dot % tf % QStringLiteral (" AND ") %
                viewhigh % dot % tf % QStringLiteral (" = ") % viewvolume % dot % tf % QStringLiteral (";");
  SQLCommand.append ('\n');

  // remove symbol from symbols table
  SQLCommand += QStringLiteral ("delete from SYMBOLS where KEY='") % vname % QStringLiteral ("';");
  SQLCommand.append ('\n');

  // insert symbol into symbols table
  SQLCommand += QStringLiteral("INSERT INTO SYMBOLS VALUES ('") %
                data->symbol % QStringLiteral("','") %
                data->name % QStringLiteral("','") %
                data->market % QStringLiteral("','") %
                data->source % QStringLiteral("','") %
                tf % QStringLiteral("','") %
                QStringLiteral(" ") % QStringLiteral("','") %
                QStringLiteral(" ") % QStringLiteral("','") %
                vname % QStringLiteral("','") %
                data->currency % QStringLiteral("',0,'") %
                data->dnlstring % QStringLiteral("',");
  if (adjust)
    SQLCommand += QStringLiteral("'YES', ");
  else
    SQLCommand += QStringLiteral("'NO', ");
  SQLCommand += QStringLiteral ("'") % data->tablename % QStringLiteral("', '") % data->format % QStringLiteral("', 0, '');");
  SQLCommand.append ('\n');

tfview_end:
  // update DATEFROM, DATETO and TFRESOLUTION of symbol entry

  SQLCommand += QStringLiteral("update SYMBOLS set DATEFROM = (select min(DATE) from ") %
                vname % QStringLiteral(") where KEY='") % vname % QStringLiteral("';");
  SQLCommand.append ('\n');

  SQLCommand += QStringLiteral("update SYMBOLS set DATETO = (select max(DATE) from ") %
                vname % QStringLiteral(") where KEY='") % vname % QStringLiteral("';");
  SQLCommand.append ('\n');

  SQLCommand += QStringLiteral("update SYMBOLS set TFRESOLUTION = (select MINUTE_RESOLUTION ") %
    QStringLiteral("from TIMEFRAMES where SYMBOLS.TIMEFRAME = TIMEFRAMES.TIMEFRAME);");

  return SQLCommand;
}

// CSV Line 2 SQL
// Returns an SQL INSERT command
const QString
csvline2SQL (QString &csvline, QString &tablename)
{
  QDate datevar;
  QStringList column, yyyymmdd;
  QString inputline, lastclose = QStringLiteral ("0.001"),
                     daynum, weeknum, SQL = QStringLiteral ("");

  inputline = csvline;
  column = inputline.split(comma, QString::KeepEmptyParts);

  if (column.size () < 8)
  {
    for (qint32 counter = column.size (); counter < 8; counter ++)
      column += QStringLiteral ("");
  }

  yyyymmdd = column[0].split(dash, QString::KeepEmptyParts);
  if (yyyymmdd.size () != 3)
    return SQL;

  for (qint32 counter = 0; counter < 8; counter ++)
  {
    if (column[counter].size () > 17)
      column[counter] = QStringLiteral ("");

    if (column[counter] == QLatin1String (""))
      column[counter] = QStringLiteral ("0");
  }

  // ATTENTION: Fix zero values. Possible FDIV error
  if (column[4].toFloat () <= 0.0001)
    column[4] = lastclose;

  if (column[1].toFloat () <= 0.0001)
    column[1] = column[4];

  if (column[2].toFloat () <= 0.0001)
    column[2] = column[4];

  if (column[3].toFloat () <= 0.0001)
    column[3] = column[4];

  if (column[5].toFloat () == 0.0)
    column[5] = QStringLiteral ("0");

  if (column[6].toFloat () <= 0.0001)
    column[6] = column[4];
  // END OF FIX

  datevar.setDate (yyyymmdd[0].toInt (),
                   yyyymmdd[1].toInt (),
                   yyyymmdd[2].toInt ());
  daynum = QString::number(datevar.dayOfYear ());
  if (daynum.size () == 1)
    daynum = QStringLiteral ("00") % daynum;
  if (daynum.size () == 2)
    daynum = QStringLiteral ("0") % daynum;
  daynum = yyyymmdd[0] % daynum;

  if (datevar.weekNumber (NULL) == 1 &&  yyyymmdd[1].toInt () == 12)
  {
    weeknum = QStringLiteral ("01");
    weeknum = QString::number (yyyymmdd[0].toInt () + 1) % weeknum;
  }
  else
  {
    weeknum = QString::number(datevar.weekNumber (NULL));
    if (weeknum.size () == 1)
      weeknum = QStringLiteral ("0")% weeknum;
    weeknum = yyyymmdd[0] % weeknum;
  }

  SQL = QStringLiteral ("insert into ") % tablename %
        QStringLiteral (" (OPEN, HIGH, LOW, CLOSE, VOLUME, ADJCLOSE, MONTH, YEAR, DAY, WEEK, DATE, TIME) values (") %
        column[1] % QStringLiteral (",") %
        column[2] % QStringLiteral (",") %
        column[3] % QStringLiteral (",") %
        column[4] % QStringLiteral (",") %
        column[5] % QStringLiteral (",") %
        column[6] % QStringLiteral (",") %
        yyyymmdd[0] % yyyymmdd[1] % QStringLiteral (",") %
        yyyymmdd[0] % QStringLiteral (",") %
        daynum % QStringLiteral (",") %
        weeknum % QStringLiteral (",") %
        QStringLiteral ("'") % column[0] % QStringLiteral ("',") %
        QStringLiteral ("'") % column[7] % QStringLiteral ("');");

  return SQL;
}

// check if view adjusted exists
static int
sqlcb_checkexistence (void *cnt, int argc, char **argv, char **column)
{
  Q_UNUSED (column)
  Q_UNUSED (argc)
  int *counter;

  counter = (int *) cnt;
  *counter = QString (argv[0]).toInt ();

  return 0;
}

// CSV 2 SQLITE:
// Returns CG_ERR_OK on success, an error otherwise
CG_ERR_RESULT
csv2sqlite (SymbolEntry *data, QString operation)
{
  QFile tmpcsv;
  QTemporaryFile *tempfile;
  QString tempfilename, SQLCommand = "", inputline,
                        indexname, adjustedviewname,
                        symbol;
  CG_ERR_RESULT result = CG_ERR_OK;
  int rc;

  // SQLCommand.reserve (12582912); //reserve 12M
  tablename_normal (data->tablename);
  tablename_normal (data->tmptablename);

  adjustedviewname = data->tablename % QStringLiteral("_ADJUSTED");
  data->name.remove ("'");

  tempfile = new QTemporaryFile ;
  if (tempfile->open ())
    tempfilename = tempfile->fileName ();
  else
  {
    result = CG_ERR_OPEN_FILE;
    setGlobalError(result, __FILE__, __LINE__);
  }
  delete tempfile;

  if (result == CG_ERR_OK)
  {
    if (data->format == QLatin1String ("YAHOO CSV"))
    {
      if (data->source == QLatin1String ("FOREX"))
        data->adjust = false;
      else
        data->adjust = true;

      if (data->source == QLatin1String ("QUANDL (INDICES)") ||
          data->source == QLatin1String ("FOREX"))
        result = yahoo2csv_old (data->csvfile, tempfilename);
      else
        result = yahoo2csv (data->csvfile, tempfilename);
    }

    if (data->format == QLatin1String ("GOOGLE CSV"))
    {
      data->adjust = false;
      result = google2csv (data->csvfile, tempfilename);
    }

    if (data->format == QLatin1String ("IEX JSON"))
    {
      data->adjust = false;
      result = iex2csv (data->csvfile, tempfilename);
    }

    if (data->format == QLatin1String ("STANDARD CSV"))
    {
      data->adjust = false;
      result = standard2csv (data->csvfile, tempfilename, symbol);
    }

    if (data->format == QLatin1String ("AMI BROKER") ||
        data->format == QLatin1String ("METASTOCK ASCII 7"))
    {
      data->adjust = false;
      result = ami2csv (data->csvfile, tempfilename, symbol);
    }

    if (data->format == QLatin1String ("METASTOCK ASCII 8"))
    {
      data->adjust = false;
      result = metastock8csv (data->csvfile, tempfilename, symbol);
    }

    if (data->format == QLatin1String ("MICROSOFT EXCEL"))
    {
      // data->adjust = true;
      result = excel2csv (data->csvfile, tempfilename, symbol);
    }
  }

  if (result != CG_ERR_OK)
    goto csv2sqlite_end;

  // drop temporary table
  SQLCommand += QStringLiteral ("drop table if exists ") % data->tmptablename % QStringLiteral ("; ");

  // create temporary table
  SQLCommand += QStringLiteral ("create temporary table ") % data->tmptablename %
                QStringLiteral (" as select * from DATAMODEL;");

  // fill temporary table
  tmpcsv.setFileName (tempfilename);
  if (tmpcsv.open (QIODevice::ReadOnly|QIODevice::Text))
  {
    QTextStream in (&tmpcsv);
    do
    {
      QString SQL;
      inputline = in.readLine (0);

      if (GlobalProgressBar != NULL)
        if (GlobalProgressBar->value () < 101)
          GlobalProgressBar->setValue (GlobalProgressBar->value () + 1);

      SQL = csvline2SQL (inputline, data->tmptablename);

      if (SQL != QStringLiteral (""))
      {
        SQLCommand.append ('\n');
        SQLCommand.append (SQL);
      }

    }
    while (!in.atEnd ());
    tmpcsv.close ();
  }
  SQLCommand.append ('\n');

  // drop data table
  SQLCommand += QStringLiteral ("drop table if exists ") % data->tablename % QStringLiteral ("; ");
  SQLCommand += QStringLiteral ("drop view if exists ") % data->tablename % QStringLiteral ("; ");
  SQLCommand.append ('\n');

  // create data table
  SQLCommand += QStringLiteral ("create table ") % data->tablename %
                QStringLiteral (" (OPEN REAL NOT NULL, HIGH REAL  NOT NULL,") %
                QStringLiteral (" LOW REAL NOT NULL, CLOSE REAL NOT NULL, VOLUME INTEGER,") %
                QStringLiteral (" ADJCLOSE REAL NOT NULL, DATE TEXT NOT NULL, TIME TEXT,") %
                QStringLiteral (" MONTH INTEGER, YEAR INTEGER, DAY INTEGER, WEEK INTEGER, ") %
                QStringLiteral (" TICK INTEGER PRIMARY KEY AUTOINCREMENT);");
  SQLCommand.append ('\n');

  // copy temporary table to data table
  SQLCommand += QStringLiteral ("insert into ") % data->tablename %
                QStringLiteral (" (OPEN, HIGH, LOW, CLOSE, VOLUME, ADJCLOSE, MONTH, YEAR, DAY, WEEK, DATE, TIME)") %
                QStringLiteral (" select OPEN, HIGH, LOW, CLOSE, VOLUME, ADJCLOSE, MONTH, YEAR, DAY, WEEK, DATE, TIME from ") %
                data->tmptablename % QStringLiteral (" GROUP BY DATE, TIME ORDER BY DATE ASC;");
  SQLCommand.append ('\n');

  SQLCommand += QStringLiteral ("DELETE FROM ") % data->tablename %
                QStringLiteral (" WHERE OPEN = 0.001 AND CLOSE = 0.001 AND HIGH = 0.001 AND LOW = 0.001; ");
  SQLCommand += QStringLiteral ("UPDATE ") % data->tablename %
                QStringLiteral (" SET HIGH = OPEN WHERE OPEN >= CLOSE AND HIGH < OPEN;");
  SQLCommand += QStringLiteral ("UPDATE ") % data->tablename %
                QStringLiteral (" SET LOW = CLOSE WHERE OPEN >= CLOSE AND LOW > CLOSE;");
  SQLCommand += QStringLiteral ("UPDATE ") % data->tablename %
                QStringLiteral (" SET HIGH = CLOSE WHERE OPEN <= CLOSE AND HIGH < CLOSE;");
  SQLCommand += QStringLiteral ("UPDATE ") % data->tablename %
                QStringLiteral (" SET LOW = CLOSE WHERE OPEN <= CLOSE AND LOW > OPEN;");

  // correct ADJCLOSE if invalid
  SQLCommand += QStringLiteral ("update ") % data->tablename %
                QStringLiteral (" set ADJCLOSE = 0 where CLOSE = 0;") %
                QStringLiteral ("update ") % data->tablename %
                QStringLiteral (" set ADJCLOSE = CLOSE where ADJCLOSE < 0;");
  SQLCommand.append ('\n');

  // remove symbol from symbols table
  SQLCommand += QStringLiteral ("delete from SYMBOLS where KEY='") % data->tablename % QStringLiteral ("';");
  SQLCommand.append ('\n');

  // insert symbol into symbols table
  SQLCommand += QStringLiteral ("insert into SYMBOLS values ('") %
                data->symbol % QStringLiteral ("','") %
                data->name % QStringLiteral ("','") %
                data->market % QStringLiteral ("','") %
                data->source % QStringLiteral ("','") %
                data->timeframe % QStringLiteral ("','") %
                QStringLiteral (" ") % QStringLiteral ("','") %
                QStringLiteral (" ") % QStringLiteral ("','") %
                data->tablename % QStringLiteral ("','") %
                data->currency % QStringLiteral ("',0,'") %
                data->dnlstring % QStringLiteral ("',") %
                QStringLiteral ("'NO', ") %
                QStringLiteral ("'") % data->tablename % QStringLiteral ("', '") % data->format % QStringLiteral ("', 0, '');");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("INSERT OR IGNORE INTO CURRENCIES (SYMBOL) VALUES ('") %
                data->currency % QStringLiteral ("');") %

                // update DATEFROM, DATETO and TFRESOLUTION of symbol entry
                QStringLiteral ("update SYMBOLS set DATEFROM = (select min(DATE) from ") %
                data->tablename % QStringLiteral (") where KEY='") % data->tablename %
                QStringLiteral ("';");
  SQLCommand.append ('\n');

  SQLCommand += QStringLiteral ("update SYMBOLS set DATETO = (select max(DATE) from ") %
                data->tablename % QStringLiteral (") where KEY='") % data->tablename %
                QStringLiteral ("';");
  SQLCommand.append ('\n');

  SQLCommand += QStringLiteral ("update SYMBOLS set TFRESOLUTION = (select MINUTE_RESOLUTION ") %
                QStringLiteral ("from TIMEFRAMES where SYMBOLS.TIMEFRAME = TIMEFRAMES.TIMEFRAME);");

  // drop temporary table
  SQLCommand += QStringLiteral ("drop table if exists ") % data->tmptablename % QStringLiteral ("; ");
  SQLCommand.append ('\n');

  // create indexes
  indexname = data->tablename % QStringLiteral ("_monthidx");
  SQLCommand += QStringLiteral ("CREATE INDEX '") % indexname % QStringLiteral ("' on ") %
                data->tablename % QStringLiteral (" (MONTH ASC);");
  SQLCommand.append ('\n');

  indexname = data->tablename % QStringLiteral ("_dateidx");
  SQLCommand += QStringLiteral ("CREATE INDEX '") % indexname % QStringLiteral ("' on ") %
                data->tablename % QStringLiteral (" (DATE ASC);");
  SQLCommand.append ('\n');

  indexname = data->tablename % QStringLiteral ("_weekidx");
  SQLCommand += QStringLiteral ("CREATE INDEX '") % indexname % QStringLiteral ("' on ") %
                data->tablename % QStringLiteral (" (WEEK ASC);");
  SQLCommand.append ('\n');

  indexname = data->tablename % QStringLiteral ("_yearidx");
  SQLCommand += QStringLiteral ("CREATE INDEX '") % indexname % QStringLiteral ("' on ") %
                data->tablename % QStringLiteral (" (YEAR ASC);");
  SQLCommand.append ('\n');

  // create week, month, year views for unadjusted data
  if (operation == QLatin1String ("CREATE") /*&& data->source != "CSV"*/)
  {
    SQLCommand += tfview (data->tablename, QStringLiteral ("WEEK"), data, operation, false) %
                  tfview (data->tablename, QStringLiteral ("MONTH"), data, operation, false) %
                  tfview (data->tablename, QStringLiteral ("YEAR"), data, operation, false);
  }

  // create adjusted view
  if (operation == QLatin1String ("CREATE") && data->adjust == true)
  {
    SQLCommand += dropview (adjustedviewname);
    SQLCommand.append ('\n');
    SQLCommand +=
      QStringLiteral ("CREATE VIEW ") % adjustedviewname %
      QStringLiteral (" AS SELECT ") % data->tablename %
      QStringLiteral (".OPEN*(") % data->tablename %
      QStringLiteral (".ADJCLOSE/") % data->tablename %
      QStringLiteral (".CLOSE) AS OPEN,") % data->tablename %
      QStringLiteral (".HIGH*(") % data->tablename %
      QStringLiteral (".ADJCLOSE/") % data->tablename %
      QStringLiteral (".CLOSE) AS HIGH,") % data->tablename %
      QStringLiteral (".LOW*(") % data->tablename %
      QStringLiteral (".ADJCLOSE/") % data->tablename %
      QStringLiteral (".CLOSE) AS LOW,") % data->tablename %
      QStringLiteral (".CLOSE*(") % data->tablename %
      QStringLiteral (".ADJCLOSE/") % data->tablename %
      QStringLiteral (".CLOSE) AS CLOSE,") % data->tablename %
      QStringLiteral (".VOLUME as VOLUME,") % data->tablename %
      QStringLiteral (".ADJCLOSE as ADJCLOSE,") % data->tablename %
      QStringLiteral (".DATE as DATE,") % data->tablename %
      QStringLiteral (".TIME as TIME,") % data->tablename %
      QStringLiteral (".MONTH as MONTH,") % data->tablename %
      QStringLiteral (".YEAR as YEAR,") % data->tablename %
      QStringLiteral (".DAY as DAY,") % data->tablename %
      QStringLiteral (".WEEK as WEEK,") % data->tablename %
      QStringLiteral (".TICK as TICK from ") % data->tablename %
      QStringLiteral (";");
    SQLCommand.append ('\n');

    SQLCommand +=
      QStringLiteral ("UPDATE ") %  data->tablename %
      QStringLiteral (" SET ADJCLOSE = CLOSE WHERE ") %
      QStringLiteral (" (SELECT  MAX (high)/MIN(low) FROM ") % adjustedviewname %
      QStringLiteral (") > 5000;");

    SQLCommand.append ('\n');

    // remove symbol from symbols table
    SQLCommand +=
      QStringLiteral ("delete from SYMBOLS where KEY='") % adjustedviewname %
      QStringLiteral ("';");
    SQLCommand.append ('\n');

    // insert adjusted symbol into symbols table
    SQLCommand += QStringLiteral ("insert into SYMBOLS values ('") %
                  data->symbol % QStringLiteral ("','") %
                  data->name % QStringLiteral ("','") %
                  data->market % QStringLiteral ("','") %
                  data->source % QStringLiteral ("','") %
                  data->timeframe % QStringLiteral ("','") %
                  QStringLiteral (" ") % QStringLiteral ("','") %
                  QStringLiteral (" ") % QStringLiteral ("','") %
                  adjustedviewname % QStringLiteral ("','") %
                  data->currency % QStringLiteral ("',0,'") %
                  data->dnlstring % QStringLiteral ("',") %
                  QStringLiteral ("'YES', ") %
                  QStringLiteral ("'") % data->tablename %
                  QStringLiteral ("', '") % data->format %
                  QStringLiteral ("', 0, '');");
    SQLCommand.append ('\n');

    // update DATEFROM, DATETO and TFRESOLUTION of adjusted view entry
    SQLCommand +=
      QStringLiteral ("update SYMBOLS set DATEFROM = (select min(DATE) from ") %
      adjustedviewname % QStringLiteral (") where KEY='") % adjustedviewname %
      QStringLiteral ("';");
    SQLCommand.append ('\n');

    SQLCommand +=
      QStringLiteral ("update SYMBOLS set DATETO = (select max(DATE) from ") %
      adjustedviewname % QStringLiteral (") where KEY='") % adjustedviewname %
      QStringLiteral ("';");
    SQLCommand.append ('\n');

    SQLCommand +=
      QStringLiteral ("update SYMBOLS set TFRESOLUTION = (select MINUTE_RESOLUTION ");
    SQLCommand +=
      QStringLiteral ("from TIMEFRAMES where SYMBOLS.TIMEFRAME = TIMEFRAMES.TIMEFRAME);");
    SQLCommand.append ('\n');

    // create week, month, year views for adjusted data
    SQLCommand += tfview (adjustedviewname, QStringLiteral ("WEEK"), data, operation, data->adjust) %
                  tfview (adjustedviewname, QStringLiteral ("MONTH"), data, operation, data->adjust) %
                  tfview (adjustedviewname, QStringLiteral ("YEAR"), data, operation, data->adjust);
    SQLCommand.append ('\n');
  }

  // insert or update statistics (basedata table)
  SQLCommand +=
    QStringLiteral ("delete from basedata where base = '") % data->tablename %
    QStringLiteral ("';");
  SQLCommand.append ('\n');
  SQLCommand +=
    QStringLiteral ("insert into basedata (base, bv, mc, ebitda, pe, peg, dy, epscurrent, epsnext, es, ps, pbv) ") %
    QStringLiteral ("values ('") % data->tablename % QStringLiteral ("','") %
    data->BookValue % QStringLiteral ("','") % data->MarketCap %
    QStringLiteral ("','") % data->EBITDA % QStringLiteral ("','") % data->PE %
    QStringLiteral ("','") % data->PEG % QStringLiteral ("','") %
    data->Yield % QStringLiteral ("','") % data->EPScy %  QStringLiteral ("','") %
    data->EPSny % QStringLiteral ("','") % data->ES %  QStringLiteral ("','") %
    data->PS %  QStringLiteral ("','") % data->PBv % QStringLiteral ("');");
  SQLCommand.append ('\n');

  // correct forex last update timestamp
  if (data->source == QLatin1String ("FOREX"))
    SQLCommand += QStringLiteral ("UPDATE SYMBOLS  SET LASTUPDATE = (SELECT NETFONDSFOREXUPDATE FROM VERSION) WHERE SOURCE = 'FOREX';");
  SQLCommand.append ('\n');

  // just update symbol table's date from and date to columns
  if (operation == QLatin1String ("UPDATE"))
  {
    QString SQL;
    int cnt = 0;

    SQL =
      QStringLiteral ("SELECT count(*) AS cnt FROM sqlite_master WHERE type = 'view' AND tbl_name = '") %
      adjustedviewname % QStringLiteral ("';");
    rc = selectfromdb(SQL.toUtf8(), sqlcb_checkexistence,
                      static_cast <void*> (&cnt));
    if (rc != SQLITE_OK)
      cnt = 0;

    // update DATEFROM, DATETO and TFRESOLUTION of adjusted view entry
    if (data->adjust && cnt > 0)
    {
      SQLCommand +=

        QStringLiteral ("UPDATE ") %  data->tablename %
        QStringLiteral (" SET ADJCLOSE = CLOSE WHERE ") %
        QStringLiteral (" (SELECT  MAX (high)/MIN(low) FROM ") %
        adjustedviewname % QStringLiteral (") > 5000;") %
        QStringLiteral ("update SYMBOLS set DATEFROM = (select min(DATE) from ") %
        adjustedviewname % QStringLiteral (") where KEY='") % adjustedviewname %
        QStringLiteral ("';") %
        QStringLiteral ("update SYMBOLS set DATETO = (select max(DATE) from ") %
        adjustedviewname % QStringLiteral (") where KEY='") % adjustedviewname %
        QStringLiteral ("';") %
        QStringLiteral ("update SYMBOLS set TFRESOLUTION = (select MINUTE_RESOLUTION ") %
        QStringLiteral ("from TIMEFRAMES where SYMBOLS.TIMEFRAME = TIMEFRAMES.TIMEFRAME);") %
        QStringLiteral ("update SYMBOLS set CURRENCY = '") % data->currency %
        QStringLiteral ("' where KEY='") % adjustedviewname %
        QStringLiteral ("';");
    }

    SQLCommand +=
      tfview (data->tablename, QStringLiteral ("WEEK"), data, operation, data->adjust) %
      tfview (data->tablename, QStringLiteral ("MONTH"), data, operation, data->adjust) %
      tfview (data->tablename, QStringLiteral ("YEAR"), data, operation, data->adjust);

    if (data->adjust && cnt > 0)
    {
      SQLCommand +=
        tfview (adjustedviewname, QStringLiteral ("WEEK"), data, operation, data->adjust) %
        tfview (adjustedviewname, QStringLiteral ("MONTH"), data, operation, data->adjust) %
        tfview (adjustedviewname, QStringLiteral ("YEAR"), data, operation, data->adjust) %

        // update timestamp
        QStringLiteral ("UPDATE SYMBOLS SET LASTUPDATE = strftime('%s', 'now') ") %
        QStringLiteral ("where KEY='") % adjustedviewname %
        QStringLiteral ("' AND LASTUPDATE <> strftime('%s', 'now');");
    }

    // update download string
    SQLCommand +=
      QStringLiteral ("UPDATE SYMBOLS SET DNLSTRING = '") % data->dnlstring %
      QStringLiteral ("' ") % QStringLiteral ("WHERE BASE = '") % data->tablename %
      QStringLiteral ("';") %

      // update currency
      QStringLiteral ("UPDATE SYMBOLS SET CURRENCY = '") % data->currency %
      QStringLiteral ("' WHERE BASE = '") % data->tablename % QStringLiteral ("';");
    // correct forex last update timestamp
    if (data->source == "FOREX")
      SQLCommand +=
        QStringLiteral ("UPDATE SYMBOLS  SET LASTUPDATE = (SELECT NETFONDSFOREXUPDATE FROM VERSION) WHERE SOURCE = 'FOREX'");
  }

  // execute sql
  rc =  updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    result = CG_ERR_TRANSACTION;
    setGlobalError(result, __FILE__, __LINE__);
    goto csv2sqlite_end;
  }

csv2sqlite_end:
  // remove temporary file
  QFile::remove(tempfilename);
  return result;
}


