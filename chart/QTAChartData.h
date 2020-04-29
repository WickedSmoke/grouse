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

#ifndef QTACHARTDATA_H
#define QTACHARTDATA_H

//#include <QMetaType>
#include <QString>
#include <QVector>


enum
{
    QTACHART_TFDAY = 0,     // timeframe: day
    QTACHART_TFWEEK,        // timeframe: week
    QTACHART_TFMONTH,       // timeframe: month

    QTACHART_LINEAR = 100,  // chart: linear price scale
    QTACHART_LOGARITHMIC,   // chart: logarithmic price scale

    QTACHART_CANDLE = 200,  // chart: candle chart
    QTACHART_HEIKINASHI,    // chart: heikin-ashi candle chart
    QTACHART_BAR,           // chart: bar chart
    QTACHART_LINE           // chart: line chart
};


// fundamenta data as loaded from sqlite table
typedef struct
{
    QString bv;
    QString mc;
    QString ebitda;
    QString pe;
    QString peg;
    QString dy;
    QString epscurrent;
    QString epsnext;
    QString es;
    QString ps;
    QString pbv;
} QTAChartData;

Q_DECLARE_TYPEINFO (QTAChartData, Q_MOVABLE_TYPE);


typedef struct alignas (max_align_t) // frame data as loaded from sqlite table
{
    QString Text;
    qreal High;
    qreal Low;
    qreal Open;
    qreal Close;
    qreal AdjClose;
    qreal Volume;
    quint16 year;
    quint16 month;
    quint16 day;
    char Date[16];
    char Time[16];
} QTAChartFrame;

Q_DECLARE_TYPEINFO (QTAChartFrame, Q_MOVABLE_TYPE);
typedef QVector < QTAChartFrame > FrameVector;


struct QTAChartProperties
{
    uint32_t foreColor;
    uint32_t backColor;
    uint32_t barColor;
    uint32_t lineColor;
    uint16_t style;       // QTACHART_CANDLE, etc.
    bool linearScale;
    bool showGrid;
    bool showVolume;
    bool showOnlinePrice;
};

#endif // QTACHARTDATA_H
