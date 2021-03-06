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

#ifndef DYNPARAM_H
#define DYNPARAM_H

#include <QString>
#include <QVector>

// paramenter types
typedef enum
{
  DPT_INT,      // integer input mask
  DPT_REAL,     // real input mask
  DPT_COLOR     // color dialog
} PARAM_TYPES;

// dynamic parameter
class DynParam
{
public:
  explicit DynParam (const QString& name)
  {
    label = paramName = name;
    callback_var = NULL;
    show = false;
  }

  QString paramName;    // parameter's name
  QString label;        // parameter's label
  qint32  type;         // parameter's type
  qreal defvalue;       // parameter's default value
  qreal value;          // parameter's current value
  bool show;            // parameter's show flag
  void *callback_var;   // parameter's callback variable (default NULL if none)
};

class ParamVector : public QVector <DynParam *>
{
public:
   ParamVector() {}
   ~ParamVector();
   DynParam* addParameter( const QString& name, qint32 type, qreal value );
   const DynParam* constParameter( const QString& name ) const;
   DynParam* parameter( const QString& name )
   {
       return const_cast<DynParam*>(constParameter(name));
   }

private:
    // Disabled copy constructor and operator=
    ParamVector( const ParamVector & ) : QVector <DynParam *>() {}
    ParamVector &operator=( const ParamVector & ) { return *this; }
};

#define FOREACH_PARAM(it,pv)    for(it = pv.begin(); it != pv.end(); ++it)


#endif // DYNPARAM_H
