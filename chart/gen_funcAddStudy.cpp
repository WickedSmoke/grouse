// Code generated by gen-param.b from qtachart_functions.cpp
// This is the body of QTACFunctions::addIndicator()

  if (fname == QLatin1String ("SMA"))
  {
    obj = referencechart->addStudySMA( (qint32) PARAM("Period"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("EMA"))
  {
    obj = referencechart->addStudyEMA( (qint32) PARAM("Period"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("MACD"))
  {
    obj = referencechart->addStudyMACD( (qint32) PARAM("Period"), PARAM("MACD color"), PARAM("Signal color") );
  }
  else if (fname == QLatin1String ("MFI"))
  {
    obj = referencechart->addStudyMFI( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Medium level"), (qint32) PARAM("Low level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("ROC"))
  {
    obj = referencechart->addStudyROC( (qint32) PARAM("Period"), (qint32) PARAM("Level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("RSI"))
  {
    obj = referencechart->addStudyRSI( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Low level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Slow Stoch"))
  {
    obj = referencechart->addStudySlowStoch( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Medium level"), (qint32) PARAM("Low level"), PARAM("%K color"), PARAM("%D color") );
  }
  else if (fname == QLatin1String ("Fast Stoch"))
  {
    obj = referencechart->addStudyFastStoch( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Medium level"), (qint32) PARAM("Low level"), PARAM("%K color"), PARAM("%D color") );
  }
  else if (fname == QLatin1String ("W%R"))
  {
    obj = referencechart->addStudyW_pct_R( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Low level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Bollinger Bands"))
  {
    obj = referencechart->addStudyBollingerBands( (qint32) PARAM("Period"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Parabolic SAR"))
  {
    obj = referencechart->addStudyParabolicSAR( PARAM("Color") );
  }
  else if (fname == QLatin1String ("ADX"))
  {
    obj = referencechart->addStudyADX( (qint32) PARAM("Period"), (qint32) PARAM("Weak"), (qint32) PARAM("Strong"), (qint32) PARAM("Very strong"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Aroon"))
  {
    obj = referencechart->addStudyAroon( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Medium level"), (qint32) PARAM("Low level"), PARAM("Up color"), PARAM("Down color") );
  }
  else if (fname == QLatin1String ("CCI"))
  {
    obj = referencechart->addStudyCCI( (qint32) PARAM("Period"), (qint32) PARAM("High level"), (qint32) PARAM("Low level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("STDDEV"))
  {
    obj = referencechart->addStudySTDDEV( (qint32) PARAM("Period"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Momentum"))
  {
    obj = referencechart->addStudyMomentum( (qint32) PARAM("Period"), (qint32) PARAM("Level"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("DMI"))
  {
    obj = referencechart->addStudyDMI( (qint32) PARAM("Period"), (qint32) PARAM("Weak"), (qint32) PARAM("Strong"), (qint32) PARAM("Very strong"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("ATR"))
  {
    obj = referencechart->addStudyATR( (qint32) PARAM("Period"), PARAM("Color") );
  }
  else if (fname == QLatin1String ("Range"))
  {
    obj = referencechart->addStudyRange( PARAM("Volatile"), PARAM("Quiet"), PARAM("Color") );
  }
