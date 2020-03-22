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

#include "function_dataset.h"
#include "qtachart_core.h"
#include "cgscript.h"


#define ARRAYID(p,t,c)  QString (QString (Q_FUNC_INFO) %\
                                 QString::number ((quint64) p) %\
                                 QStringLiteral("-") %\
                                 QString::number ((quint64) c) %\
                                 QStringLiteral("-") %\
                                 QString::number (t)).toStdString().c_str()


using std::unique_ptr;

/* return HLOC of selected timeframe */
static const FrameVector *
hloc (const void *ptr, TimeFrame_t tf)
{
  Q_UNUSED (QTACastFromConstVoid)

  const QTAChartCore *core =  static_cast <const QTAChartCore *> (ptr);
  QString TF;

  if (tf == TF_DAY) TF = QStringLiteral ("D");
  else if (tf == TF_WEEK) TF = QStringLiteral ("W");
  else if (tf == TF_MONTH) TF = QStringLiteral ("M");
  else if (tf == TF_YEAR) TF = QStringLiteral ("Y");

  int counter = 0;
  while (core->TIMEFRAME.at (counter).TFSymbol != TF) counter ++;

  return &(core->TIMEFRAME.at (counter)).HLOC;
}

/* return OPEN/HIGH/LOW or CLOSE DataSet of selected timeframe
   and applied price */
GNUMALLOC static DataSet
hloc (const void *ptr, TimeFrame_t tf, Price_t appliedprice)
{
  const FrameVector *HLOC;
  PriceVector *dset = new (std::nothrow) PriceVector;
  if (dset == nullptr)
    return nullptr;

  HLOC = hloc (ptr, tf);
  dset->reserve (HLOC->size ());

  foreach (const QTAChartFrame frame, *HLOC)
  {
    if (appliedprice == CLOSE_PRICE)
      dset->append (frame.Close);
    else if (appliedprice == OPEN_PRICE)
      dset->append (frame.Open);
    else if (appliedprice == HIGH_PRICE)
      dset->append (frame.High);
    else if (appliedprice == LOW_PRICE)
      dset->append (frame.Low);
  }

  return dset;
}

/* ADX implementation */
extern "C" Q_DECL_EXPORT Array_t
fADX_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (ADX (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> ((&r))) == -1)
      return nullptr;

  return result;
}

/* ATR implementation */
extern "C" Q_DECL_EXPORT Array_t
fATR_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (ATR (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* AROONUP implementation */
extern "C" Q_DECL_EXPORT Array_t
fAROONUP_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (AROONUP (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* AROONDOWN implementation */
extern "C" Q_DECL_EXPORT Array_t
fAROONDOWN_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (AROONDOWN (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* BBANDSUPPER implementation */
extern "C" Q_DECL_EXPORT Array_t
fBBANDSUPPER_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (BBANDSUPPER (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* BBANDSLOWER implementation */
extern "C" Q_DECL_EXPORT Array_t
fBBANDSLOWER_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (BBANDSLOWER (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* BBANDSMIDDLE implementation */
extern "C" Q_DECL_EXPORT Array_t
fBBANDSMIDDLE_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (BBANDSMIDDLE (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* CCI implementation */
extern "C" Q_DECL_EXPORT Array_t
fCCI_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (CCI (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* DMX implementation */
extern "C" Q_DECL_EXPORT Array_t
fDMX_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (DMX (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* EMA implementation */
extern "C" Q_DECL_EXPORT Array_t
fEMA_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{

  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (EMA (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Generic EMA implementation */
extern "C" Q_DECL_EXPORT Array_t
gEMA_imp (const void *ptr, int period, Array_t data)
{
  DataSet dsetin;
  PriceVector pv;
  Array_t result = nullptr;
  int dim = ArraySize_imp (data);

  for(int counter = 0; counter < dim; counter ++)
    pv += *(qreal *)ArrayGet_imp (data, counter);

  dsetin = &pv;
  unique_ptr <PriceVector> dsetout (EMA (dsetin, period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, 0, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Slow Stochastic %K implementation */
extern "C" Q_DECL_EXPORT Array_t
fSTOCHSLOWK_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (STOCHSLOWK (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Slow Stochastic %D implementation */
extern "C" Q_DECL_EXPORT Array_t
fSTOCHSLOWD_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (STOCHSLOWD (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Fast Stochastic %K implementation */
extern "C" Q_DECL_EXPORT Array_t
fSTOCHFASTK_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (STOCHFASTK (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Fast Stochastic %D implementation */
extern "C" Q_DECL_EXPORT Array_t
fSTOCHFASTD_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (STOCHFASTD (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* MACD implementation */
extern "C" Q_DECL_EXPORT Array_t
fMACD_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector>  dsetout (MACD (dsetin.get (), period));

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* MACDSIGNAL implementation */
extern "C" Q_DECL_EXPORT Array_t
fMACDSIGNAL_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (MACDSIGNAL (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* MACDHIST implementation */
extern "C" Q_DECL_EXPORT Array_t
fMACDHIST_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (MACDHIST (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* MFI implementation */
extern "C" Q_DECL_EXPORT Array_t
fMFI_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t   result;

  HLOC = hloc (ptr, tf);
  unique_ptr <PriceVector> dset (MFI (HLOC, period));
  if (dset == nullptr)
    return nullptr;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* MOMENTUM implementation */
extern "C" Q_DECL_EXPORT Array_t
fMOMENTUM_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (MOMENTUM (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* PSAR implementation */
extern "C" Q_DECL_EXPORT Array_t
fPSAR_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);

  unique_ptr <PriceVector> dset (PSAR (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* ROC implementation */
extern "C" Q_DECL_EXPORT Array_t
fROC_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (ROC (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* RSI implementation */
extern "C" Q_DECL_EXPORT Array_t
fRSI_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (RSI (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* SMA implementation */
extern "C" Q_DECL_EXPORT Array_t
fSMA_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (SMA (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* Generic SMA implementation */
extern "C" Q_DECL_EXPORT Array_t
gSMA_imp (const void *ptr, int period, Array_t data)
{
  PriceVector pv;
  Array_t result = nullptr;
  int dim = ArraySize_imp (data);

  for (int counter = 0; counter < dim; counter ++)
    pv += *(qreal *) ArrayGet_imp (data, counter);

  DataSet dsetin = &pv;
  unique_ptr <PriceVector> dsetout (SMA (dsetin, period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, 0, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* STDDEV implementation */
extern "C" Q_DECL_EXPORT Array_t
fSTDDEV_imp (const void *ptr, TimeFrame_t tf, int period, Price_t appliedprice)
{
  Array_t result = nullptr;

  unique_ptr <PriceVector> dsetin (hloc (ptr, tf, appliedprice));
  if (dsetin == nullptr)
    return result;

  unique_ptr <PriceVector> dsetout (STDDEV (dsetin.get (), period));
  if (dsetout == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dsetout.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dsetout.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}

/* WILLR implementation */
extern "C" Q_DECL_EXPORT Array_t
fWILLR_imp (const void *ptr, TimeFrame_t tf, int period)
{
  const FrameVector *HLOC;
  Array_t result = nullptr;

  HLOC = hloc (ptr, tf);

  unique_ptr <PriceVector> dset (WILLR (HLOC, period));
  if (dset == nullptr)
    return result;

  result = ArrayCreate2_imp (ptr, ARRAYID(ptr, tf, period),
                             sizeof (double), dset.get ()->size ());
  if (result == nullptr)
    return result;

  foreach (const qreal r, *dset.get ())
    if (ArrayAppend_imp (result, const_cast <qreal *> (&r)) == -1)
      return nullptr;

  return result;
}
