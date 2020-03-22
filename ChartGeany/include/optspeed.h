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

#ifndef OPTSPEED_H
#define OPTSPEED_H

#include <QtGlobal>

#ifndef DEBUG
#ifdef Q_CC_GNU
#ifndef Q_CC_INTEL

#if defined(__GNUC__) && (__GNUC__ > 4)
#ifdef __OPTIMIZE__
#pragma GCC optimize ("O3", "omit-frame-pointer")
#else
#pragma GCC optimize ("O3")
#endif /* __OPTIMIZE__ */
#endif /* defined(__GNUC__) && (__GNUC__ > 4) */

#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__>3)
#ifdef __OPTIMIZE__
#pragma GCC optimize ("O3", "omit-frame-pointer")
#else
#pragma GCC optimize ("O3")
#endif /* __OPTIMIZE__ */
#endif /* defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__>3) */

#endif /* !Q_CC_INTEL */
#endif /* Q_CC_GNU  */
#endif /* DEBUG */

#endif /* OPTSPEED_H */
