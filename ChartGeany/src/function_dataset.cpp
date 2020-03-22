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

#include <memory>

#include "ta_func.h"
#include "function_dataset.h"

using std::nothrow;
using std::unique_ptr;

// dummy: returns input
GNUMALLOC DataSet
DUMMY (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  qint32 setsize;

  if (period > 0)
    period = 0;

  result = new (nothrow) PriceVector;
  if (result == nullptr)
    return result;

  setsize = dset->size ();

  for (qint32 counter = 0; counter < setsize; counter ++)
    result->append ((qreal) dset->at (counter));
  return result;
}

// simple moving average
GNUMALLOC DataSet
SMA (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MA(0, setsize - 1, input.get (), period, TA_MAType_SMA,
                    &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// exponential moving average
GNUMALLOC DataSet
EMA (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MA(0, setsize - 1, input.get (), period, TA_MAType_EMA,
                    &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// parabolic SAR
GNUMALLOC DataSet
PSAR (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
  }

  retcode = TA_S_SAR (0, setsize - 1, high.get (), low.get (), 0.1, 0.1,
                      &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// relative strength index
GNUMALLOC DataSet
RSI (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_RSI (0, setsize - 1, input.get (), period,
                      &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;

  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// money flow index
GNUMALLOC DataSet
MFI (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <float[]>volume (new (nothrow) float[setsize + 1]());
  if (volume == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
    volume.get ()[setsize - counter] =  frame.Volume;
  }

  retcode = TA_S_MFI (0, setsize - 1, high.get (), low.get (),
                      close.get (), volume.get (), period,
                      &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// rate of change
GNUMALLOC DataSet
ROC (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_ROC (0, setsize - 1, input.get (), period,
                      &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// Williams %R
GNUMALLOC DataSet
WILLR (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_WILLR (0, setsize - 1, high.get (), low.get (),
                        close.get (), period, &outBegIdx, &outNbElement,
                        output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
    {
      if ((qreal) output.get ()[counter] > 0 || (qreal) output.get ()[counter] < -100)
        result->append (0);
      else
        result->append ((qreal) output.get ()[counter]);
    }
  }

  return result;
}

// slow stochastic %K
GNUMALLOC DataSet
STOCHSLOWK (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> outputK (new (nothrow) double[setsize + 1]());
  if (outputK == nullptr)
    return result;

  unique_ptr <double[]> outputD (new (nothrow) double[setsize + 1]());
  if (outputD == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_STOCH (0, setsize - 1, high.get (), low.get (),
                        close.get (), period, 3, TA_MAType_SMA, 3,
                        TA_MAType_SMA, &outBegIdx, &outNbElement,
                        outputK.get (), outputD.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
    {
      if (qAbs ((qreal) outputK.get ()[counter]) > 10000)
        result->append (0);
      else
        result->append ((qreal) outputK.get ()[counter]);
    }
  }

  return result;
}

// slow stochastic %D
GNUMALLOC DataSet
STOCHSLOWD (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> outputK (new (nothrow) double[setsize + 1]());
  if (outputK == nullptr)
    return result;

  unique_ptr <double[]> outputD (new (nothrow) double[setsize + 1]());
  if (outputD == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_STOCH (0, setsize - 1, high.get (), low.get (),
                        close.get (), period, 3, TA_MAType_SMA, 3,
                        TA_MAType_SMA, &outBegIdx, &outNbElement,
                        outputK.get (), outputD.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) outputD.get ()[counter]);
  }

  return result;
}

// fast stochastic %K
GNUMALLOC DataSet
STOCHFASTK (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> outputK (new (nothrow) double[setsize + 1]());
  if (outputK == nullptr)
    return result;

  unique_ptr <double[]> outputD (new (nothrow) double[setsize + 1]());
  if (outputD == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_STOCHF (0, setsize - 1, high.get (), low.get (),
                         close.get (), period, 3, TA_MAType_SMA,
                         &outBegIdx, &outNbElement, outputK.get (),
                         outputD.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) outputK.get ()[counter]);
  }

  return result;
}

// fast stochastic %D
GNUMALLOC DataSet
STOCHFASTD (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> outputK (new (nothrow) double[setsize + 1]());
  if (outputK == nullptr)
    return result;

  unique_ptr <double[]> outputD (new (nothrow) double[setsize + 1]());
  if (outputD == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_STOCHF (0, setsize - 1, high.get (), low.get (),
                         close.get (), period, 3, TA_MAType_SMA,
                         &outBegIdx, &outNbElement, outputK.get (),
                         outputD.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) outputD.get ()[counter]);
  }

  return result;
}

// moving average convergence/divergence
GNUMALLOC DataSet
MACD (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> signal (new (nothrow) double[setsize + 1]());
  if (signal == nullptr)
    return result;

  unique_ptr <double[]> hist (new (nothrow) double[setsize + 1]());
  if (hist == nullptr)
    return result;

  unique_ptr <double[]> macd (new (nothrow) double[setsize + 1]());
  if (macd == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MACDFIX (0, setsize - 1, input.get (), period,
                          &outBegIdx, &outNbElement, macd.get (),
                          signal.get (), hist.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      if (qAbs ((qreal) macd.get ()[counter]) > 100000)
        result->append (0);
      else
        result->append ((qreal) macd.get ()[counter]);
  }

  return result;
}

// moving average convergence/divergence signal
GNUMALLOC DataSet
MACDSIGNAL (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> signal (new (nothrow) double[setsize + 1]());
  if (signal == nullptr)
    return result;

  unique_ptr <double[]> hist (new (nothrow) double[setsize + 1]());
  if (hist == nullptr)
    return result;

  unique_ptr <double[]> macd (new (nothrow) double[setsize + 1]());
  if (macd == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MACDFIX (0, setsize - 1, input.get (), period,
                          &outBegIdx, &outNbElement, macd.get (),
                          signal.get (), hist.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) signal.get ()[counter]);
  }

  return result;
}

// moving average convergence/divergence histogram
GNUMALLOC DataSet
MACDHIST (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> signal (new (nothrow) double[setsize + 1]());
  if (signal == nullptr)
    return result;

  unique_ptr <double[]> hist (new (nothrow) double[setsize + 1]());
  if (hist == nullptr)
    return result;

  unique_ptr <double[]> macd (new (nothrow) double[setsize + 1]());
  if (macd == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MACDFIX (0, setsize - 1, input.get (), period,
                          &outBegIdx, &outNbElement, macd.get (),
                          signal.get (), hist.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) hist.get ()[counter]);
  }

  return result;
}

// bollinger bands upper
GNUMALLOC DataSet
BBANDSUPPER (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> upper (new (nothrow) double[setsize + 1]());
  if (upper == nullptr)
    return result;

  unique_ptr <double[]> middle (new (nothrow) double[setsize + 1]());
  if (middle == nullptr)
    return result;

  unique_ptr <double[]> lower (new (nothrow) double[setsize + 1]());
  if (lower == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_BBANDS (0, setsize - 1, input.get (), period, 2.0, 2.0,
                         TA_MAType_SMA, &outBegIdx, &outNbElement,
                         upper.get (), middle.get (), lower.get ());
  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) upper.get ()[counter]);
  }

  return result;
}

// bollinger bands middle
GNUMALLOC DataSet
BBANDSMIDDLE (const DataSet dset, int period)
{
 PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> upper (new (nothrow) double[setsize + 1]());
  if (upper == nullptr)
    return result;

  unique_ptr <double[]> middle (new (nothrow) double[setsize + 1]());
  if (middle == nullptr)
    return result;

  unique_ptr <double[]> lower (new (nothrow) double[setsize + 1]());
  if (lower == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_BBANDS (0, setsize - 1, input.get (), period, 2.0, 2.0,
                         TA_MAType_SMA, &outBegIdx, &outNbElement,
                         upper.get (), middle.get (), lower.get ());
  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) middle.get ()[counter]);
  }

  return result;
}

// bollinger bands lower
GNUMALLOC DataSet
BBANDSLOWER (const DataSet dset, int period)
{
 PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> upper (new (nothrow) double[setsize + 1]());
  if (upper == nullptr)
    return result;

  unique_ptr <double[]> middle (new (nothrow) double[setsize + 1]());
  if (middle == nullptr)
    return result;

  unique_ptr <double[]> lower (new (nothrow) double[setsize + 1]());
  if (lower == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_BBANDS (0, setsize - 1, input.get (), period, 2.0, 2.0,
                         TA_MAType_SMA, &outBegIdx, &outNbElement,
                         upper.get (), middle.get (), lower.get ());
  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) lower.get ()[counter]);
  }

  return result;
}

// average directional movement index
GNUMALLOC DataSet
ADX (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_ADX (0, setsize - 1, high.get (), low.get (), close.get (),
                      period, &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// aroon up
GNUMALLOC DataSet
AROONUP (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <double[]> up (new (nothrow) double[setsize + 1]());
  if (up == nullptr)
    return result;

  unique_ptr <double[]> down (new (nothrow) double[setsize + 1]());
  if (down == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
  }

  retcode = TA_S_AROON (0, setsize - 1, high.get (), low.get (), period,
                        &outBegIdx, &outNbElement, down.get (), up.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) up.get ()[counter]);
  }

  return result;
}

// aroon down
GNUMALLOC DataSet
AROONDOWN (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <double[]> up (new (nothrow) double[setsize + 1]());
  if (up == nullptr)
    return result;

  unique_ptr <double[]> down (new (nothrow) double[setsize + 1]());
  if (down == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
  }

  retcode = TA_S_AROON (0, setsize - 1, high.get (), low.get (), period,
                        &outBegIdx, &outNbElement, down.get (), up.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) down.get ()[counter]);
  }

  return result;
}

// commodity channel index
GNUMALLOC DataSet
CCI (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_CCI (0, setsize - 1, high.get (), low.get (), close.get (),
                      period, &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// standard deviation
GNUMALLOC DataSet
STDDEV (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_STDDEV (0, setsize - 1, input.get (), period, period,
                         &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;

  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// momentum
GNUMALLOC DataSet
MOMENTUM (const DataSet dset, int period)
{
  PriceVector *result = nullptr;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = dset->size ();

  unique_ptr <float[]>input (new (nothrow) float[setsize + 1]());
  if (input == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
    input.get ()[setsize - counter] = dset->at (counter - 1);

  retcode = TA_S_MOM (0, setsize - 1, input.get (), period,
                      &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;

  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// directional movement index
GNUMALLOC DataSet
DMX (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_DX (0, setsize - 1, high.get (), low.get (), close.get (),
                     period, &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}

// average true range
GNUMALLOC DataSet
ATR (const FrameVector *HLOC, int period)
{
  PriceVector *result = nullptr;
  QTAChartFrame frame;
  TA_RetCode retcode;
  int outBegIdx, outNbElement;
  qint32 setsize;

  if (period < 2)
    period = 2;

  setsize = HLOC->size ();

  unique_ptr <float[]>high (new (nothrow) float[setsize + 1]());
  if (high == nullptr)
    return result;

  unique_ptr <float[]>low (new (nothrow) float[setsize + 1]());
  if (low == nullptr)
    return result;

  unique_ptr <float[]>close (new (nothrow) float[setsize + 1]());
  if (close == nullptr)
    return result;

  unique_ptr <double[]> output (new (nothrow) double[setsize + 1]());
  if (output == nullptr)
    return result;

  for (qint32 counter = setsize; counter > 0; counter --)
  {
    frame = HLOC->at (counter - 1);
    high.get ()[setsize - counter] = frame.High;
    low.get ()[setsize - counter] =  frame.Low;
    close.get ()[setsize - counter] =  frame.Close;
  }

  retcode = TA_S_ATR (0, setsize - 1, high.get (), low.get (), close.get (),
                      period, &outBegIdx, &outNbElement, output.get ());

  result = new (nothrow) PriceVector;
  if (retcode == TA_SUCCESS && result != nullptr)
  {
    result->reserve (outNbElement);
    for (qint32 counter = outNbElement - 1; counter >=0; counter --)
      result->append ((qreal) output.get ()[counter]);
  }

  return result;
}
