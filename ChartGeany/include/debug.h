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

#pragma once

#include <QtGlobal>

#if defined (DEBUG) || defined (Q_OS_WIN)

#include <QDebug>

#ifdef Q_OS_WIN

#define PrintDebug \
qDebug () << __LINE__ << ": "

#else

#define PrintDebug  \
qDebug () << __PRETTY_FUNCTION__ << __LINE__ << ": "

#endif

#else
#include <sstream>

struct NullStream: public std::stringstream
{
   NullStream(): std::stringstream() {}
};

template <typename T>
void operator<<(const NullStream&, const T&) {}

static NullStream nullStream;
static std::ostream &outStream = nullStream;

#define PrintDebug      outStream
#endif





