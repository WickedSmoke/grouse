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

// this source includes the constructor, the destructor(s) and
// the event functions of QTAChart
#include <limits>
#include <QEvent>
#include <QKeyEvent>
#include <QShortcut>
#include "defs.h"
#include "ui_qtachart.h"
#include "qtachart_core.h"
#include "qtachart_properties.h"
#include "qtachart_help.h"
#include "qtachart_objects.h"
#include "qtachart_eventfilters.h"
#include "mainwindow.h"
#include "cgscript.h"

// constructor
QTAChart::QTAChart (QWidget * parent):
  QWidget (parent), ui (new Ui::QTAChart)
{
  QTAChartCore *core;
  QPalette palette;
  QFont font;

  classError = CG_ERR_OK;
  // dynamic allocation
  core = new (std::nothrow) QTAChartCore (this);
  if (core == NULL)
    return;

  if (core->classError != 0)
  {
    classError = core->classError;
    delete core;
    return;
  }

  core->init ();
  chartdata = static_cast <void*> (core);
  tabText = QStringLiteral ("Default");

  ui->setupUi (this);

  // initialize
  core->firstshow = true;
  core->textitem = NULL;
  core->hvline = NULL;
  core->events_enabled = true;
  core->tfinit = false;
  core->forecolor = Application_Options->forecolor;
  core->framecolor = core->forecolor;
  core->textcolor = core->forecolor;
  core->backcolor = Application_Options->backcolor;
  core->gridcolor = core->forecolor;
  core->cursorcolor = Qt::yellow;
  core->linecolor = Application_Options->linecolor;
  core->barcolor = Application_Options->barcolor;

  core->titletext[0] = 0;
  core->subtitletext[0] = 0;
  core->framewidth = 10;
  core->linethickness = 3;
  core->barwidth = 1;
  core->points = 0.1;
  core->chartleftmost = 45;
  core->right_border_width = 70;
  core->title_height = 30;
  core->bottomline_height = 25;
  core->chartframe = 10;
  core->nsubcharts = 0;
  core->lineprice = &core->CLOSE;
  core->show_volumes = Application_Options->showvolume;
  core->volumes = NULL;
  core->show_grid = Application_Options->showgrid;
  core->show_onlineprice = Application_Options->showonlineprice;
  core->onlineprice = NULL;
  core->show_ruller = true;
  core->drag = false;
  core->object_drag = false;
  core->reloaded = false;
  core->redraw = true;
  core->recalc = true;
  core->linear = Application_Options->linear;
  core->currenttf = QStringLiteral ("");
  core->always_redraw = true;
  core->chart_style = Application_Options->chartstyle;
  core->height = height ();
  core->width = width ();
  core->chartrightmost = (qint64) (core->width - core->right_border_width);
  core->max_high = std::numeric_limits < qreal >::min ();
  core->min_low = std::numeric_limits < qreal >::max ();
  core->fracdig = 2;
  core->ruller_cursor_x = 200;
  core->ruller_cursor_y = 200;

  core->title->setPlainText ("");
  font = core->title->font ();
  font.setWeight (QFont::Bold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (10 + CHART_FONT_SIZE_PAD);
  core->title->setFont (font);
  core->title->setPos (core->chartleftmost, 1);

  core->gridFont = font;
  core->gridFont.setPixelSize (7 + CHART_FONT_SIZE_PAD);
  core->gridFont.setWeight (QFont::DemiBold);

  core->subtitle->setPlainText ("");
  font.setWeight (QFont::DemiBold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  core->subtitle->setFont (font);
  core->subtitle->setPos (core->chartleftmost, 18);

  core->scaletitle->setPlainText ("");
  font = core->scaletitle->font ();
  font.setWeight (QFont::Bold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (9 + CHART_FONT_SIZE_PAD);
  core->scaletitle->setFont (font);
  core->scaletitle->setDefaultTextColor (core->textcolor);
  core->scaletitle->setPos (core->chartrightmost - 100, 1);

  core->typetitle->setPlainText ("");
  font.setWeight (QFont::DemiBold);
  font.setStyle (QFont::StyleNormal);
  font.setPixelSize (9 + CHART_FONT_SIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  core->typetitle->setFont (font);
  core->typetitle->setDefaultTextColor (core->textcolor);
  core->typetitle->setPos (core->chartrightmost - 100, 18);

  font = core->bottom_text->font ();
  font.setWeight (QFont::DemiBold);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setPixelSize (8 + CHART_FONT_SIZE_PAD);
  core->bottom_text->setFont (font);
  core->bottom_text->setPlainText ("");
  core->bottom_text->setPos (core->chartleftmost, core->height - (core->bottomline_height + 5));
  core->bottom_text->setDefaultTextColor (core->textcolor);

  font = core->ruller_cursor->font ();
  font.setPixelSize (7 + CHART_FONT_SIZE_PAD);
  font.setFamily (DEFAULT_FONT_FAMILY);
  font.setWeight (QFont::DemiBold);
  core->ruller_cursor->setFont (font);
  core->ruller_cursor->setPos (core->ruller_cursor_x, core->ruller_cursor_y);
  core->ruller_cursor->setDefaultTextColor (core->backcolor);
  core->ruller_cursor->setDefaultBackgroundColor (core->forecolor);
  core->ruller_cursor->setZValue (1.0);

  core->topedge->setLine (0, 0, 0, 0);
  core->topedge->setPen (QPen (core->gridcolor));
  core->bottomedge->setLine (0, 0, 0, 0);
  core->bottomedge->setPen (QPen (Qt::darkYellow));
  core->rightedge->setLine (0, 0, 0, 0);
  core->rightedge->setPen (QPen (core->gridcolor));
  core->leftedge->setLine (0, 0, 0, 0);
  core->leftedge->setPen (QPen (core->gridcolor));

  core->chart = this->findChild < QGraphicsView * > ("graphicsView");
  core->chart->setViewportUpdateMode (QGraphicsView::FullViewportUpdate);
  core->chart->setCacheMode (QGraphicsView::CacheBackground);
  core->chart->setAlignment (Qt::AlignLeft | Qt::AlignTop);

  core->chart->setScene (core->scene);
  core->chart->installEventFilter (core->chartEventFilter);
  core->chart->setMouseTracking (true);
  core->chart->viewport()->setMouseTracking(true);

  core->scene->setItemIndexMethod (QTCGraphicsScene::NoIndex);
  core->scene->setBackgroundBrush (core->backcolor);
  core->scene->setBackgroundBrush (Qt::SolidPattern);
  core->scene->addItem (core->title);
  core->scene->addItem (core->subtitle);
  core->scene->addItem (core->scaletitle);
  core->scene->addItem (core->typetitle);
  core->scene->addItem (core->bottom_text);
  core->scene->addItem (core->ruller_cursor);
  core->scene->addItem (core->topedge);
  core->scene->addItem (core->bottomedge);
  core->scene->addItem (core->rightedge);
  core->scene->addItem (core->leftedge);
  core->scene->setObjectName ("graphicsScene");
  core->scene->installEventFilter (core->sceneEventFilter);

  // expand/shrink button
  core->expandicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/fullscreen.png"));
  core->expandBtn->setIcon (core->expandicon);
  core->expandBtn->setIconSize (QSize (32, 32));
  core->expandBtn->setFocusPolicy (Qt::NoFocus);
  core->expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Expand/Restore (Alt+E)</span>"));
  core->prxexpandBtn = core->scene->addWidget (core->expandBtn, Qt::Widget);
  core->prxexpandBtn->setGeometry (QRectF (0, 0, 32, 32));

  // properies button
  core->propicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Gear.png"));
  core->propertiesBtn->setIcon (core->propicon);
  core->propertiesBtn->setIconSize (QSize (32, 32));
  core->propertiesBtn->setFocusPolicy (Qt::NoFocus);
  core->propertiesBtn->setToolTip (TOOLTIP % QStringLiteral ("Preferences (Alt+S)</span>"));
  core->prxpropBtn = core->scene->addWidget (core->propertiesBtn, Qt::Widget);
  core->prxpropBtn->setGeometry (QRectF (0, 45, 32, 32));

  // help button
  core->helpBtn->setText (QStringLiteral ("HELP"));
  core->helpBtn->setStyleSheet
  (QStringLiteral ("background: transparent; color: %1;font: 11px;\
        font-weight: bold;border: 1px solid transparent;\
        border-color: %1;").arg (core->forecolor.name ()));
  core->helpBtn->setFocusPolicy (Qt::NoFocus);
  core->helpBtn->setToolTip (TOOLTIP % QStringLiteral ("Shortcuts (Alt+H)</span>"));
  core->prxhelpBtn = core->scene->addWidget (core->helpBtn, Qt::Widget);
  core->prxhelpBtn->setGeometry (QRectF(core->chartrightmost + 5,
                                        core->height - (core->bottomline_height + 2),
                                        core->right_border_width - 10,
                                        core->bottomline_height - 10));

  // data button
  core->dataicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Blocknotes.png"));
  core->dataBtn->setIcon (core->dataicon);
  core->dataBtn->setIconSize (QSize (32, 32));
  core->dataBtn->setFocusPolicy (Qt::NoFocus);
  core->dataBtn->setToolTip (TOOLTIP % QStringLiteral ("Data (Alt+Y)</span>"));
  core->prxdataBtn = core->scene->addWidget (core->dataBtn, Qt::Widget);
  core->prxdataBtn->setGeometry (QRectF(0, 90, 32, 32));

  // zoom in button
  core->zoominicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Zoom_In.png"));
  core->zoomInBtn->setIcon (core->zoominicon);
  core->zoomInBtn->setIconSize (QSize (32, 32));
  core->zoomInBtn->setFocusPolicy (Qt::NoFocus);
  core->zoomInBtn->setToolTip (TOOLTIP % QStringLiteral ("Zoom In (+)</span>"));
  core->prxzoominBtn = core->scene->addWidget (core->zoomInBtn, Qt::Widget);
  core->prxzoominBtn->setGeometry (QRectF(0, 135, 32, 32));

  // zoom out button
  core->zoomouticon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Zoom_Out.png"));
  core->zoomOutBtn->setIcon (core->zoomouticon);
  core->zoomOutBtn->setIconSize (QSize (32, 32));
  core->zoomOutBtn->setFocusPolicy (Qt::NoFocus);
  core->zoomOutBtn->setToolTip (TOOLTIP % QStringLiteral ("Zoom Out (-)</span>"));
  core->prxzoomoutBtn = core->scene->addWidget (core->zoomOutBtn, Qt::Widget);
  core->prxzoomoutBtn->setGeometry (QRectF(0, 180, 32, 32));

  // function button
  core->functionicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Chart_Graph_Descending.png"));
  core->functionBtn->setIcon (core->functionicon);
  core->functionBtn->setIconSize (QSize (32, 32));
  core->functionBtn->setFocusPolicy (Qt::NoFocus);
  core->functionBtn->setToolTip (TOOLTIP % QStringLiteral ("Indicators (Alt+F)</span>"));
  core->prxfunctionBtn = core->scene->addWidget (core->functionBtn, Qt::Widget);
  core->prxfunctionBtn->setGeometry (QRectF(0, 225, 32, 32));

  // draw button
  core->drawicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/Application_Blueprint.png"));
  core->drawBtn->setIcon (core->drawicon);
  core->drawBtn->setIconSize (QSize (32, 32));
  core->drawBtn->setFocusPolicy (Qt::NoFocus);
  core->drawBtn->setToolTip (TOOLTIP % QStringLiteral ("Draw (Alt+D)</span>"));
  core->prxdrawBtn = core->scene->addWidget (core->drawBtn, Qt::Widget);
  core->prxdrawBtn->setGeometry (QRectF(0, 270, 32, 32));

  // objects button
  core->objectsicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/File_List.png"));
  core->objectsBtn->setIcon (core->objectsicon);
  core->objectsBtn->setIconSize (QSize (32, 32));
  core->objectsBtn->setFocusPolicy (Qt::NoFocus);
  core->objectsBtn->setToolTip (TOOLTIP % QStringLiteral ("Manage Objects (Alt+O)</span>"));
  core->prxobjectsBtn = core->scene->addWidget (core->objectsBtn, Qt::Widget);
  core->prxobjectsBtn->setGeometry (QRectF(0, 315, 32, 32));

  // properties screen
  core->propScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->propScr->setVisible (false);
  core->prxpropScr = core->scene->addWidget (core->propScr, Qt::Widget);
  core->prxpropScr->setPos (0, 40);
  core->prxpropScr->resize (core->width, core->height - 40);
  core->propScr->setVolumes (core->show_volumes);
  core->propScr->setGrid (core->show_grid);
  core->setChartStyle (core->chart_style);
  core->setLinearScale (core->linear);
  core->propScr->setOnlinePrice (core->show_onlineprice);

  // draw screen
  core->drawScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->drawScr->setVisible (false);
  core->prxdrawScr = core->scene->addWidget (core->drawScr, Qt::Widget);
  core->prxdrawScr->setPos (0, 40);
  core->prxdrawScr->resize (core->width, core->height - 40);

  // function screen
  core->functionScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->functionScr->setVisible (false);
  core->prxfunctionScr = core->scene->addWidget (core->functionScr, Qt::Widget);
  core->prxfunctionScr->setPos (0, 40);
  core->prxfunctionScr->resize (core->width, core->height - 40);

  // objects screen
  core->objectsScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->objectsScr->setVisible (false);
  core->prxobjectsScr = core->scene->addWidget (core->objectsScr, Qt::Widget);
  core->prxobjectsScr->setPos (0, 40);
  core->prxobjectsScr->resize (core->width, core->height - 40);

  // help screen
  core->helpScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->helpScr->setVisible (false);
  core->prxhelpScr = core->scene->addWidget (core->helpScr, Qt::Widget);
  core->prxhelpScr->setPos (0, 40);
  core->prxhelpScr->resize (core->width, core->height - 40);

  // data screen
  core->dataScr->setStyleSheet (QStringLiteral ("background:transparent;color:white;"));
  core->dataScr->setVisible (false);
  core->prxdataScr = core->scene->addWidget (core->dataScr, Qt::Widget);
  core->prxdataScr->setPos (0, 40);
  core->prxdataScr->resize (core->width, core->height - 40);

  setFocusProxy (&core->View);

  connect (core->propertiesBtn, SIGNAL (clicked ()), this,
           SLOT (propertiesBtn_clicked ()));

  connect (core->helpBtn, SIGNAL (clicked ()), this,
           SLOT (helpBtn_clicked ()));

  connect (core->dataBtn, SIGNAL (clicked ()), this,
           SLOT (dataBtn_clicked ()));

  connect (core->zoomInBtn, SIGNAL (clicked ()), this,
           SLOT (zoomInBtn_clicked ()));

  connect (core->zoomOutBtn, SIGNAL (clicked ()), this,
           SLOT (zoomOutBtn_clicked ()));

  connect (core->expandBtn, SIGNAL (clicked ()), this,
           SLOT (expandBtn_clicked ()));

  connect (core->drawBtn, SIGNAL (clicked ()), this,
           SLOT (drawBtn_clicked ()));

  connect (core->functionBtn, SIGNAL (clicked ()), this,
           SLOT (functionBtn_clicked ()));

  connect (core->objectsBtn, SIGNAL (clicked ()), this,
           SLOT (objectsBtn_clicked ()));
}

// destructor
QTAChart::~QTAChart ()
{
  if (classError == CG_ERR_NOMEM)
    return;

  const QTAChartCore *core =  static_cast <const QTAChartCore *> (chartdata);

  if (core->VOLUME.size () > 0)
    core->saveSettings ();

  ArrayDestroyAll_imp (this);
  StringDestroyAll_imp (this);

  delete ui;
}

// back button implementation
void
QTAChart::goBack (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->gridstep <= 1)
    return;

  core->expandicon =
    QIcon (QStringLiteral (":/png/images/icons/PNG/fullscreen.png"));
  core->expandBtn->setIcon (core->expandicon);
  core->expandBtn->setToolTip (TOOLTIP % QStringLiteral ("Expand/Restore (Alt+E)</span>"));
  core->events_enabled = true;
  core->showAllItems ();

  core->prxfunctionScr->setVisible (false);
  core->prxpropScr->setVisible (false);
  core->prxhelpScr->setVisible (false);
  core->prxdataScr->setVisible (false);
  core->prxdrawScr->setVisible (false);
  core->prxobjectsScr->setVisible (false);

  core->show_volumes = core->propScr->Volumes ();
  core->show_grid = core->propScr->Grid ();
  core->setChartStyle (core->propScr->ChartStyle ());
  core->linecolor = core->propScr->lineColor ();
  core->barcolor = core->propScr->barColor ();
  core->setLinearScale (core->propScr->LinearScale ());
  core->forecolor = core->propScr->foreColor ();
  core->backcolor = core->propScr->backColor ();
  core->show_onlineprice = core->propScr->OnlinePrice ();
  core->changeForeColor (core->forecolor);
  core->scene->setBackgroundBrush (core->backcolor);
  core->ruller_cursor->setDefaultTextColor (core->backcolor);
  core->ruller_cursor->setDefaultBackgroundColor (core->forecolor);

  QString btnStyleSheet =
    QString ("background: transparent; color: %1; \
            border: 1px solid transparent;  border-color: %1;").arg (core->backcolor.name ());

  core->expandBtn->setStyleSheet (btnStyleSheet);
  core->propertiesBtn->setStyleSheet (btnStyleSheet);
  core->dataBtn->setStyleSheet (btnStyleSheet);
  core->zoomInBtn->setStyleSheet (btnStyleSheet);
  core->zoomOutBtn->setStyleSheet (btnStyleSheet);
  core->functionBtn->setStyleSheet (btnStyleSheet);
  core->drawBtn->setStyleSheet (btnStyleSheet);
  core->objectsBtn->setStyleSheet (btnStyleSheet);
  core->helpBtn->setStyleSheet
  (QString ("background: transparent; color: %1;font: 11px;\
        font-weight: bold;border: none;").arg (core->forecolor.name ()));

  if (core->propScr->Volumes ())
    core->drawVolumes ();
  else
    core->deleteVolumes ();

  core->deleteITEMS ();
  core->draw ();
}

/// Events
///
// resize
void
QTAChart::resizeEvent (QResizeEvent * event)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (event->oldSize () == event->size ())
    return;

  core->setSizeChanged ();
  core->height = height ();
  core->width = width ();

  core->chartrightmost = (qint64) (core->width - core->right_border_width);
  core->title->setPos (core->chartleftmost, 1);
  core->subtitle->setPos (core->chartleftmost, 18);
  core->scaletitle->setPos (core->chartrightmost - 100, 1);
  core->typetitle->setPos (core->chartrightmost - 100, 18);
  core->prxhelpBtn->setPos (core->chartrightmost + 5,
                            core->height - (core->bottomline_height + 2));
  core->chart->resize (core->width, core->height);
  core->scene->setSceneRect (0, 0, core->width - 5, core->height - 5);
  if (core->events_enabled == true)
  {
    core->draw ();
    return;
  }

  core->prxpropScr->resize (core->width, core->height - 40);
  core->prxhelpScr->resize (core->width, core->height - 40);
  core->prxdataScr->resize (core->width, core->height - 40);
  core->prxdrawScr->resize (core->width, core->height - 40);
  core->prxobjectsScr->resize (core->width, core->height - 40);
  core->prxfunctionScr->resize (core->width, core->height - 40);
}

// show event
void
QTAChart::showEvent (QShowEvent * event)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (event->spontaneous ())
    return;

  if (core->firstshow == true)
  {
    TemplateManagerDialog *tmpldlg = new TemplateManagerDialog;
    tmpldlg->setReferenceChart (static_cast <void*> (this));
    tmpldlg->attachtemplate (QStringLiteral ("template_") % core->SymbolKey);
    delete tmpldlg;
    core->firstshow = false;
  }
}

// keypress (+,-, Alt + S)
void
QTAChart::keyPressEvent (QKeyEvent * event)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  // Alt
  if (event->modifiers () & Qt::AltModifier)
  {
    // expand (Alt + E)
    if (event->key () == Qt::Key_E && core->events_enabled == true)
    {
      expandBtn_clicked ();
      return;
    }
    else
    // properties (Alt + S)
    if (event->key () == Qt::Key_S && core->events_enabled == true)
    {
      propertiesBtn_clicked ();
      return;
    }
    else
    // help (Alt + H)
    if (event->key () == Qt::Key_H && core->events_enabled == true)
    {
      helpBtn_clicked ();
      return;
    }
    else
    // data (Alt + Y)
    if (event->key () == Qt::Key_Y && core->events_enabled == true)
    {
      dataBtn_clicked ();
      return;
    }
    else
    // indicators (Alt + F)
    if (event->key () == Qt::Key_F && core->events_enabled == true)
    {
      functionBtn_clicked ();
      return;
    }
    else
    // draw (Alt + D)
    if (event->key () == Qt::Key_D && core->events_enabled == true)
    {
      drawBtn_clicked ();
      return;
    }
    else
    // objects (Alt + O)
    if (event->key () == Qt::Key_O && core->events_enabled == true)
    {
      objectsBtn_clicked ();
      return;
    }
    else
    // back (Alt + Z)
    if (event->key () == Qt::Key_Z)
    {
      backBtn_clicked ();
      return;
    }
    else if (!core->events_enabled)
    {
      return;
    }
    else
    // line chart (Alt + L)
    if (event->key () == Qt::Key_L)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_LINE);
      goto EventEndLbl;
    }
    else
    // candle chart (Alt + C)
    if (event->key () == Qt::Key_C)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_CANDLE);
      goto EventEndLbl;
    }
    else
    // heikin-ashi chart (Alt + A)
    if (event->key () == Qt::Key_A)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_HEIKINASHI);
      goto EventEndLbl;
    }
    else
    // bar chart (Alt + B)
    if (event->key () == Qt::Key_B)
    {
      core->deleteITEMS ();
      core->setChartStyle (QTACHART_BAR);
      goto EventEndLbl;
    }
    else
    // volumes on/off (Alt + V)
    if (event->key () == Qt::Key_V)
    {
      if (core->propScr->Volumes ())
        core->deleteVolumes ();
      else
        core->drawVolumes ();
      goto EventEndLbl;
    }
    else
    // grid on/off (Alt + G)
    if (event->key () == Qt::Key_G)
    {
      if (core->propScr->Grid ())
        core->propScr->setGrid (false);
      else
        core->propScr->setGrid (true);
      core->show_grid = core->propScr->Grid ();
      goto EventEndLbl;
    }
    else
    // online price on/off (Alt + P)
    if (event->key () == Qt::Key_P)
    {
      if (core->propScr->OnlinePrice ())
        core->propScr->setOnlinePrice (false);
      else
        core->propScr->setOnlinePrice (true);

      core->show_onlineprice = core->propScr->OnlinePrice ();
      goto EventEndLbl;
    }
    else
    // linear price scale on/off (Alt + X)
    if (event->key () == Qt::Key_X)
    {
      if (core->propScr->LinearScale ())
        core->setLinearScale (false);
      else
        core->setLinearScale (true);

      goto EventEndLbl;
    }
    else
      return;
  }

  if (!core->events_enabled)
    return;

  // plus
  if (event->key () == Qt::Key_Plus)
  {
    if (core->framewidth < 25)
      core->framewidth++;
    else
      return;
  }
  else
  // minus
  if (event->key () == Qt::Key_Minus)
  {
    if (core->framewidth > 3)
      core->framewidth--;
    else
      return;
  }
  else
    return;

EventEndLbl:
  core->draw ();
}

void
QTAChart::backBtn_clicked (void)
{
  goBack ();
}

// properties button
void
QTAChart::propertiesBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  core->setChartProperties ();
}

// expand button
void
QTAChart::expandBtn_clicked (void)
{
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);
  MainWindow *mainwindow;

  if (core->object_drag)
    return;

  mainwindow = (qobject_cast <MainWindow *> (this->parentWidget ()->parentWidget ()->
                parentWidget ()->parentWidget ()));

  if (core->events_enabled == true)
  {
    if (mainwindow->expandedChart ())
      mainwindow->setExpandChart (false);
    else
      mainwindow->setExpandChart (true);
    return;
  }
  else
    backBtn_clicked ();

}

// help button
void
QTAChart::helpBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  if (core->events_enabled == true)
  {
    core->hideAllItems ();
    core->showHelp ();
    core->events_enabled = false;
    return;
  }
}

// data button
void
QTAChart::dataBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  if (core->events_enabled == true)
  {
    core->hideAllItems ();
    core->showData ();
    core->events_enabled = false;
    return;
  }
}

// zoom in button
void
QTAChart::zoomInBtn_clicked (void)
{
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);

  if (core->object_drag)
    return;

  QKeyEvent event = QKeyEvent(QEvent::KeyPress, Qt::Key_Plus, Qt::NoModifier,
                              QStringLiteral ("+"), false, 1);
  keyPressEvent (&event);
}

// zoom out button
void
QTAChart::zoomOutBtn_clicked (void)
{
  const QTAChartCore *core = static_cast <const QTAChartCore *> (chartdata);

  if (core->object_drag)
    return;

  QKeyEvent event = QKeyEvent(QEvent::KeyPress, Qt::Key_Minus, Qt::NoModifier,
                              QStringLiteral ("-"), false, 1);
  keyPressEvent (&event);
}

void
QTAChart::drawBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  core->selectDrawObject ();
}

// function button
void
QTAChart::functionBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  core->selectFunction ();
}

// objects button
void
QTAChart::objectsBtn_clicked (void)
{
  QTAChartCore *core = static_cast <QTAChartCore *> (const_cast <void *> (chartdata));

  if (core->object_drag)
    return;

  core->manageObjects ();
}
