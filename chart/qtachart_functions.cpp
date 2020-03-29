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
 
// Full implementation of QTACFunctions
#include "common.h"
#include "qtachart_core.h"
#include "ui_qtacfunctions.h"
#include "qtachart_functions.h"

// constructor
QTACFunctions::QTACFunctions (QWidget * parent):
  QWidget (parent), ui (new Ui::QTACFunctions)
{
  ui->setupUi (this);

  button_width = 200;
  button_height = 40;

  layout = new QGridLayout(this);
  this->setLayout(layout);

  if (parent != NULL)
    setParent (parent);
}

static bool
ButtonCmp(const QPushButton *b1, const QPushButton *b2)
{
  return b1->text() < b2->text();
}

void
QTACFunctions::createButtons (void)
{
  QPushButton *btn;
  appColorDialog *colorDialog;
  DynParamsDialog *ParamDialog;

  btn = addButton (QStringLiteral ("SMA"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("EMA"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("MACD"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 9.0);
  ParamDialog->addParam (QStringLiteral ("MACD color"), QStringLiteral ("MACD color"), DPT_COLOR, (qreal) QColor (Qt::yellow).rgb ());
  ParamDialog->addParam (QStringLiteral ("Signal color"), QStringLiteral ("Signal color"), DPT_COLOR, (qreal) QColor (Qt::red).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("MFI"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 80);
  ParamDialog->addParam (QStringLiteral ("Medium level"), QStringLiteral ("Medium level"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, 20);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("ROC"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 10.0);
  ParamDialog->addParam (QStringLiteral ("Level"), QStringLiteral ("Level"), DPT_INT, 0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("RSI"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 70);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, 30);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Slow Stoch"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 5.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 80);
  ParamDialog->addParam (QStringLiteral ("Medium level"), QStringLiteral ("Medium level"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, 20);
  ParamDialog->addParam (QStringLiteral ("%K color"), QStringLiteral ("%K Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->addParam (QStringLiteral ("%D color"), QStringLiteral ("%D Color"), DPT_COLOR, (qreal) QColor (Qt::yellow).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Fast Stoch"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 5.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 80);
  ParamDialog->addParam (QStringLiteral ("Medium level"), QStringLiteral ("Medium level"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, 20);
  ParamDialog->addParam (QStringLiteral ("%K color"), QStringLiteral ("%K Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->addParam (QStringLiteral ("%D color"), QStringLiteral ("%D Color"), DPT_COLOR, (qreal) QColor (Qt::yellow).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("W%R"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, -20);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, -80);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Bollinger Bands"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 20.0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::magenta).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Parabolic SAR"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("ADX"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 13.0);
  ParamDialog->addParam (QStringLiteral ("Weak"), QStringLiteral ("Weak"), DPT_INT, 25);
  ParamDialog->addParam (QStringLiteral ("Strong"), QStringLiteral ("Strong"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Very strong"), QStringLiteral ("Very strong"), DPT_INT, 75);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Aroon"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 25.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 70);
  ParamDialog->addParam (QStringLiteral ("Medium level"), QStringLiteral ("Medium level"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, 30);
  ParamDialog->addParam (QStringLiteral ("Up color"), QStringLiteral ("Up Color"), DPT_COLOR, (qreal) QColor (Qt::green).rgb ());
  ParamDialog->addParam (QStringLiteral ("Down color"), QStringLiteral ("Down Color"), DPT_COLOR, (qreal) QColor (Qt::red).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("CCI"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 20.0);
  ParamDialog->addParam (QStringLiteral ("High level"), QStringLiteral ("High level"), DPT_INT, 100);
  ParamDialog->addParam (QStringLiteral ("Low level"), QStringLiteral ("Low level"), DPT_INT, -100);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("STDDEV"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 10.0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("Momentum"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 12.0);
  ParamDialog->addParam (QStringLiteral ("Level"), QStringLiteral ("Level"), DPT_INT, 0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("DMI"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 13.0);
  ParamDialog->addParam (QStringLiteral ("Weak"), QStringLiteral ("Weak"), DPT_INT, 25);
  ParamDialog->addParam (QStringLiteral ("Strong"), QStringLiteral ("Strong"), DPT_INT, 50);
  ParamDialog->addParam (QStringLiteral ("Very strong"), QStringLiteral ("Very strong"), DPT_INT, 75);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  btn = addButton (QStringLiteral ("ATR"));
  connect (btn, SIGNAL (clicked ()), this, SLOT (button_clicked ()));
  ParamDialog = new DynParamsDialog (QStringLiteral (""), btn);
  colorDialog = new appColorDialog;
  ParamDialog->setColorDialog (colorDialog);
  ParamDialog->addParam (QStringLiteral ("Period"), QStringLiteral ("Period"), DPT_INT, 14.0);
  ParamDialog->addParam (QStringLiteral ("Color"), QStringLiteral ("Color"), DPT_COLOR, (qreal) QColor (Qt::cyan).rgb ());
  ParamDialog->setObjectName (QStringLiteral ("ParamDialog"));
  connect(ParamDialog->buttonBox, SIGNAL(accepted ()), this, SLOT(function_accepted()));
  connect(ParamDialog->buttonBox, SIGNAL(rejected ()), this, SLOT(function_rejected()));

  // sort buttons
  qSort(Button.begin(), Button.end(), ButtonCmp);

  // correct fonts
  correctWidgetFonts (this);
}

// destructor
QTACFunctions::~QTACFunctions ()
{

  delete ui;
}

// add a button
QPushButton *
QTACFunctions::addButton (QString text)
{
  QFont fnt;
  QPushButton *btn;
  QString stylesheet;

  stylesheet =
    QStringLiteral ("background: transparent; border: 1px solid transparent;border-color: darkgray;");

  btn = new QPushButton (text, this);
  btn->setFixedSize (QSize (button_width, button_height));
  fnt = btn->font ();
  fnt.setPixelSize (16);
  fnt.setBold (true);
  btn->setFont (fnt);
  btn->setStyleSheet (stylesheet);
  btn->setAutoFillBackground (false);
  btn->setFocusPolicy (Qt::NoFocus);
  btn->setObjectName ("Indicator Button");
  Button += btn;
  return btn;
}

// set the reference chart
void
QTACFunctions::setReferenceChart (void *chart)
{
  referencechart = static_cast <QTAChart *> (chart);
  createButtons ();
}

// get the reference chart
void *
QTACFunctions::getReferenceChart (void)
{
  return referencechart;
}

// add indicator
void
QTACFunctions::addIndicator (DynParamsDialog *paramDialog)
{
  QTAChartCore *core = getData (referencechart);
  QTACObject *indicator = NULL, *childobj;
  QString fname;
  qint32 period;
  QColor color;

  if (core->CLOSE.size () == 0)
    return;

  fname = paramDialog->getTitle ();

  if (fname == QLatin1String ("SMA"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), SMA, 0, 0, color, QStringLiteral ("Color"));
    indicator->setTitle (fname);
  }

  if (fname == QLatin1String ("EMA"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), EMA, 0, 0, color, QStringLiteral ("Color"));
    indicator->setTitle (fname);
  }

  if (fname == QLatin1String ("Parabolic SAR"))
  {
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_DOT);
    indicator->setAttributes (QTACHART_CLOSE, 1, QStringLiteral (""), PSAR, 0, 0, color, QStringLiteral ("Color"));
    indicator->setTitle (fname);
  }

  if (fname == QLatin1String ("RSI"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), RSI, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("MFI"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MFI, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Medium level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("ROC"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ROC, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ROC, QREAL_MIN, QREAL_MAX, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Level"), DUMMY, 0, 0, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("W%R"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, -100, 0, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), WILLR, -100, 0, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, -100, 0, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, -100, 0, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("Slow Stoch"))
  {
    QColor colorD, colorK;
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    colorD = paramDialog->getParam (QStringLiteral ("%D color"));
    colorK = paramDialog->getParam (QStringLiteral ("%K color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, colorD, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHSLOWD, 0, 100, colorD, "%D color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHSLOWK, 0, 100, colorK, "%K color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Medium level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
  }

  if (fname == QLatin1String ("Fast Stoch"))
  {
    QColor colorD, colorK;
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    colorD = paramDialog->getParam (QStringLiteral ("%D color"));
    colorK = paramDialog->getParam (QStringLiteral ("%K color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, colorD, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHFASTD, 0, 100, colorD, "%D color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STOCHFASTK, 0, 100, colorK, "%K color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Medium level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, colorK, QStringLiteral (""));
  }

  if (fname == QLatin1String ("MACD"))
  {
    QColor colorMACD, colorSignal;
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;

    colorMACD = paramDialog->getParam (QStringLiteral ("MACD color"));
    colorSignal = paramDialog->getParam (QStringLiteral ("Signal color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MACD, QREAL_MIN, QREAL_MAX, QColor (Qt::white).rgb (), QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MACD, QREAL_MIN, QREAL_MAX, colorMACD, QStringLiteral ("MACD color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MACDSIGNAL, QREAL_MIN, QREAL_MAX, colorSignal, QStringLiteral ("Signal color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_VBARS);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MACDHIST, QREAL_MIN, QREAL_MAX, QColor (Qt::white).rgb (), QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, 0);
    childobj->setAttributes (QTACHART_CLOSE, 0, QStringLiteral (""), DUMMY, 0, 0, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("Bollinger Bands"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_CURVE);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSMIDDLE, 0, 0, color, QStringLiteral ("Color"));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSUPPER, 0, 0, color, QStringLiteral ("Color"));
    // childobj->setParamDialog (paramDialog->getPVector (), fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), BBANDSLOWER, 0, 0, color, QStringLiteral ("Color"));
    // childobj->setParamDialog (paramDialog->getPVector (), fname);
  }

  if (fname == QLatin1String ("ADX"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ADX, 0, 100, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ADX, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Weak")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Weak"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("Strong"));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Strong"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("Very strong"));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Very strong"), DUMMY, 0, 100, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("Aroon"))
  {
    QColor colorUp, colorDown;
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    colorUp = paramDialog->getParam (QStringLiteral ("Up color"));
    colorDown = paramDialog->getParam ("Down color");
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, 100, colorDown, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), AROONDOWN, 0, 100, colorDown, "Down color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), AROONUP, 0, 100, colorUp, "Up color");
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, 0, 100, colorUp, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, 0, 100, colorUp, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Medium level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Medium level"), DUMMY, 0, 100, colorUp, QStringLiteral (""));
  }

  if (fname == QLatin1String ("CCI"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), CCI, QREAL_MIN, QREAL_MAX, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("High level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("High level"), DUMMY, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Low level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Low level"), DUMMY, QREAL_MIN, QREAL_MAX,
                             color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("STDDEV"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DUMMY, 0, QREAL_MAX, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), STDDEV, 0, QREAL_MAX, color, QStringLiteral ("Color"));
  }

  if (fname == QLatin1String ("Momentum"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MOMENTUM, QREAL_MIN, QREAL_MAX, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), MOMENTUM, QREAL_MIN, QREAL_MAX, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Level")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Level"), DUMMY, 0, 0, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("DMI"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DMX, 0, 100, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), DMX, 0, 100, color, QStringLiteral ("Color"));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam (QStringLiteral ("Weak")));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Weak"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("Strong"));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Strong"), DUMMY, 0, 100, color, QStringLiteral (""));
    childobj = new QTACObject (indicator, QTACHART_OBJ_HLINE);
    childobj->setHLine (NULL, paramDialog->getParam ("Very strong"));
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Very strong"), DUMMY, 0, 100, color, QStringLiteral (""));
  }

  if (fname == QLatin1String ("ATR"))
  {
    period = (qint32) paramDialog->getParam (QStringLiteral ("Period"));
    if (period < 1)
      return;
    color = paramDialog->getParam (QStringLiteral ("Color"));
    indicator = new QTACObject (core, QTACHART_OBJ_SUBCHART);
    indicator->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ATR, 0, QREAL_MAX, color, QStringLiteral (""));
    indicator->setTitle (fname);
    childobj = new QTACObject (indicator, QTACHART_OBJ_CURVE);
    childobj->setAttributes (QTACHART_CLOSE, period, QStringLiteral ("Period"), ATR, 0, QREAL_MAX, color, QStringLiteral ("Color"));
  }

  if (indicator != NULL)
    indicator->setParamDialog (paramDialog->getPVector (), fname);
}

// resize
void
QTACFunctions::resizeEvent (QResizeEvent * event)
{
  QSize newsize;
  int w, h, counter;

  if (event->oldSize () == event->size ())
    return;

  newsize = event->size ();
  w = newsize.width () - 2;
  h = newsize.height () - 2;
  layout->setGeometry (QRect (0, 0, w, h));
  for (counter = 0; counter < Button.size (); counter ++)
  {
    DynParamsDialog *paramDialog;
    layout->setRowMinimumHeight (counter % 6, Button[counter]->height ());
    layout->setColumnMinimumWidth (counter/6, Button[counter]->width ());
    layout->addWidget (Button[counter], counter % 6, counter / 6, Qt::AlignHCenter);
    paramDialog = Button[counter]->findChild<DynParamsDialog *> (QStringLiteral ("ParamDialog"));
    if (paramDialog != NULL)
    {
      paramDialog->move ((width () - paramDialog->width ()) / 2, 25);
    }
  }
}

// button clicked
void
QTACFunctions::button_clicked (void)
{
  DynParamsDialog *paramDialog;
  QPushButton *btn;
  btn = qobject_cast <QPushButton *> (QObject::sender());
  paramDialog = btn->findChild<DynParamsDialog *> (QStringLiteral ("ParamDialog"));
  if (paramDialog != NULL)
  {
    paramDialog->setReferenceChart (referencechart);
    paramDialog->move ((width () - paramDialog->width ()) / 2, 25);
    // paramDialog->exec ();
    paramDialog->setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    paramDialog->open ();
    qApp->processEvents(QEventLoop::AllEvents, 10);
  }
}

// function accepted
void
QTACFunctions::function_accepted (void)
{
  QTAChart *chart = static_cast <QTAChart *> (referencechart);
  DynParamsDialog *paramDialog;

  paramDialog = qobject_cast <DynParamsDialog *> (QObject::sender()->parent ());
  addIndicator (paramDialog);

  chart->goBack ();
}

// function rejected
void
QTACFunctions::function_rejected (void)
{

}

