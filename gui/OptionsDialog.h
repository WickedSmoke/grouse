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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>


class QCheckBox;
class QColorDialog;
class QLineEdit;
class QGroupBox;
class QRadioButton;
class QSpinBox;
class OptionColor;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    OptionsDialog(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent*);

private slots:
    void colorSel(const QColor&);
    void setChartStyle(int);
    void colorClicked();
    void saveOptions();

private:
    int chartStyle();

    QColorDialog* _colorDialog;
    OptionColor* _lastColorClicked;

    QLineEdit* _keyIEX ;
    QLineEdit* _keyAlpha;
    QCheckBox* _convertGBP;
    QCheckBox* _updateQuotes;

    QGroupBox* _network;
    QLineEdit* _host;
    QSpinBox*  _port;
    QLineEdit* _user;
    QLineEdit* _passwd;
    QSpinBox*  _timeout;

    QRadioButton* _lineChart;
    QRadioButton* _candleChart;
    QRadioButton* _heikinChart;
    QRadioButton* _barChart;
    OptionColor* _lineColor;
    OptionColor* _barColor;

    QCheckBox* _showGrid;
    QCheckBox* _showVolumes;
    QCheckBox* _linearScale;
    QCheckBox* _onlinePrice;
    OptionColor* _fgColor;
    OptionColor* _bgColor;

    bool _enableproxyOrig;
};


#endif // OPTIONSDIALOG_H
