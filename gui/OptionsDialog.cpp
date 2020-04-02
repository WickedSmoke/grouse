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
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QToolButton>
#include "OptionsDialog.h"
#include "defs.h"       // Application_Options
#include "chartapp.h"   // showMessage
#include "qtachart.h"


#define gPref   Application_Options
#define SET_CHECK(cb,opt)  cb->setCheckState(opt?Qt::Checked : Qt::Unchecked)
#define COLOR_CONNECT(oc) \
    connect(oc, SIGNAL(clicked(bool)), SLOT(colorClicked()))


class OptionColor : public QToolButton
{
public:
    OptionColor( const QColor& color ) : _pix(24, 24), _col(color)
    {
        setColor( color );
    }

    void setColor( const QColor& color )
    {
        _pix.fill( color );
        QIcon icon( _pix );
        setIcon(icon);
    }

    const QColor& color() const { return _col; }

private:
    QPixmap _pix;
    QColor  _col;
};


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
        form->addRow( new QLabel("IEX API key:"), _keyIEX );
        form->addRow( new QLabel("Alpha Vantage API key:"), _keyAlpha );

        _convertGBP = new QCheckBox("Convert London prices to GBP");
        SET_CHECK( _convertGBP, gPref->longbp );
        lo2->addWidget(_convertGBP);
        _updateQuotes = new QCheckBox("Update quotes when chart opened");
        SET_CHECK( _updateQuotes, gPref->autoupdate );
        lo2->addWidget(_updateQuotes);

        lo2->addStretch();


    QWidget* charts = new QWidget;
    grid = new QGridLayout( charts );

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

            _lineColor = new OptionColor( gPref->linecolor );
            COLOR_CONNECT( _lineColor );
            g2->addWidget( _lineColor, 0, 0 );
            _barColor = new OptionColor( gPref->barcolor );
            COLOR_CONNECT( _barColor );
            g2->addWidget( _barColor, 3, 0 );

            g2->setRowStretch(4, 1);

            setChartStyle( gPref->chartstyle );

        lo2 = new QVBoxLayout;
        grid->addLayout(lo2, 0, 1);

            _showGrid = new QCheckBox("Show Grid");
            SET_CHECK( _showGrid, gPref->showgrid );
            lo2->addWidget(_showGrid);
            _showVolumes = new QCheckBox("Show Volumes");
            SET_CHECK( _showVolumes, gPref->showvolume );
            lo2->addWidget(_showVolumes);
            _linearScale = new QCheckBox("Use Linear Scale");
            SET_CHECK( _linearScale, gPref->linear );
            lo2->addWidget(_linearScale);
            _onlinePrice = new QCheckBox("Show Online Price");
            SET_CHECK( _onlinePrice, gPref->showonlineprice );
            lo2->addWidget(_onlinePrice);

            lo2->addStretch();

            g2 = new QGridLayout;
            lo2->addLayout(g2);

                _fgColor = new OptionColor( gPref->forecolor );
                COLOR_CONNECT( _fgColor );
                g2->addWidget( _fgColor, 0, 0 );
                g2->addWidget( new QLabel("Foreground Color"), 0, 1 );
                _bgColor = new OptionColor( gPref->backcolor );
                COLOR_CONNECT( _bgColor );
                g2->addWidget( _bgColor, 1, 0 );
                g2->addWidget( new QLabel("Background Color"), 1, 1 );


    tab->addTab( general, "General" );
    tab->addTab( new QWidget, "Ticker" );
    tab->addTab( new QWidget, "Network" );
    tab->addTab( charts, "Chart Defaults" );
    //tab->addTab( "Develop" );
}


void OptionsDialog::colorClicked()
{
    OptionColor* btn = static_cast<OptionColor*>( sender() );
    QColorDialog* dlg = new QColorDialog(this);
    dlg->setOption(QColorDialog::DontUseNativeDialog, true);
    if( dlg->exec() == QDialog::Accepted )
        btn->setColor( dlg->selectedColor() );
}


int OptionsDialog::chartStyle()
{
    if( _lineChart->isChecked() )
        return QTACHART_LINE;
    if( _candleChart->isChecked() )
        return QTACHART_CANDLE;
    if( _heikinChart->isChecked() )
        return QTACHART_HEIKINASHI;
    return QTACHART_BAR;
}


void OptionsDialog::setChartStyle( int style )
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


void OptionsDialog::saveOptions()
{
    // General
    gPref->iexapikey  = _keyIEX->text();
    gPref->avapikey   = _keyAlpha->text();
    gPref->longbp     = _convertGBP->isChecked();
    gPref->autoupdate = _updateQuotes->isChecked();

    // Chart Defaults
    gPref->linecolor  = _lineColor->color();
    gPref->barcolor   = _barColor->color();
    gPref->forecolor  = _fgColor->color();
    gPref->backcolor  = _bgColor->color();
    gPref->chartstyle = chartStyle();
    gPref->showgrid   = _showGrid->isChecked();
    gPref->showvolume = _showVolumes->isChecked();
    gPref->linear     = _linearScale->isChecked();
    gPref->showonlineprice = _onlinePrice->isChecked();

    int result = saveAppOptions(gPref);
    if(result != CG_ERR_OK)
        showMessage(errorMessage(result), this);

    accept();
}

void OptionsDialog::showEvent(QShowEvent *event)
{
    // General
    _keyIEX->setText( gPref->iexapikey );
    _keyAlpha->setText( gPref->avapikey );
    _convertGBP->setChecked( gPref->longbp );
    _updateQuotes->setChecked( gPref->autoupdate );

    // Chart Defaults
    _lineColor->setColor( gPref->linecolor );
    _barColor->setColor( gPref->barcolor );
    _fgColor->setColor( gPref->forecolor );
    _bgColor->setColor( gPref->backcolor );
    setChartStyle( gPref->chartstyle );
    _showGrid->setChecked( gPref->showgrid );
    _showVolumes->setChecked( gPref->showvolume );
    _linearScale->setChecked( gPref->linear );
    _onlinePrice->setChecked( gPref->showonlineprice );

    QDialog::showEvent(event);
}