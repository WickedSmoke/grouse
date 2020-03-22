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

#include <algorithm>
#include <QString>
#include <QList>
#include "defs.h"

#include "qtachart_object.h"
#include "cgscript.h"

// CGScript string registry record
class StringRegistryRecord
{
public:
  StringRegistryRecord (QString qs = 0, QString ids = 0)
  {
    if (ids == 0)
      id = QStringLiteral ("");
    else
      id = ids;

    if (qs == 0)
      qstr = QStringLiteral ("");
    else
      qstr = qs;

    cstr = nullptr;
  }

  ~StringRegistryRecord ()
  {
    if (cstr != nullptr)
    {
      delete [] cstr;
      cstr = nullptr;

      if (CGSCRIPT_SANITIZER)
      {
        QTACObject *obj = QTACastFromConstVoid (objptr);
        if (obj != nullptr)
          obj->sanitizer->cgsDec ();
      }
    }
  }

  QString qstr;
  QString id;
  char *cstr;
  ObjectHandler_t objptr;
};

// CGScript string type
typedef void * String_t;

// CGScript string registry
typedef QList <StringRegistryRecord *> StringRegistryType;
static THREAD StringRegistryType *StringRegistry = nullptr;

// StringDestroyAll_imp
extern "C" Q_DECL_EXPORT void
StringDestroyAll_imp (const ObjectHandler_t objptr)
{
  if (StringRegistry == nullptr)
    return;

  StringRegistryType::iterator it = StringRegistry->begin();
  while (it != StringRegistry->end())
  {
    if ((*it)->objptr == objptr)
    {
      delete (*it);

      if (CGSCRIPT_SANITIZER)
      {
        QTACObject *obj = QTACastFromConstVoid (objptr);
        if (obj != nullptr)
          obj->sanitizer->cgsDec ();
      }

      it = StringRegistry->erase(it);
    }
    else
      it++;
  }
}

// StrRegister
GNUMALLOC static StringRegistryRecord *
StrRegister (ObjectHandler_t objptr)
{
  StringRegistryRecord *rec = new (std::nothrow) StringRegistryRecord;

  if (rec == nullptr)
    return nullptr;

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj = QTACastFromConstVoid (objptr);
    if (obj != nullptr)
      obj->sanitizer->cgsInc ();
  }

  rec->objptr = objptr;

  if (StringRegistry == nullptr)
  {
    StringRegistry = new (std::nothrow) StringRegistryType;
    if (CGSCRIPT_SANITIZER && StringRegistry)
    {
      QTACObject *obj = QTACastFromConstVoid (objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }
  }

  if (StringRegistry != nullptr)
    StringRegistry->append (rec);
  else
  {
    delete rec;

    if (CGSCRIPT_SANITIZER)
    {
      QTACObject *obj = QTACastFromConstVoid (objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsDec ();
    }

    return nullptr;
  }

  return rec;
}

// StrInit2
extern "C" Q_DECL_EXPORT String_t
StrInit2_imp (const ObjectHandler_t objptr, const char *id, const char *initstr)
{
  StringRegistryRecord *rec = nullptr;

  if (StringRegistry != nullptr)
  {
    foreach (const StringRegistryRecord *r, *StringRegistry)
      if (r->id == id && r->id != QLatin1String ("") &&
          r->objptr == objptr)
        rec = const_cast <StringRegistryRecord *> (r);
  }

  if (rec == nullptr)
    rec = StrRegister (objptr);

  if (rec != nullptr)
  {
    rec->id = QString (id);
    if (initstr != nullptr)
      rec->qstr = QString::fromUtf8 (initstr);
    else
      rec->qstr = QStringLiteral ("");
  }

  return static_cast <String_t> (rec);
}

// StrInit
extern "C" Q_DECL_EXPORT String_t
StrInit_imp (const ObjectHandler_t objptr, const char *initstr)
{
  return StrInit2_imp (objptr, "", initstr);
}

// StringDestroy
extern "C" Q_DECL_EXPORT void
StringDestroy_imp (const ObjectHandler_t objptr, String_t str)
{
  if (StringRegistry == nullptr)
    return;

  if (objptr == nullptr)
    return;

  if (str == nullptr)
    return;

  StringRegistryRecord *s = static_cast <StringRegistryRecord *> (str);

  StringRegistryType::iterator it = StringRegistry->begin();
  while (it != StringRegistry->end())
  {
    if ((*it)->objptr == objptr)
    {
      if ((*it) == s)
      {
        delete (*it);
        it = StringRegistry->erase(it);

        if (CGSCRIPT_SANITIZER)
        {
          QTACObject *obj = QTACastFromConstVoid (objptr);
          if (obj != nullptr)
            obj->sanitizer->cgsDec ();
        }

        return;
      }
    }
    it++;
  }
}

// StrGetWithId
extern "C" Q_DECL_EXPORT String_t
StrGetWithId_imp (const ObjectHandler_t objptr, const char *id)
{
  if (StringRegistry == nullptr)
    return nullptr;

  int counter = 0, sz = StringRegistry->size ();
  while (counter < sz)
  {
    if (StringRegistry->at (counter)->id == id &&
        StringRegistry->at (counter)->objptr == objptr)
      return StringRegistry->at (counter);

    counter ++;
  }

  return nullptr;
}

// StrCpy
extern "C" Q_DECL_EXPORT String_t
StrCpy_imp (String_t dst, String_t src)
{
  if (StringRegistry == nullptr)
  {
    StringRegistry = new (std::nothrow) StringRegistryType;

    if (CGSCRIPT_SANITIZER && StringRegistry)
    {
      StringRegistryRecord *s = static_cast <StringRegistryRecord *> (src);
      QTACObject *obj = QTACastFromConstVoid (s->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }
  }

  if (StringRegistry == nullptr)
    return dst;

  StringRegistryRecord *d, *s;
  int idx;

  d = static_cast <StringRegistryRecord *> (dst);
  s = static_cast <StringRegistryRecord *> (src);

  idx = StringRegistry->indexOf (s);
  if (idx == -1)
    return static_cast <String_t> (d);

  idx = StringRegistry->indexOf (d);
  if (idx == -1)
    d = StrRegister (s->objptr);

  d->qstr = s->qstr;
  return static_cast <String_t> (d);
}

// StrCat
extern "C" Q_DECL_EXPORT String_t
StrCat_imp (String_t dst, String_t src)
{
  if (StringRegistry == nullptr)
  {
    StringRegistry = new (std::nothrow) StringRegistryType;

    if (CGSCRIPT_SANITIZER && StringRegistry)
    {
      StringRegistryRecord *s = static_cast <StringRegistryRecord *> (src);
      QTACObject *obj = QTACastFromConstVoid (s->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }
  }

  if (StringRegistry == nullptr)
    return dst;

  StringRegistryRecord *d, *s;
  int idx1, idx2;

  d = static_cast <StringRegistryRecord *> (dst);
  s = static_cast <StringRegistryRecord *> (src);

  idx2 = StringRegistry->indexOf (s);
  if (idx2 == -1)
    return static_cast <String_t> (d);

  idx1 = StringRegistry->indexOf (d);
  if (idx1 == -1)
    d = StrRegister (s->objptr);

  d->qstr += s->qstr;
  return static_cast <String_t> (d);
}

// StrCmp
extern "C" Q_DECL_EXPORT int
StrCmp_imp (String_t s1, String_t s2)
{
  if (StringRegistry == nullptr)
    return 0;

  StringRegistryRecord *sp1, *sp2;
  int idx1, idx2;

  sp1 = static_cast <StringRegistryRecord *> (s1);
  sp2 = static_cast <StringRegistryRecord *> (s2);

  idx1 = StringRegistry->indexOf (sp1);
  idx2 = StringRegistry->indexOf (sp2);

  if (idx1 == idx2)
    return 0;

  if (idx2 == -1)
    return 1;

  if (idx1 == -1)
    return -1;

  if (sp1->qstr < sp2->qstr)
    return -1;

  if (sp1->qstr == sp2->qstr)
    return 0;

  return 1;
}

// StrCstrCmp
extern "C" Q_DECL_EXPORT int
StrCstrCmp_imp (String_t s1, const char *s2)
{
  if (StringRegistry == nullptr)
    return 0;

  if (s2 == nullptr)
    return 1;

  StringRegistryRecord *sp1;
  int idx1;

  sp1 = static_cast <StringRegistryRecord *> (s1);
  idx1 = StringRegistry->indexOf (sp1);
  if (idx1 == -1)
    return -1;

  if (sp1->qstr < QString(s2))
    return -1;

  if (sp1->qstr == QString(s2))
    return 0;

  return 1;
}

// StrLen
extern "C" Q_DECL_EXPORT size_t
StrLen_imp (String_t str)
{
  if (StringRegistry == nullptr)
    return 0;

  StringRegistryRecord *strp1;
  int idx;

  strp1 = static_cast <StringRegistryRecord *> (str);
  idx = StringRegistry->indexOf (strp1);
  if (idx == -1)
    return 0;

  return strlen (strp1->qstr.toStdString().c_str ());
}

// Cstr2Str
extern "C" Q_DECL_EXPORT String_t
Cstr2Str_imp (ObjectHandler_t objptr, String_t dst, const char *src)
{
  if (StringRegistry == nullptr)
  {
    StringRegistry = new (std::nothrow) StringRegistryType;

    if (CGSCRIPT_SANITIZER)
    {
      QTACObject *obj = QTACastFromConstVoid (objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }
  }

  if (StringRegistry == nullptr)
    return dst;

  StringRegistryRecord *d;
  int idx;

  d = static_cast <StringRegistryRecord *> (dst);

  idx = StringRegistry->indexOf (d);
  if (idx == -1)
    d = StrRegister (objptr);

  d->qstr = QString::fromUtf8 (src);

  return static_cast <String_t> (d);
}

// Str2NCstr
extern "C" Q_DECL_EXPORT char *
Str2NCstr_imp (char *cstr, String_t str, size_t n)
{
  if (StringRegistry == nullptr)
  {
    StringRegistry = new (std::nothrow) StringRegistryType;

    if (CGSCRIPT_SANITIZER && StringRegistry)
    {
      StringRegistryRecord *s = static_cast <StringRegistryRecord *> (str);
      QTACObject *obj = QTACastFromConstVoid (s->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }

  }

  if (StringRegistry == nullptr)
    return nullptr;



  StringRegistryRecord *s;
  int idx;

  s = static_cast <StringRegistryRecord *> (str);

  idx = StringRegistry->indexOf (s);
  if (idx == -1)
  {
    cstr[0] = 0;
    return cstr;
  }

  cstr[n] = 0;
  strncpy (cstr, s->qstr.toUtf8 (), n);
  return cstr;
}

// Str2Cstr
extern "C" Q_DECL_EXPORT const char *
Str2Cstr_imp (String_t str)
{
  if (StringRegistry == nullptr)
    return nullptr;

  StringRegistryRecord *s;
  int idx;

  s = static_cast <StringRegistryRecord *> (str);

  idx = StringRegistry->indexOf (s);
  if (idx == -1)
    return nullptr;

  if (s->cstr != nullptr)
  {
    delete [] s->cstr;
    s->cstr = nullptr;

    if (CGSCRIPT_SANITIZER)
    {
      QTACObject *obj = QTACastFromConstVoid (s->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsDec ();
    }
  }

  s->cstr = new (std::nothrow) char[StrLen_imp (str) + 1];

  if (s->cstr != nullptr)
  {
    strcpy (s->cstr, s->qstr.toUtf8 ());

    if (CGSCRIPT_SANITIZER)
    {
      QTACObject *obj = QTACastFromConstVoid (s->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgsInc ();
    }

    return static_cast <const char *> (s->cstr);
  }

  return nullptr;
}
