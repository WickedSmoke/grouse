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

#ifndef FUNCTION_DATASET_H
#define FUNCTION_DATASET_H

#include "defs.h"
#include "QTAChartData.h"

typedef QVector < qreal >PriceVector;
typedef PriceVector* DataSet;

// dummy: returns input
DataSet DUMMY (const DataSet dset, int period) GNUMALLOC;

// simple moving average
DataSet SMA (const DataSet dset, int period) GNUMALLOC;

// exponential moving average
DataSet EMA (const DataSet dset, int period) GNUMALLOC;

// parabolic SAR
DataSet PSAR (const FrameVector *HLOC, int period) GNUMALLOC;

// relative strength index
DataSet RSI (const DataSet dset, int period) GNUMALLOC;

// money flow index
DataSet MFI (const FrameVector *HLOC, int period) GNUMALLOC;

// rate of change
DataSet ROC (const DataSet dset, int period) GNUMALLOC;

// Williams %R
DataSet WILLR (const FrameVector *HLOC, int period) GNUMALLOC;

// slow stochastic %K
DataSet STOCHSLOWK (const FrameVector *HLOC, int period) GNUMALLOC;

// slow stochastic %D
DataSet STOCHSLOWD (const FrameVector *HLOC, int period) GNUMALLOC;

// fast stochastic %K
DataSet STOCHFASTK (const FrameVector *HLOC, int period) GNUMALLOC;

// fast stochastic %D
DataSet STOCHFASTD (const FrameVector *HLOC, int period) GNUMALLOC;

// moving average convergence/divergence
DataSet MACD (const DataSet dset, int period) GNUMALLOC;

// moving average convergence/divergence signal
DataSet MACDSIGNAL (const DataSet dset, int period) GNUMALLOC;

// moving average convergence/divergence histogram
DataSet MACDHIST (const DataSet dset, int period) GNUMALLOC;

// bollinger bands upper
DataSet BBANDSUPPER (const DataSet dset, int period) GNUMALLOC;

// bollinger bands middle
DataSet BBANDSMIDDLE (const DataSet dset, int period) GNUMALLOC;

// bollinger bands lower
DataSet BBANDSLOWER (const DataSet dset, int period) GNUMALLOC;

// average directional movement index
DataSet ADX (const FrameVector *HLOC, int period) GNUMALLOC;

// aroon up
DataSet AROONUP (const FrameVector *HLOC, int period) GNUMALLOC;

// aroon down
DataSet AROONDOWN (const FrameVector *HLOC, int period) GNUMALLOC;

// commodity channel index
DataSet CCI (const FrameVector *HLOC, int period) GNUMALLOC;

// standard deviation
DataSet STDDEV (const DataSet dset, int period) GNUMALLOC;

// momentum
DataSet MOMENTUM (const DataSet dset, int period) GNUMALLOC;

// directional movement index
DataSet DMX (const FrameVector *HLOC, int period) GNUMALLOC;

// average true range
DataSet ATR (const FrameVector *HLOC, int period) GNUMALLOC;

// range
DataSet Range (const FrameVector *HLOC, int) GNUMALLOC;

#endif // FUNCTION_DATASET_H
