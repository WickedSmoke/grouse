﻿/*
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

#ifndef DEFS_H
#define DEFS_H

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
#error "Unsupported Qt Version. Supported versions are 5.3.0 or above"
#endif

// Is nullptr supported?
#if __cplusplus < 201103L
#ifndef Q_OS_MAC
#define nullptr     NULL
#endif
#endif

// Is noexcept supported?
#ifndef NOEXCEPT
#if \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46) || \
    (defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 180021114)
#define NOEXCEPT noexcept
#else
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > 160040219
#define NOEXCEPT _NOEXCEPT
#else
#define NOEXCEPT
#endif
#endif
#endif

#include <QtGlobal>
#include <QMetaType>
#include <QThread>
#include <QStringList>
#include <QColor>
#include "appdata.h"
#include "sqlite3.h"
#include "idb.h"

#define TICKER_HEIGHT        35

// minimum and maximum qreal values
#define QREAL_MAX   (std::numeric_limits<qreal>::max())
#define QREAL_MIN   (std::numeric_limits<qreal>::min())

#ifdef  __clang__
#ifndef Q_CC_CLANG
#define Q_CC_CLANG  1
#endif
#endif

#ifdef Q_OS_MAC
#define THREAD
#else
#ifdef Q_CC_MSVC
#define THREAD  __declspec(thread)
#else
#define THREAD  __thread
#endif
#endif

// fastcall, hot
#if defined(__i386__) || defined(_M_IX86)
#ifdef Q_CC_GNU
#define GNUFASTCALL __attribute__((fastcall))
#ifdef Q_OS_MAC
#define GNUHOT
#define GNUCOLD
#else
#define GNUHOT      __attribute__((hot))
#define GNUCOLD     __attribute__((cold))
#endif // Q_OS_MAC
#define MSVCFASTCALL
#else
#define GNUFASTCALL
#define GNUHOT
#define GNUCOLD
#define MSVCFASTCALL __fastcall
#endif
#else // !(defined(__i386__) || defined(_M_IX86))
#define GNUFASTCALL
#define GNUHOT
#define GNUCOLD
#define MSVCFASTCALL
#endif

// GCC specific attributes
#ifdef Q_CC_GNU
#define GNUMALLOC       __attribute__((malloc))
#else
#define GNUMALLOC
#endif // Q_CC_GNU

// export
#ifndef Q_DECL_EXPORT
#ifdef Q_OS_WIN
#define Q_DECL_EXPORT       __declspec(dllexport)
#else
#define Q_DECL_EXPORT
#endif
#endif // Q_DECL_EXPORT

// module extensions
#ifdef Q_OS_WIN
#define SOEXT       QString (".dll")
#else
#define SOEXT       QString (".so")
#endif

// alignas(n) for visual studio 2010 C++
#ifdef Q_OS_WIN

#ifdef _MSC_VER
#if _MSC_VER<=1600
#define alignas(n)
#define max_align_t     4
#endif
#endif

#endif

// CGScript sanitizer enabled
#ifdef CGSCRIPT_SANITIZE
#ifndef CGSCRIPT_SANITIZER
#define CGSCRIPT_SANITIZER      true
#endif
#else
#ifndef CGSCRIPT_SANITIZER
#define CGSCRIPT_SANITIZER      false
#endif
#endif

// base 10 logarithm
#define qln10       2.302585f // qLn((qreal)10)
#define qLog10(v)   ((qreal)qLn((qreal)v)/qln10)

// convert QString to UTF8
#define TO_UTF8(s)  QString().fromUtf8(s.toStdString ().c_str())
#define TO_CSTR(s)  s.toUtf8().constData()

// Module initialization function
typedef int (*ModuleInit)(void *, int, void *, int *, void *, void *);

// Module loop function
typedef int (*ModuleLoop)(void);

// Module event function
typedef void (*ModuleEvent)(int);

// Module finish function
typedef void (*ModuleFinish)(void);

// Module's set of values
typedef void * (*ModuleValueSet)(void);

// Module's deactivation function
typedef void * (*ModuleDeactivate) (const char *, int);

// Module's compiler report
typedef const char * (*ModuleCompiler) (void);

#include "errors.h"

// toolchain record type
typedef struct
{
  QString platform;
  QString compiler;
  QString compilerdbg;
  QString compilerrel;
  QString linker;
  QString linkerdbg;
  QString linkerrel;
} ToolchainRec;

Q_DECLARE_TYPEINFO (ToolchainRec, Q_MOVABLE_TYPE);
typedef QVector < ToolchainRec * > ToolchainVector;


extern QString Year, Month, Day;

// load a csv file to sqlite
// operation may be "CREATE" or "UPDATE"
extern CG_ERR_RESULT
csv2sqlite (SymbolEntry *data, QString operation);

// Form an SQL INSERT command from csvline
// Return "" on fail
extern const QString
csvline2SQL (QString &csvline, QString &tablename);

// sqlite3_exec callback for retrieving toolchain path and options
extern int
sqlcb_toolchain (void *classptr, int argc, char **argv, char **column);

extern int
sqlcb_toolchains (void *classptr, int argc, char **argv, char **column);


// TOOLTIP
// #define TOOLTIP  QString ("<span style=\"background-color:black; color: white; font: 11px;\">")
#define TOOLTIP  QStringLiteral ("<span style=\"background-color:black; color: white; font: 11px;\">")

// Sleeper Class
class Sleeper : public QThread
{
  Q_OBJECT

public:
  Sleeper (QObject *parent = nullptr)
  {
    if (parent != nullptr)
      setParent (parent);
  }

  static void usleep(unsigned long usecs)
  {
    QThread::usleep(usecs);
  }
  static void msleep(unsigned long msecs)
  {
    QThread::msleep(msecs);
  }
  static void sleep(unsigned long secs)
  {
    QThread::sleep(secs);
  }
};

#endif // DEFS_H
