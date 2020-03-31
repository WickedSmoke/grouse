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

#include <QPushButton>
#include <QScrollBar>
#include <QTextStream>
#include <QTemporaryFile>
#include "common.h"
#include "qtachart_core.h"
#include "qtachart_object.h"
#include "templatemanagerdialog.h"
#include "ui_templatemanagerdialog.h"
#include "simplecrypt.h"
#ifdef GUI_DESKTOP
#include "ParameterDialog.h"
#else
#include "dynparamsdialog.h"
#endif

// keep indicator data localy
struct Lindicator
{
  QString Title;
  ParamVector Plist;
};

// keep drawing object's data localy
struct Lobject
{
  QString Title;
  QString fontfamily;
  QString text;
  QString trailerCandleText;
  QString trailerCandleText2;
  double color;
  qreal price, price2, pad, pad2, x1, x2, y1, y2;
  qint32 type;
  qint32 fontsize;
  qint32 fontweight;
  qint16 txtdirection, txtdirection2;
};

// constructor
TemplateManagerDialog::TemplateManagerDialog (QWidget * parent):
  QDialog (parent), ui (new Ui::TemplateManagerDialog)
{
  const QString
  stylesheet = QStringLiteral ("background: transparent; background-color: white;"),
  tooltipstyle = QStringLiteral ("<span style=\"border: 1px solid transparent;border-color: black;background-color:white; color: black; font: 12px;\">");
  QStringList cheadersList;

  ui->setupUi (this);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Ok);
  correctButtonBoxFonts (ui->buttonBox, QDialogButtonBox::Cancel);

  cheadersList << QStringLiteral (" Indicator Templates ")
               << QStringLiteral (" Serial Number ")
               << QStringLiteral (" Table Name ")
               << QStringLiteral (" SQL Statement");
  this->setStyleSheet (stylesheet);
  ui->tableWidget->setColumnCount (4);
  ui->tableWidget->setHorizontalHeaderLabels (cheadersList);
  ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
  ui->tableWidget->sortByColumn (1, Qt::AscendingOrder);
  ui->tableWidget->setSortingEnabled(false);
  ui->tableWidget->setStyleSheet (stylesheet);
  ui->tableWidget->verticalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->horizontalScrollBar ()->setStyleSheet (QStringLiteral ("background: transparent; background-color:lightgray;"));
  ui->tableWidget->setColumnHidden (1, true);
  ui->tableWidget->setColumnHidden (2, true);
  ui->tableWidget->setColumnHidden (3, true);
  ui->upToolButton->setToolTip (QString (tooltipstyle % QStringLiteral ("Scroll Up")));
  ui->downToolButton->setToolTip (QString (tooltipstyle % QStringLiteral ("Scroll Down")));
  ui->importToolButton->setToolTip (QString (tooltipstyle % QStringLiteral ("Import Template")));
  ui->exportToolButton->setToolTip (QString (tooltipstyle % QStringLiteral ("Export Template")));
  ui->delButton->setToolTip (QString (tooltipstyle % QStringLiteral ("Delete")));

  connect (ui->downToolButton, SIGNAL (clicked ()), this,
           SLOT (downButton_clicked ()));
  connect (ui->upToolButton, SIGNAL (clicked ()), this,
           SLOT (upButton_clicked ()));
  connect (ui->importToolButton, SIGNAL (clicked ()), this,
           SLOT (importButton_clicked ()));
  connect (ui->exportToolButton, SIGNAL (clicked ()), this,
           SLOT (exportButton_clicked ()));
  connect(ui->buttonBox, SIGNAL(accepted ()), this, SLOT(ok_clicked ()));
  connect(ui->buttonBox, SIGNAL(rejected ()), this, SLOT(cancel_clicked ()));
  connect(ui->delButton, SIGNAL(clicked ()), this, SLOT(del_clicked ()));
  connect(ui->tableWidget, SIGNAL(cellClicked (int, int)), this,
          SLOT(cell_clicked (int, int)));
  connect(ui->tableWidget, SIGNAL(cellDoubleClicked (int, int)), this,
          SLOT(ok_clicked ()));

  // correctWidgetFonts (this);
  if (parent != NULL)
    setParent (parent);

  correctTitleBar (this);
}

// destructor
TemplateManagerDialog::~TemplateManagerDialog ()
{
  delete ui;
}

// load
void
TemplateManagerDialog::loadtemplate (void *chart)
{
  CG_ERRORS result;

  ui->saveasLbl->setVisible(false);
  ui->saveasEdit->setVisible(false);
  savemode = false;
  result = loaddir ();
  if (result != CG_ERR_OK)
    showMessage (errorMessage (result), this);

  referencechart = chart;
  show ();
}

// save
void
TemplateManagerDialog::savetemplate (void *chart)
{
  CG_ERRORS result;

  ui->saveasLbl->setVisible(true);
  ui->saveasEdit->setVisible(true);
  ui->saveasEdit->setText("");
  savemode = true;

  result = loaddir ();
  if (result != CG_ERR_OK)
    showMessage (errorMessage (result), this);

  referencechart = chart;
  show ();
}

// set the reference chart
void
TemplateManagerDialog::setReferenceChart (void *chart)
{
  referencechart = chart;
}

// sql from object list
QString
TemplateManagerDialog::qtachart2sql (QString tableKey)
{
  QTAChart *chart = static_cast <QTAChart *> (referencechart);
  QTAChartCore *core = static_cast <QTAChartCore *> (getData (chart));
  ObjectVector Object;
  QString SQLCommand = "", tablename;
  int order = 0;

  if (core->Object.size () < 1)
    return SQLCommand;

  foreach (QTACObject *object, core->Object)
    if (!object->deleteit)
      Object += object;

  SQLCommand.reserve (2048);
  tablename = QStringLiteral ("template_") % tableKey;
  SQLCommand += QStringLiteral ("DELETE FROM ") % tablename % QStringLiteral (";");
  SQLCommand.append ('\n');
  foreach (QTACObject *object, Object)
  {
    DynParamsDialog *paramDialog;
    paramDialog = object->getParamDialog ();

    if (object->dynamic == false)
    {
      if (object->moduleName == QLatin1String (""))
      {
        if (paramDialog != NULL) // technical indicator object
        {
          const ParamVector& pvector = paramDialog->parameters ();
          ParamVector::const_iterator it;

          FOREACH_PARAM(it, pvector)
          {
            const DynParam* param = *it;
            SQLCommand += QStringLiteral ("INSERT INTO ") % tablename;
            SQLCommand += QStringLiteral (" (SERIAL, DYNPARAM, TYPE, TITLE, PARAMNAME, PARAMLABEL,");
            SQLCommand += QStringLiteral ("PARAMTYPE, PARAMDEF, PARAMVALUE) VALUES ");
            SQLCommand += QStringLiteral ("(") % QString::number (order) % QStringLiteral (", 1, ");
            SQLCommand += QString::number (object->type) % QStringLiteral (", ");
            SQLCommand += QStringLiteral ("'") + object->getTitle () % QStringLiteral ("', ");
            SQLCommand += QStringLiteral ("'") + param->paramName % QStringLiteral ("', ");
            SQLCommand += QStringLiteral ("'") + param->label % QStringLiteral ("', ");
            SQLCommand += QString::number (param->type) % QStringLiteral (", ");
            SQLCommand += QString::number (param->defvalue, 'f', 1) % QStringLiteral (", ");
            SQLCommand += QString::number (param->value, 'f', 1) % QStringLiteral (");");
            SQLCommand.append ('\n');
          }
        }
        else // drawing object
        {
          if (object->onlineprice == false)
          {
            qreal x1, x2, y1, y2;
            object->getCoordinates (&x1, &y1, &x2, &y2);

            SQLCommand +=
              QStringLiteral ("INSERT INTO ") % tablename %
              QStringLiteral (" (SERIAL, TYPE, TITLE, COLOR, ") %
              QStringLiteral (" FONTSIZE, FONTWEIGHT, FONT, TEXT, X1, Y1, X2, Y2, ") %
              QStringLiteral ("TRAILERCANDLETEXT, PRICE, PAD, TXTDIRECTION, ") %
              QStringLiteral ("TRAILERCANDLETEXT2, PRICE2, PAD2, TXTDIRECTION2) VALUES ") %
              QStringLiteral ("(") % QString::number (order) % QStringLiteral (", ") %
              QString::number (object->type) % QStringLiteral (", ") %
              QStringLiteral ("'") + object->getTitle () % QStringLiteral ("', ") %
              QString::number (object->getColor().rgb (), 'f', 1) % QStringLiteral (", ") %
              QString::number (object->getFont().pointSize (), 'f', 1) % QStringLiteral (", ") %
              QString::number (object->getFont().weight (), 'f', 1) % QStringLiteral (", ") %
              QStringLiteral ("'") + object->getFont ().family () % QStringLiteral ("', ") %
              QStringLiteral ("'") + object->getText () % QStringLiteral ("', ") %
              QString::number (x1, 'f', 1) % QStringLiteral (", ") %
              QString::number (y1, 'f', 1) % QStringLiteral (", ") %
              QString::number (x2, 'f', 1) % QStringLiteral (", ") %
              QString::number (y2, 'f', 1) % QStringLiteral (", ");

            SQLCommand += QStringLiteral ("'") + object->getTrailerCandleText () % QStringLiteral ("', ");
            if (QString::number (object->getPrice (), 'f', 4).contains (QStringLiteral ("inf")) == true ||
                QString::number (object->getPrice (), 'f', 4).size () > 64)
              SQLCommand += QStringLiteral ("0, ");
            else
              SQLCommand += QString::number (object->getPrice (), 'f', 4) % QStringLiteral (", ");
            SQLCommand += QString::number (object->getPad (), 'f', 4) % QStringLiteral (", ");
            SQLCommand += QString::number (object->getTxtDirection (), 'f', 0) % QStringLiteral (", ");

            SQLCommand += QStringLiteral ("'") + object->getTrailerCandleText2 () % QStringLiteral ("', ");
            if (QString::number (object->getPrice2 (), 'f', 4).contains (QStringLiteral ("inf")) == true ||
                QString::number (object->getPrice2 (), 'f', 4).size () > 64)
              SQLCommand += QStringLiteral ("0, ");
            else
              SQLCommand += QString::number (object->getPrice2 (), 'f', 4) % QStringLiteral (", ");
            SQLCommand += QString::number (object->getPad2 (), 'f', 4) % QStringLiteral (", ");
            SQLCommand += QString::number (object->getTxtDirection2 (), 'f', 0);

            SQLCommand += QStringLiteral ("); ");
            SQLCommand.append ('\n');
          }
        }
      }
    }

    order ++;
  }

  return SQLCommand;
}

// templates' last entry
static int
sqlcb_last (void *dummy, int argc, char **argv, char **column)
{
  QString colname;
  int *newid = (int *) dummy;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("NEWID"))
      *newid = QString::fromUtf8 (argv[counter]).toInt ();
  }
  return 0;
}

// new
CG_ERRORS
TemplateManagerDialog::newtemplate (void)
{
  QString SQLCommand, chartSQL;
  int rc, newid;

  SQLCommand = QStringLiteral ("INSERT INTO TEMPLATES (TEMPLATE) VALUES ('");
  SQLCommand += ui->saveasEdit->text ().trimmed () % QStringLiteral ("');");
  SQLCommand.append ('\n');

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  SQLCommand = QStringLiteral ("SELECT MAX (TEMPLATE_ID) AS NEWID FROM TEMPLATES;");
  SQLCommand.append ('\n');
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_last, static_cast <void *> (&newid));
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  SQLCommand.append ('\n');
  SQLCommand +=  QStringLiteral ("UPDATE TEMPLATES SET TABLENAME = '");
  SQLCommand += QStringLiteral ("template_") % QString::number(newid) % QStringLiteral ("' ");
  SQLCommand += QStringLiteral ("WHERE TEMPLATE_ID = ") % QString::number(newid) % QStringLiteral (";");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("DROP TABLE IF EXISTS template_") % QString::number(newid) % QStringLiteral (";");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("CREATE TABLE template_") % QString::number(newid) % QStringLiteral (" AS ");
  SQLCommand += QStringLiteral ("SELECT * FROM templatemodel;");
  SQLCommand.append ('\n');
  chartSQL = qtachart2sql (QString::number(newid));
  SQLCommand += chartSQL;
  SQLCommand.append ('\n');
  chartSQL.replace ((QStringLiteral ("template_") % QString::number(newid)),
                    "%TABLE$NAME%");
  SQLCommand += QStringLiteral ("UPDATE TEMPLATES SET SQLSTATEMENT = \"");
  SQLCommand += QStringLiteral ("-- 1|1");
  SQLCommand += QStringLiteral ("-- ") % ui->saveasEdit->text ().trimmed ();
  SQLCommand.append ('\n');
  SQLCommand += chartSQL % QStringLiteral ("\" ");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("WHERE TEMPLATE_ID = ") % QString::number(newid) % QStringLiteral (";");

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  return CG_ERR_OK;
}

// update
CG_ERRORS
TemplateManagerDialog::updatetemplate (QString tablename)
{
  QString SQLCommand, key, chartSQL;
  int rc;

  key = tablename;
  key = key.remove (QStringLiteral ("template_"));
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("DROP TABLE IF EXISTS ") % tablename % QStringLiteral (";");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("CREATE TABLE ") % tablename % QStringLiteral (" AS ");
  SQLCommand += QStringLiteral ("SELECT * FROM templatemodel;");
  SQLCommand.append ('\n');
  chartSQL = qtachart2sql (key);
  SQLCommand += chartSQL;
  SQLCommand.append ('\n');
  chartSQL.replace (tablename, QStringLiteral ("%TABLE$NAME%"));
  SQLCommand += QStringLiteral ("UPDATE TEMPLATES SET SQLSTATEMENT = \"");
  SQLCommand += QStringLiteral ("-- 1|1");
  SQLCommand += QStringLiteral ("--") % ui->saveasEdit->text ().trimmed ();
  SQLCommand.append ('\n');
  SQLCommand += chartSQL % QStringLiteral ("\" ");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("WHERE TABLENAME = '") % tablename % QStringLiteral ("';");

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  return CG_ERR_OK;
}

// delete template
CG_ERRORS
TemplateManagerDialog::deletetemplate (int id, QString tablename)
{
  QString SQLCommand;
  int rc;

  SQLCommand = QStringLiteral ("DROP TABLE IF EXISTS ") % tablename % QStringLiteral (";");
  SQLCommand.append ('\n');
  SQLCommand += QStringLiteral ("DELETE FROM TEMPLATES WHERE TEMPLATE_ID = ") % QString::number(id) % QStringLiteral (";");
  SQLCommand.append ('\n');

  rc = updatedb (SQLCommand);
  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }

  return CG_ERR_OK;
}

// get serials of indicators or objects callback
static int
sqlcb_getobjectserials  (void *seriallist, int argc, char **argv, char **column)
{
  QList <int> *serial;
  serial = (QList <int> *) seriallist;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("SERIAL"))
      serial->append (QString::fromUtf8 (argv[counter]).toInt ());
  }
  return 0;
}

// load template callback
static int
sqlcb_loadtemplateindicators (void *dummy, int argc, char **argv, char **column)
{
  QString colname;
  Lindicator *ind = static_cast <Lindicator *> (dummy);
  DynParam *Param = NULL;
  qint32 counter = 0;

  for (counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();
    if (colname == QLatin1String ("PARAMNAME"))
      Param = new DynParam (QString::fromUtf8 (argv[counter]));
  }

  for (counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();

    if (colname == QLatin1String ("TITLE"))
    {
      ind->Title = QString::fromUtf8 (argv[counter]);
      ind->Plist += Param;
    }
    else if (colname == QLatin1String ("PARAMLABEL") && Param != NULL)
    {
      Param->label = QString::fromUtf8 (argv[counter]);
    }
    else if (colname == QLatin1String ("PARAMTYPE") && Param != NULL)
    {
      Param->type = QString::fromUtf8 (argv[counter]).toInt ();
    }
    else if (colname == QLatin1String ("PARAMDEF") && Param != NULL)
    {
      Param->defvalue = QString::fromUtf8 (argv[counter]).toDouble ();
    }
    else if (colname == QLatin1String ("PARAMVALUE") && Param != NULL)
      Param->value = QString::fromUtf8 (argv[counter]).toDouble ();
  }

  return 0;
}

// load template objects
static int
sqlcb_loadtemplateobjects (void *dummy, int argc, char **argv, char **column)
{
  QString colname;
  Lobject *obj = static_cast <Lobject *> (dummy);


  for (qint32 counter = 0; counter < argc; counter ++)
  {
    colname = QString::fromUtf8(column[counter]);
    colname = colname.toUpper ();

    if (colname == QLatin1String ("TITLE"))
      obj->Title = QString::fromUtf8 (argv[counter]);
    else if (colname == QLatin1String ("TYPE"))
      obj->type = QString::fromUtf8 (argv[counter]).toInt ();
    else if (colname == QLatin1String ("COLOR"))
      obj->color = QString::fromUtf8 (argv[counter]).toDouble ();
    else if (colname == QLatin1String ("FONTSIZE"))
      obj->fontsize = QString::fromUtf8 (argv[counter]).toInt ();
    else if (colname == QLatin1String ("FONT"))
      obj->fontfamily = QString::fromUtf8 (argv[counter]);
    else if (colname == QLatin1String ("FONTWEIGHT"))
      obj->fontweight = QString::fromUtf8 (argv[counter]).toInt ();
    else if (colname == QLatin1String ("TEXT"))
      obj->text = QString::fromUtf8 (argv[counter]);
    else if (colname == QLatin1String ("TRAILERCANDLETEXT"))
      obj->trailerCandleText = QString::fromUtf8 (argv[counter]);
    else if (colname == QLatin1String ("PRICE"))
      obj->price = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("PAD"))
      obj->pad = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("TXTDIRECTION"))
      obj->txtdirection = QString::fromUtf8 (argv[counter]).toShort ();
    else if (colname == QLatin1String ("TRAILERCANDLETEXT2"))
      obj->trailerCandleText2 = QString::fromUtf8 (argv[counter]);
    else if (colname == QLatin1String ("PRICE2"))
      obj->price2 = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("PAD2"))
      obj->pad2 = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("TXTDIRECTION2"))
      obj->txtdirection2 = QString::fromUtf8 (argv[counter]).toShort ();
    else if (colname == QLatin1String ("X1"))
      obj->x1 = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("X2"))
      obj->x2 = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("Y1"))
      obj->y1 = QString::fromUtf8 (argv[counter]).toFloat ();
    else if (colname == QLatin1String ("Y2"))
      obj->y2 = QString::fromUtf8 (argv[counter]).toFloat ();
  }

  return 0;
}
// attach
CG_ERRORS
TemplateManagerDialog::attachtemplate (QString tablename)
{
  QList <int> indserial;
  QList <int> objectserial;
  QTAChart *chart = static_cast <QTAChart *> (referencechart);
  QTAChartCore *core = static_cast <QTAChartCore *> (getData (chart));
  QTACObject *object;
  QString SQLCommand;
  int rc;

  // attach technical indicators
  SQLCommand = QStringLiteral ("SELECT DISTINCT SERIAL FROM ") %  tablename % QStringLiteral (" WHERE TYPE > 6;");
  rc = selectfromdb (SQLCommand.toUtf8(), sqlcb_getobjectserials,
                     static_cast <void*> (&indserial));

  if (indserial.size () == 0)
    goto attachtemplate_drawing_objects;

  foreach (object, core->Object)
    if (object->getParamDialog () != NULL)
      object->setForDelete ();

  foreach (const int counter, indserial)
  {
    QString query;
    Lindicator *lind;
    DynParamsDialog *pdialog;

    lind = new Lindicator;
    query  = QStringLiteral ("SELECT TITLE, PARAMNAME, PARAMLABEL, PARAMTYPE, ") %
             QStringLiteral ("PARAMDEF,PARAMVALUE FROM ") % tablename % QStringLiteral (" WHERE ") %
             QStringLiteral ("SERIAL = ") % QString::number (counter);
    rc = selectfromdb (query.toUtf8(),
                       sqlcb_loadtemplateindicators, static_cast <void*> (lind));
    if (rc != SQLITE_OK)
    {
      setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
      return CG_ERR_DBACCESS;
    }

    pdialog = new DynParamsDialog (lind->Plist, lind->Title);
    core->functionScr->addIndicator (pdialog);
    delete lind;
    delete pdialog;
  }

  // attach drawing objects
attachtemplate_drawing_objects:
  SQLCommand = QStringLiteral ("SELECT SERIAL FROM ") %  tablename % QStringLiteral (" WHERE TYPE <= 6;");
  rc = selectfromdb (SQLCommand.toUtf8(), sqlcb_getobjectserials,
                     static_cast <void*> (&objectserial));

  if (objectserial.size () == 0)
    goto attachtemplate_end;

  foreach (object, core->Object)
    if (object->getParamDialog () == NULL && object->onlineprice == false)
      object->setForDelete ();

  foreach (const int counter, objectserial)
  {
    QString query;
    Lobject *lobj;

    lobj = new Lobject;
    query  = QStringLiteral ("SELECT TITLE, TYPE, COLOR, FONTSIZE, ") %
             QStringLiteral ("FONT, FONTWEIGHT, TEXT, TRAILERCANDLETEXT, ") %
             QStringLiteral ("PRICE, PAD, TXTDIRECTION, X1, X2, Y1, Y2, ") %
             QStringLiteral ("TRAILERCANDLETEXT2, PRICE2, PAD2, TXTDIRECTION2 ") %
             QStringLiteral ("FROM ") % tablename % QStringLiteral (" WHERE ") %
             QStringLiteral ("SERIAL = ") % QString::number (counter);
    rc = selectfromdb (query.toUtf8(),
                       sqlcb_loadtemplateobjects, static_cast <void*> (lobj));
    if (rc != SQLITE_OK)
    {
      setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
      return CG_ERR_DBACCESS;
    }

    if (lobj->type == QTACHART_OBJ_LABEL)
    {
      QTACObject *qtaco;
      QGraphicsTextItem *item = new QGraphicsTextItem;
      QFont font;

      item->setDefaultTextColor (lobj->color);
      font.setFamily (lobj->fontfamily);
      font.setWeight (lobj->fontweight);
      font.setPointSize ((lobj->fontsize==-1?7:lobj->fontsize));
      item->setFont (font);
      item->setPlainText (lobj->text);
      core->scene->qtcAddItem (item);
      qtaco = core->addLabel (item, lobj->x1, lobj->y1);
      qtaco->setPrice (lobj->price);
    }

    if (lobj->type == QTACHART_OBJ_TEXT)
    {
      QGraphicsTextItem *item = new QGraphicsTextItem;
      QFont font;

      item->setDefaultTextColor (lobj->color);
      font.setFamily (lobj->fontfamily);
      font.setWeight (lobj->fontweight);
      font.setPointSize ((lobj->fontsize==-1?7:lobj->fontsize));
      item->setFont (font);
      item->setPlainText (lobj->text);
      core->scene->qtcAddItem (item);
      core->addText (item, lobj->trailerCandleText, lobj->price);
    }

    if (lobj->type == QTACHART_OBJ_HLINE)
    {
      QGraphicsLineItem *item = new QGraphicsLineItem;
      QColor color;

      item->setPen (QPen (lobj->color));
      core->scene->qtcAddItem (item);
      core->addHLine (item, lobj->price);
    }

    if (lobj->type == QTACHART_OBJ_VLINE)
    {
      QGraphicsLineItem *item = new QGraphicsLineItem;
      QColor color;

      item->setPen (QPen (lobj->color));
      core->scene->qtcAddItem (item);
      core->addVLine (item, lobj->trailerCandleText);
    }

    if (lobj->type == QTACHART_OBJ_LINE)
    {
      QGraphicsLineItem *item = new QGraphicsLineItem;
      LineEdge e1, e2;
      QColor color;

      item->setPen (QPen (lobj->color));
      e1.price = lobj->price;
      e1.pad = lobj->pad;
      e1.trailerCandleText = lobj->trailerCandleText;
      e1.txtdirection = lobj->txtdirection;
      e1.sequence = 1;
      e2.price = lobj->price2;
      e2.pad = lobj->pad2;
      e2.trailerCandleText = lobj->trailerCandleText2;
      e2.txtdirection = lobj->txtdirection2;
      e2.sequence = 2;
      core->scene->qtcAddItem (item);
      core->addTLine (item, e1, e2);
    }

    if (lobj->type == QTACHART_OBJ_FIBO)
    {
      QGraphicsLineItem *item = new QGraphicsLineItem;
      LineEdge e1, e2;
      QColor color;

      item->setPen (QPen (lobj->color));
      e1.price = lobj->price;
      e1.pad = lobj->pad;
      e1.trailerCandleText = lobj->trailerCandleText;
      e1.txtdirection = lobj->txtdirection;
      e1.sequence = 1;
      e2.price = lobj->price2;
      e2.pad = lobj->pad2;
      e2.trailerCandleText = lobj->trailerCandleText2;
      e2.txtdirection = lobj->txtdirection2;
      e2.sequence = 2;
      core->scene->qtcAddItem (item);
      item->setVisible (false);
      item->setLine (lobj->x1, core->yOnPrice (e1.price),
                     lobj->x1, core->yOnPrice (e2.price));
      core->addFibo (item, e1, e2);
    }

    delete lobj;
  }

attachtemplate_end:
  chart->goBack ();

  return CG_ERR_OK;
}

// load template's directory callback
static int
sqlcb_loaddir (void *dlist, int argc, char **argv, char **column)
{
  QString colname;
  QStringList *dirlist = (QStringList *) dlist;

  for (qint32 counter = 0; counter < argc; counter ++)
  {
    QString colname = QString (column[counter]).toUpper ();
    if (colname == QLatin1String ("TEMPLATE") || colname == QLatin1String ("TEMPLATE_ID") ||
        colname == QLatin1String ("TABLENAME") || colname == QLatin1String ("SQLSTATEMENT"))
      dirlist->append (QString::fromUtf8 (argv[0]));
  }
  return 0;
}

// load templates' directory
CG_ERRORS
TemplateManagerDialog::loaddir (void)
{
  QString SQLCommand;
  int rc;

  cleartable ();
  description.clear ();
  SQLCommand = QStringLiteral ("SELECT TEMPLATE FROM TEMPLATES ORDER BY TEMPLATE_ID;");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_loaddir, static_cast <void *> (&description));

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }
  ui->tableWidget->setRowCount (description.size ());
  fillcolumn (description, 0);

  id.clear ();
  SQLCommand = QStringLiteral ("SELECT TEMPLATE_ID FROM TEMPLATES ORDER BY TEMPLATE_ID;");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_loaddir, static_cast <void *> (&id));

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }
  fillcolumn (id, 1);

  tablename.clear ();
  SQLCommand = QStringLiteral ("SELECT TABLENAME FROM TEMPLATES ORDER BY TEMPLATE_ID;");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_loaddir, static_cast <void *> (&tablename));

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }
  fillcolumn (tablename, 2);

  sqlstatement.clear ();
  SQLCommand = QStringLiteral ("SELECT SQLSTATEMENT FROM TEMPLATES ORDER BY TEMPLATE_ID;");
  rc = selectfromdb (SQLCommand.toUtf8(),
                     sqlcb_loaddir, static_cast <void *> (&sqlstatement));

  if (rc != SQLITE_OK)
  {
    setGlobalError(CG_ERR_DBACCESS, __FILE__, __LINE__);
    return CG_ERR_DBACCESS;
  }
  fillcolumn (sqlstatement, 3);

  ui->tableWidget->viewport()->update();
  return CG_ERR_OK;
}

// clear table
void
TemplateManagerDialog::cleartable ()
{
  int row, nrows, col, ncols = 3;

  nrows = ui->tableWidget->rowCount ();
  for (row = 0; row < nrows; row ++)
    for (col = 0; col < ncols; col ++)
      delete ui->tableWidget->takeItem(row,col);
}

// fill table column
void
TemplateManagerDialog::fillcolumn (QStringList list, int col)
{
  for (qint32 counter = 0; counter < list.size (); counter++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QTableWidgetItem::Type);
    item->setText (list[counter]);
    ui->tableWidget->setItem(counter,col,item);
  }
}

/// Events
///
// resize
void
TemplateManagerDialog::resizeEvent (QResizeEvent * event)
{
  if (event->spontaneous ())
    return;

  ui->buttonBox->move (width () - 350, height () - 50);
  ui->upToolButton->move (width () - 50, 10);
  ui->downToolButton->move (width () - 50, 60);
  ui->importToolButton->move (width () - 50, 110);
  ui->exportToolButton->move (width () - 50, 160);
  ui->delButton->move (width () - 55, 210);
  ui->saveasLbl->move (10, height () - 100);
  ui->saveasEdit->move (110, height () - 100);
  ui->saveasEdit->resize (width () - 120, 32);
  ui->tableWidget->resize (width () - 70, height () - 120);
}

// show
void
TemplateManagerDialog::showEvent (QShowEvent * event)
{
  if (event->spontaneous ())
    return;

  ui->tableWidget->clearSelection ();
}

/// Signals
///
// ok
void
TemplateManagerDialog::ok_clicked ()
{
  QTAChart *chart = static_cast <QTAChart *> (referencechart);
  CG_ERRORS result;

  if (savemode == true)
  {
    QString tablename;
    int maxrow;
    bool exists = false;

    if (ui->saveasEdit->text ().size () == 0)
      return;

    if (showOkCancel("Save template?", this) == false)
      return;

    maxrow = ui->tableWidget->rowCount ();
    for (int row = 0; row < maxrow; row ++)
    {
      if (ui->tableWidget->item (row, 0)->text ().trimmed () ==
          ui->saveasEdit->text ().trimmed ())
      {
        exists = true;
        tablename = ui->tableWidget->item (row, 2)->text ();
      }
    }

    if (exists == false)
    {
      result = newtemplate ();
      if (result != CG_ERR_OK)
      {
        showMessage (errorMessage (result), this);
        return;
      }
    }
    else
    {
      result = updatetemplate (tablename);
      if (result != CG_ERR_OK)
      {
        showMessage (errorMessage (result), this);
        return;
      }
    }

    loaddir ();
    showMessage (QStringLiteral ("Template saved."), this);

    return;
  }

  if (selectedtable == QLatin1String (""))
    return;

  result = attachtemplate (selectedtable);
  if (result != CG_ERR_OK)
  {
    showMessage (errorMessage (result), this);
    return;
  }

  chart->goBack ();
  hide ();
}

// cancel
void
TemplateManagerDialog::cancel_clicked ()
{
  hide ();
}

// del
void
TemplateManagerDialog::del_clicked ()
{
  CG_ERRORS result;
  int maxrow = ui->tableWidget->rowCount ();

  if (selectedtable == QLatin1String (""))
  {
    showMessage ("Select a template first please.", this);
    return;
  }

  if (showOkCancel("Delete selected template?", this) == false)
    goto del_clicked_end;

  for (int row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
    {
      result = deletetemplate (ui->tableWidget->item (row, 1)->text ().toInt (),
                               ui->tableWidget->item (row, 2)->text ());
      if (result != CG_ERR_OK)
      {
        showMessage (errorMessage (result), this);
        return;
      }
    }

del_clicked_end:
  selectedtable = "";
  loaddir ();
}

// template selected
void
TemplateManagerDialog::cell_clicked (int row, int column)
{
  Q_UNUSED (column);
  selectedtable = ui->tableWidget->item (row, 2)->text ();
  ui->saveasEdit->setText (ui->tableWidget->item (row, 0)->text ());
}

// down
void
TemplateManagerDialog::downButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// up
void
TemplateManagerDialog::upButton_clicked (void)
{
  QScrollBar *vScrollBar = ui->tableWidget->verticalScrollBar();
  vScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
  ui->tableWidget->setFocus (Qt::MouseFocusReason);
}

// import
void
TemplateManagerDialog::importButton_clicked (void)
{
  SimpleCrypt crypto(ENCKEY);
  QString filename, inputline, templatename, tablename, SQLCommand;
  QStringList sqlScript, header;
  QFileDialog *fileDialog;
  QTemporaryFile *tempfile;
  int rc, newid;
  CG_ERRORS localError = CG_ERR_OK;

  fileDialog = new QFileDialog;
  correctWidgetFonts (fileDialog);
  filename = fileDialog->getOpenFileName(this, "Import template", filename, "(*.cts)");
  delete fileDialog;

  if (filename == QLatin1String (""))
    return;

  tempfile = new QTemporaryFile;
  if (tempfile->open())
  {
    QFile tmplscript (filename);
    if (tmplscript.open (QIODevice::ReadOnly|QIODevice::Text))
    {
      QString encstr, unencstr;
      QTextStream in (&tmplscript);
      encstr = in.readLine (0);
      unencstr = crypto.decryptToString (encstr);
      tempfile->write(unencstr.toUtf8 ());
      tmplscript.close ();
    }
    else
    {
      localError = CG_ERR_OPEN_FILE;
      goto importButton_clicked_end;
    }
  }
  else
  {
    localError = CG_ERR_OPEN_FILE;
    goto importButton_clicked_end;
  }
  tempfile->close ();

  if (tempfile->open ())
  {
    QTextStream in (tempfile);
    sqlScript += in.readLine (0);
    while (!in.atEnd ())
    {
      sqlScript += in.readLine (0);
    }
    tempfile->close();

    header = sqlScript[0].split ("--", QString::KeepEmptyParts);
    if (header.size () < 2)
    {
      showMessage ("Invalid template script.", this);
      goto importButton_clicked_end;
    }

    if (header[1].trimmed () != "1|1")
    {
      showMessage ("Invalid template script.", this);
      goto importButton_clicked_end;
    }
    templatename = header[2].trimmed ();
    SQLCommand += QStringLiteral ("INSERT INTO TEMPLATES (TEMPLATE) VALUES ('");
    SQLCommand += templatename % QStringLiteral ("');");
    SQLCommand.append ('\n');

    rc = updatedb (SQLCommand);
    if (rc != SQLITE_OK)
    {
      localError = CG_ERR_DBACCESS;
      goto importButton_clicked_end;
    }

    SQLCommand = QStringLiteral ("SELECT MAX (TEMPLATE_ID) AS NEWID FROM TEMPLATES;");
    SQLCommand.append ('\n');
    rc = selectfromdb (SQLCommand.toUtf8(),
                       sqlcb_last, static_cast <void *> (&newid));
    if (rc != SQLITE_OK)
    {
      localError = CG_ERR_DBACCESS;
      goto importButton_clicked_end;
    }
    tablename = QStringLiteral ("template_") % QString::number(newid);

    SQLCommand.append ('\n');
    SQLCommand += QStringLiteral ("DROP TABLE IF EXISTS ") % tablename % QStringLiteral (";");
    SQLCommand.append ('\n');
    SQLCommand += QStringLiteral ("CREATE TABLE ") % tablename % QStringLiteral (" AS ");
    SQLCommand += QStringLiteral ("SELECT * FROM templatemodel;");
    SQLCommand.append ('\n');
    for (qint32 counter = 1; counter < sqlScript.size (); counter ++)
    {
      SQLCommand += sqlScript[counter];
      SQLCommand.append ('\n');
    }
    SQLCommand.replace (QStringLiteral ("%TABLE$NAME%"), tablename);

    SQLCommand += QStringLiteral ("UPDATE TEMPLATES SET SQLSTATEMENT = \"");
    for (qint32 counter = 0; counter < sqlScript.size (); counter ++)
    {
      SQLCommand += sqlScript[counter];
      SQLCommand.append ('\n');
    }
    SQLCommand += QStringLiteral ("\" WHERE TEMPLATE_ID = ") % QString::number(newid) % QStringLiteral (";");
    SQLCommand += QStringLiteral ("UPDATE TEMPLATES SET TABLENAME = '") % tablename % QStringLiteral ("' ");
    SQLCommand += QStringLiteral ("WHERE TEMPLATE_ID = ") % QString::number(newid) % QStringLiteral (";");
    SQLCommand.append ('\n');

    rc = updatedb (SQLCommand);
    if (rc != SQLITE_OK)
    {
      localError = CG_ERR_DBACCESS;
      goto importButton_clicked_end;
    }
    loaddir ();
    showMessage ("Import complete.", this);
  }
  else
    localError = CG_ERR_OPEN_FILE;

importButton_clicked_end:
  delete tempfile;

  if (localError != CG_ERR_OK)
  {
    setGlobalError(localError, __FILE__, __LINE__);
    showMessage (errorMessage (localError), this);
  }

  ui->tableWidget->clearSelection ();
}

// export
void
TemplateManagerDialog::exportButton_clicked (void)
{
  SimpleCrypt crypto(ENCKEY);
  QString filename, encstr;
  QFileDialog *fileDialog;
  int maxrow = ui->tableWidget->rowCount (), selected = -1;

  for (int row = 0; row < maxrow; row ++)
    if (ui->tableWidget->item (row, 0)->isSelected ())
      selected = row;

  if (selected == -1)
  {
    showMessage ("Select a template first please.", this);
    return;
  }

  if (ui->tableWidget->item (selected, 3)->text ().contains (QStringLiteral ("-- 0|0 -- No template script")))
  {
    showMessage ("This is a template created by an old version. Please delete it, recreate it and try again.", this);
    ui->tableWidget->clearSelection ();
    return;
  }

  filename = ui->tableWidget->item (selected, 0)->text ();
  filename.replace (QStringLiteral (" "), QStringLiteral ("_"));
  filename += QStringLiteral (".cts");

  fileDialog = new QFileDialog;
  correctWidgetFonts (fileDialog);
  filename = fileDialog->getSaveFileName(this, "Export template", filename, "(*.cts)");
  delete fileDialog;
  if (filename == QLatin1String (""))
  {
    ui->tableWidget->clearSelection ();
    return;
  }

  QFile tmplscript (filename);
  tmplscript.open(QIODevice::WriteOnly | QIODevice::Text);
  encstr = crypto.encryptToString (ui->tableWidget->item (selected, 3)->text ());
  tmplscript.write(encstr.toUtf8 ());
  tmplscript.close();
  showMessage ("Export complete.", this);
  ui->tableWidget->clearSelection ();
}
