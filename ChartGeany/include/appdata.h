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

#ifndef APPDATA_H
#define APPDATA_H

#if defined (Q_OS_MAC) && defined (Q_CC_GNU)
#include <cinttypes>
#endif // Q_OS_MAC && Q_CC_GNU

#define APPNAME         QStringLiteral ("Chart Geany")
#define APPDIR          QStringLiteral ("ChartGeany")
#define APPWEBPAGE      QStringLiteral ("https://chart-geany.sourceforge.io")
#define APPFORUM        QStringLiteral ("https://sourceforge.net/p/chart-geany/discussion/")
#define APPEMAIL        QStringLiteral ("chartgeany@gmail.com")
#define APPAUTHOR       QStringLiteral ("Lucas Tsatiris")
#define APPCOPYRIGHT    QStringLiteral ("(C) 2020 Lucas Tsatiris. All rights reserved.")
#define DBNAME          QStringLiteral ("cgbase.dat")
#define DBNAMEBACKUP    QStringLiteral ("cgbase.bak")
#define ENCSTATUS       QStringLiteral ("noenc.status")
#define DBKEY           QStringLiteral ("'Ts@^930&uFs'")

// gcc under os x
#if (defined (Q_OS_MAC) && defined (Q_CC_GNU)) || \
    (defined (Q_CC_GNU) && LONG_TYPE_SIZE<8)
#define ENCKEY           ((quint64) strtoll ("0x5152535455565758", NULL, 16))
#else
#define ENCKEY      0x5152535455565758
#endif // Q_OS_MAC && Q_CC_GNU

#define VERSION_MAJOR       6
#define VERSION_MINOR       1
#define VERSION_PATCH       5
#define LICFILE         QStringLiteral ("license1.txt")
#define LOCKWAIT        2000 // milliseconds
#endif // APPDATA_H
