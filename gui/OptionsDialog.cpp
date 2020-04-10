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

#include <QBoxLayout>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolButton>
#include "OptionsDialog.h"
#include "defs.h"       // Application_Options
#include "chartapp.h"   // showMessage
#include "qtachart.h"
#include "netservice.h"


#define gPref   Application_Options
#define SET_CHECK(cb,opt)  cb->setCheckState(opt?Qt::Checked : Qt::Unchecked)
#define COLOR_CONNECT(oc) \
    connect(oc, SIGNAL(clicked(bool)), SLOT(colorClicked()))


class OptionColor : public QToolButton
{
public:
    OptionColor( const QColor& color ) : _pix(24, 24)
    {
        setColor( color );
    }

    const QColor& color() const { return _col; }

    void setColor( const QColor& color )
    {
        _pix.fill( color );
        QIcon icon( _pix );
        setIcon(icon);
        _col = color;
    }

private:
    QPixmap _pix;
    QColor  _col;
};


ChartPropertiesWidget::ChartPropertiesWidget(QWidget *parent) :
    QWidget(parent), _colorDialog(nullptr), _lastColorClicked(nullptr)
{
    QBoxLayout* lo2;
    QGridLayout* grid = new QGridLayout( this );

    QGroupBox* group = new QGroupBox("Chart Type");
    grid->addWidget(group, 0, 0);

        QGridLayout* g2 = new QGridLayout(group);
        _lineChart   = new QRadioButton("Line");
        _candleChart = new QRadioButton("Candle");
        _heikinChart = new QRadioButton("Heikin-Ashi");
        _barChart    = new QRadioButton("Bar");
        g2->addWidget( _lineChart,   0, 1 );
        g2->addWidget( _candleChart, 1, 1 );
        g2->addWidget( _heikinChart, 2, 1 );
        g2->addWidget( _barChart,    3, 1 );

        _lineColor = new OptionColor( gPref->chart.lineColor );
        COLOR_CONNECT( _lineColor );
        g2->addWidget( _lineColor, 0, 0 );
        _barColor = new OptionColor( gPref->chart.barColor );
        COLOR_CONNECT( _barColor );
        g2->addWidget( _barColor, 3, 0 );

        g2->setRowStretch(4, 1);

        setChartStyle( gPref->chart.style );

    lo2 = new QVBoxLayout;
    grid->addLayout(lo2, 0, 1);

        _showGrid = new QCheckBox("Show Grid");
        SET_CHECK( _showGrid, gPref->chart.showGrid );
        lo2->addWidget(_showGrid);
        _showVolumes = new QCheckBox("Show Volumes");
        SET_CHECK( _showVolumes, gPref->chart.showVolume );
        lo2->addWidget(_showVolumes);
        _linearScale = new QCheckBox("Use Linear Scale");
        SET_CHECK( _linearScale, gPref->chart.linearScale );
        lo2->addWidget(_linearScale);
        _onlinePrice = new QCheckBox("Show Online Price");
        SET_CHECK( _onlinePrice, gPref->chart.showOnlinePrice );
        lo2->addWidget(_onlinePrice);

        lo2->addStretch();

        g2 = new QGridLayout;
        lo2->addLayout(g2);

            _fgColor = new OptionColor( gPref->chart.foreColor );
            COLOR_CONNECT( _fgColor );
            g2->addWidget( _fgColor, 0, 0 );
            g2->addWidget( new QLabel("Foreground Color"), 0, 1 );
            _bgColor = new OptionColor( gPref->chart.backColor );
            COLOR_CONNECT( _bgColor );
            g2->addWidget( _bgColor, 1, 0 );
            g2->addWidget( new QLabel("Background Color"), 1, 1 );
}


void ChartPropertiesWidget::properties( QTAChartProperties& pr ) const
{
    pr.style        = chartStyle();
    pr.showGrid     = _showGrid->isChecked();
    pr.showVolume   = _showVolumes->isChecked();
    pr.linearScale  = _linearScale->isChecked();
    pr.showOnlinePrice = _onlinePrice->isChecked();

    pr.foreColor = _fgColor->color().rgb();
    pr.backColor = _bgColor->color().rgb();
    pr.barColor  = _barColor->color().rgb();
    pr.lineColor = _lineColor->color().rgb();
}


void ChartPropertiesWidget::setProperties( const QTAChartProperties& pr )
{
    setChartStyle( pr.style );
    _showGrid   ->setChecked(pr.showGrid);
    _showVolumes->setChecked(pr.showVolume);
    _linearScale->setChecked(pr.linearScale);
    _onlinePrice->setChecked(pr.showOnlinePrice);

    _fgColor  ->setColor( QRgb(pr.foreColor) );
    _bgColor  ->setColor( QRgb(pr.backColor) );
    _lineColor->setColor( QRgb(pr.lineColor) );
    _barColor ->setColor( QRgb(pr.barColor) );
}


void ChartPropertiesWidget::colorClicked()
{
    if( ! _colorDialog )
    {
        _colorDialog = new QColorDialog(this);
        if( ! _colorDialog )
            return;
        _colorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
        connect( _colorDialog, SIGNAL(colorSelected(const QColor&)),
                 SLOT(colorSel(const QColor&)) );
    }
    _lastColorClicked = static_cast<OptionColor*>( sender() );
    _colorDialog->setCurrentColor( _lastColorClicked->color() );
    _colorDialog->open();
}


void ChartPropertiesWidget::colorSel(const QColor& col)
{
    if( _lastColorClicked )
        _lastColorClicked->setColor(col);
}


int ChartPropertiesWidget::chartStyle() const
{
    if( _lineChart->isChecked() )
        return QTACHART_LINE;
    if( _candleChart->isChecked() )
        return QTACHART_CANDLE;
    if( _heikinChart->isChecked() )
        return QTACHART_HEIKINASHI;
    return QTACHART_BAR;
}


void ChartPropertiesWidget::setChartStyle( int style )
{
    switch( style )
    {
        case QTACHART_LINE:
            _lineChart->setChecked(true);
            break;
        case QTACHART_CANDLE:
            _candleChart->setChecked(true);
            break;
        case QTACHART_HEIKINASHI:
            _heikinChart->setChecked(true);
            break;
        case QTACHART_BAR:
            _barChart->setChecked(true);
            break;
    }
}


//----------------------------------------------------------------------------


ChartPropertiesDialog::ChartPropertiesDialog(QWidget *parent ) :
    QDialog(parent), _chart(nullptr)
{
    setWindowTitle( "Chart Properties" );
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setModal(true);

    QBoxLayout* lo = new QVBoxLayout( this );

    _prop = new ChartPropertiesWidget;
    lo->addWidget( _prop );

    lo->addSpacing( 8 );

    QDialogButtonBox* bbox = new QDialogButtonBox;
    bbox->setStandardButtons( QDialogButtonBox::Save |
                              QDialogButtonBox::Cancel );
    connect( bbox, SIGNAL(accepted()), this, SLOT(acceptProperties()));
    connect( bbox, SIGNAL(rejected()), this, SLOT(reject()));
    lo->addWidget( bbox );
}


void ChartPropertiesDialog::setChart( QTAChart* chartToEdit )
{
    _chart = chartToEdit;

    QTAChartProperties tp;
    _chart->properties( tp );
    _prop->setProperties( tp );
}


void ChartPropertiesDialog::acceptProperties()
{
    if( _chart )
    {
        QTAChartProperties tp;
        _prop->properties( tp );
        _chart->setProperties( tp );
    }
    accept();
}


//----------------------------------------------------------------------------


OptionsDialog::OptionsDialog(QWidget* parent) :
    QDialog(parent)
{
    QFormLayout* form;
    QGridLayout* grid;
    QBoxLayout* lo2;

    setWindowTitle( "Options" );
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setModal(true);
    setMinimumWidth(500);

    _enableproxyOrig = gPref->enableproxy;
    _tickerLoaded = false;
    _tickerModified = false;

    QBoxLayout* lo = new QVBoxLayout( this );
    QTabWidget* tab = new QTabWidget;
    lo->addWidget( tab );

    lo->addSpacing( 8 );

    QDialogButtonBox* bbox = new QDialogButtonBox;
    bbox->setStandardButtons( QDialogButtonBox::Save |
                              QDialogButtonBox::Cancel );
    connect( bbox, SIGNAL(accepted()), this, SLOT(saveOptions()));
    connect( bbox, SIGNAL(rejected()), this, SLOT(reject()));
    lo->addWidget( bbox );


    QWidget* general = new QWidget;
    lo2 = new QVBoxLayout(general);

        form = new QFormLayout;
        lo2->addLayout( form );
        _keyIEX = new QLineEdit( gPref->iexapikey );
        _keyAlpha = new QLineEdit( gPref->avapikey );
        form->addRow( "IEX API key:", _keyIEX );
        form->addRow( "Alpha Vantage API key:", _keyAlpha );

        _convertGBP = new QCheckBox("Convert London prices to GBP");
        SET_CHECK( _convertGBP, gPref->longbp );
        lo2->addWidget(_convertGBP);
        _updateQuotes = new QCheckBox("Update quotes when chart opened");
        SET_CHECK( _updateQuotes, gPref->autoupdate );
        lo2->addWidget(_updateQuotes);

        lo2->addStretch();


    QWidget* ticker = new QWidget;
    grid = new QGridLayout( ticker );
    {
        QStringList hdr;
        hdr << QStringLiteral("Symbol") << QStringLiteral("Feed");

        _tickerList = new QTreeWidget;
        _tickerList->setRootIsDecorated( false );
        _tickerList->setColumnCount( 2 );
        _tickerList->setHeaderLabels( hdr );
        _tickerList->setEditTriggers( QAbstractItemView::NoEditTriggers );
        _tickerList->setSelectionBehavior( QAbstractItemView::SelectRows );
        _tickerList->installEventFilter(this);
        grid->addWidget( _tickerList, 0, 0, 2, 4 );

        _tsymbol = new QLineEdit;
        _tfeed = new QComboBox;
        _tfeed->addItem("IEX", 0);
        _tfeed->addItem("YAHOO", 1);
        QToolButton* add = new QToolButton;
        add->setIcon( QIcon(":/icons/add_item.png") );
        add->setIconSize(QSize(20, 20));
        connect( add, SIGNAL(clicked(bool)), this, SLOT(addTickerSymbol()) );
        QToolButton* rem = new QToolButton;
        rem->setIcon( QIcon(":/icons/remove_item.png") );
        rem->setIconSize(QSize(20, 20));
        connect( rem, SIGNAL(clicked(bool)), this, SLOT(removeTickerSymbol()) );
        _tsymbol->setMaximumWidth( _tfeed->sizeHint().width() );
        grid->addWidget( _tsymbol, 2, 0 );
        grid->addWidget( _tfeed, 2, 1 );
        grid->addWidget( add, 2, 2 );
        grid->addWidget( rem, 2, 3 );

        grid->addWidget( new QLabel("Scroll Speed:"), 0, 4, Qt::AlignRight );
        _tspeed = new QSpinBox;
        _tspeed->setRange( 10, 50 );
        _tspeed->setValue( gPref->scrollspeed );
        connect( _tspeed, SIGNAL(valueChanged(int)),
                 SIGNAL(tickerSpeedChanged(int)) );
        grid->addWidget( _tspeed, 0, 5 );

        grid->setRowStretch(1, 1);
    }


    _network = new QGroupBox;
    _network->setFlat(true);
    _network->setCheckable(true);
    _network->setChecked( gPref->enableproxy );
    _network->setTitle("&Enable Proxy");
    form = new QFormLayout( _network );

        _host    = new QLineEdit( gPref->proxyhost );
        _port    = new QSpinBox;
        _port->setMaximum( 32767 );
        _port->setValue( gPref->proxyport );
        _user    = new QLineEdit( gPref->proxyuser );
        _passwd  = new QLineEdit( gPref->proxypass );
        _timeout = new QSpinBox;
        _timeout->setRange( 20, 120 );
        _timeout->setSuffix(" sec.");
        _timeout->setValue( gPref->nettimeout );
        form->addRow( "Host:", _host );
        form->addRow( "Port:", _port );
        form->addRow( "User:", _user );
        form->addRow( "Password:", _passwd );
        form->addRow( "Timeout:", _timeout );


    _charts = new ChartPropertiesWidget;


    tab->addTab( general, "General" );
    tab->addTab( ticker, "Ticker" );
    tab->addTab( _network, "Network" );
    tab->addTab( _charts, "Chart Defaults" );
    //tab->addTab( "Develop" );

    connect( tab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)) );
}


void OptionsDialog::tabChanged(int index)
{
    if( index == 1 && ! _tickerLoaded )
    {
        populateTickerSymbols();
        _tickerLoaded = true;
    }
}


void OptionsDialog::populateTickerSymbols()
{
    QStringList symbol, feed;
    CG_ERR_RESULT err;

    err = gDatabase->loadTickerSymbols(symbol, feed);
    if(err != CG_ERR_OK)
    {
        showMessage(errorMessage(err), this);
        return;
    }

    _tickerList->clear();

    QStringList istr;
    istr << QStringLiteral("") << QStringLiteral("");
    for(int i = 0; i < symbol.size (); ++i)
    {
        istr[0] = symbol[i];
        istr[1] = feed[i];
        _tickerList->addTopLevelItem( new QTreeWidgetItem( istr ) );
    }
}


void OptionsDialog::updateTickerSymbols()
{
    QStringList symbol, feed;
    CG_ERR_RESULT err;

    QTreeWidgetItemIterator it(_tickerList);
    while(*it)
    {
        symbol << (*it)->text(0);
        feed   << (*it)->text(1);
        ++it;
    }

    err = gDatabase->saveTickerSymbols(symbol, feed);
    if( err != CG_ERR_OK )
        showMessage(errorMessage(err), this);
}


void OptionsDialog::addTickerSymbol()
{
    const QString& symbol = _tsymbol->text();
    if( symbol.isEmpty() )
        return;

    QList<QTreeWidgetItem*> found =
        _tickerList->findItems( symbol, Qt::MatchFixedString, 0);
    if( ! found.isEmpty() )
    {
        showMessage("Symbol already in ticker.", this);
        return;
    }

    QStringList istr;
    istr << symbol << _tfeed->currentText();
    _tickerList->addTopLevelItem( new QTreeWidgetItem( istr ) );
    _tickerList->scrollToBottom();

    _tickerModified = true;
}


void OptionsDialog::removeTickerSymbol()
{
    QList<QTreeWidgetItem *> sel = _tickerList->selectedItems();
    while( ! sel.isEmpty() )
    {
        delete sel.takeFirst();
        _tickerModified = true;
    }
}


bool OptionsDialog::eventFilter(QObject* obj, QEvent* ev)
{
    if( obj == _tickerList && ev->type() == QEvent::KeyPress )
    {
        if( static_cast<QKeyEvent*>(ev)->key() == Qt::Key_Delete )
        {
            removeTickerSymbol();
            return true;
        }
    }
    return QDialog::eventFilter(obj, ev);
}


void OptionsDialog::saveOptions()
{
    // General
    gPref->iexapikey  = _keyIEX->text();
    gPref->avapikey   = _keyAlpha->text();
    gPref->longbp     = _convertGBP->isChecked();
    gPref->autoupdate = _updateQuotes->isChecked();

    // Ticker
    gPref->scrollspeed = (qint16) _tspeed->value();

    // Network
    gPref->enableproxy = _network->isChecked();
    gPref->proxyhost  = _host->text();
    gPref->proxyport  = _port->value();
    gPref->proxyuser  = _user->text();
    gPref->proxypass  = _passwd->text();
    gPref->nettimeout = _timeout->value();

    // Chart Defaults
    _charts->properties( gPref->chart );

    // TODO: Only apply if anything actually changed.  Until precise change
    // tracking is implemented we only skip apply if the proxy is now
    // disabled and was also originally disabled.
    if( gPref->enableproxy || (_enableproxyOrig != gPref->enableproxy) )
        NetService::applyProxyOptions(gPref);

    if( _tickerModified )
        updateTickerSymbols();

    int result = saveAppOptions(gPref);
    if(result != CG_ERR_OK)
        showMessage(errorMessage(result), this);

    accept();
}

void OptionsDialog::showEvent(QShowEvent *event)
{
    _enableproxyOrig = gPref->enableproxy;
    _tickerLoaded = false;
    _tickerModified = false;

    // General
    _keyIEX->setText( gPref->iexapikey );
    _keyAlpha->setText( gPref->avapikey );
    _convertGBP->setChecked( gPref->longbp );
    _updateQuotes->setChecked( gPref->autoupdate );

    // Ticker
    _tspeed->setValue( gPref->scrollspeed );

    // Network
    _network->setChecked( gPref->enableproxy );
    _host->setText( gPref->proxyhost );
    _port->setValue( gPref->proxyport );
    _user->setText( gPref->proxyuser );
    _passwd->setText( gPref->proxypass );
    _timeout->setValue( gPref->nettimeout );

    // Chart Defaults
    _charts->setProperties( gPref->chart );

    QDialog::showEvent(event);
}
