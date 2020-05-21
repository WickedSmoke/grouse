Chart Grouse 0.3
================

Chart Grouse is a program for technical market analysis.  It is a fork of
[Chart Geany](https://chart-geany.sourceforge.io/) 6.1.5
which adds an alternate desktop style GUI.

The code is being refactored to separate the database, network, and chart
code from the touch-based GUI.


Building
========

Some of the third party code is not part of this repository so a copy of the
ChartGeany/3rdparty directory from [chartgeany-6.1.5.tar.gz](https://sourceforge.net/projects/chart-geany/files/6.0/chartgeany-6.1.5.tar.gz/download)
must be be obtained.  The following two directories must be copied into
3rdparty:

    sqlite3
    x86dis

After that use *qmake; make*.
