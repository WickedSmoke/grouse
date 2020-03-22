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
#include <cstring>
#include <QString>  // needed for early versions of Qt5
#include <QList>
#include <QString>

#include "qtachart_object.h"
#include "cgscript.h"

using std::unique_ptr;

// Array type
typedef struct alignas (max_align_t)
{
public:	
  QString id;               // array id
  QList <void *> element;   // elements of the array
  quint32 esize;            // element size in bytes
  quint16 maxdim;           // maximum dimension
  char reserved[10];        // reserved for alignment
} Array;
Q_DECLARE_TYPEINFO (Array, Q_MOVABLE_TYPE);

// Array registry record
typedef struct alignas (max_align_t)
{
  Array *array;         // the array itselef
  const void *objptr;   // _THIS
} ArrayRegistryRecord;
Q_DECLARE_TYPEINFO (ArrayRegistryRecord, Q_PRIMITIVE_TYPE);

// Array registry
typedef QList <ArrayRegistryRecord *> ArrayRegistryType;
static THREAD ArrayRegistryType *ArrayRegistry = nullptr;

// Register array
static void
RegisterArray (const void *objptr, const Array *array)
{
  ArrayRegistryRecord *rec = new (std::nothrow) ArrayRegistryRecord;

  if (rec == nullptr)
  {
    ArrayDestroy_imp (static_cast <void *> (const_cast <Array *> (array)));
    return;
  }

  rec->array = const_cast <Array *> (array);
  rec->objptr = objptr;

  if (ArrayRegistry == nullptr)
    ArrayRegistry = new (std::nothrow) ArrayRegistryType;

  if (ArrayRegistry != nullptr)
  {
    ArrayRegistry->append (rec);

    if (CGSCRIPT_SANITIZER)
    {
      QTACObject *obj = QTACastFromConstVoid (rec->objptr);
      if (obj != nullptr)
        obj->sanitizer->cgaInc ();
    }

  }
  else
  {
    ArrayDestroy_imp (static_cast <void *> (const_cast <Array *> (array)));
    delete rec;
  }
}

// Array reset
extern "C" Q_DECL_EXPORT void
ArrayReset_imp (void *arrptr)
{
  if (arrptr == nullptr)
    return;

  Array *arr = static_cast <Array *> (arrptr);
  int arrsize = arr->element.size ();

  foreach (void *ptr, arr->element)
    ::operator delete (ptr);
  arr->element.clear ();

  void *nelem = ::operator new (arr->esize,std::nothrow);
  if (nelem == nullptr)
    return;

  std::memset (nelem, 0, arr->esize);
  arr->element.append (nelem);

  if (CGSCRIPT_SANITIZER)
  {
    foreach (ArrayRegistryRecord *r, *ArrayRegistry)
    {
      if (r->array == arr)
      {
        QTACObject *obj = QTACastFromConstVoid (r->objptr);
        if (obj != nullptr)
        {
          obj->sanitizer->cgaeInc ();
          obj->sanitizer->cgaeDec (arrsize);
        }
        return;
      }
    }
  }
}

// Array Create 2
extern "C" Q_DECL_EXPORT void *
ArrayCreate2_imp (const void *objptr, const char *id, int elemsize, int maxd)
{
  Array *arr = nullptr;

  if (ArrayRegistry != nullptr)
  {
    foreach (const ArrayRegistryRecord *r, *ArrayRegistry)
      if (r->array->id == id && r->array->id != QLatin1String (""))
        arr = r->array;
  }

  if (arr != nullptr)
  {
    ArrayReset_imp (arr);
    return arr;
  }

  arr = new (std::nothrow) Array;
  if (arr == nullptr)
    return nullptr;

  if (maxd <= 0)
    maxd = 65535;

  arr->esize = static_cast <quint32> (elemsize);
  arr->maxdim = static_cast <quint16> (maxd + 1);
  arr->element.reserve (arr->maxdim);
  arr->id = QString (id);
  
  void *nelem = ::operator new (arr->esize,std::nothrow);
  if (nelem == nullptr)
  {
    ArrayDestroy_imp (arr);
    return nullptr;
  }

  if (CGSCRIPT_SANITIZER)
  {
    QTACObject *obj = QTACastFromConstVoid (objptr);
    if (obj != nullptr)
      obj->sanitizer->cgaeInc ();
  }

  std::memset (nelem, 0, arr->esize);
  arr->element.append (nelem);

  RegisterArray (objptr, arr);

  return static_cast <void *> (arr);
}

// Array Create
extern "C" Q_DECL_EXPORT void *
ArrayCreate_imp (void *objptr, int elemsize, int maxd)
{
  return ArrayCreate2_imp (objptr, "", elemsize, maxd);
}

// Put element in array
extern "C" Q_DECL_EXPORT int
ArrayAppend_imp (void *arrptr, void *elem)
{
  if (arrptr == nullptr)
    return -1;

  Array *arr = static_cast <Array *> (arrptr);

  if (arr->element.size () == arr->maxdim)
    return -1;

  void *nelem = ::operator new (arr->esize,std::nothrow);

  if (nelem == nullptr)
    return -1;

  std::memcpy (nelem, elem, arr->esize);
  arr->element.append (nelem);

  if (CGSCRIPT_SANITIZER)
  {
    ArrayRegistryType::iterator it = ArrayRegistry->begin();
    while ((*it)->array != arr && it != ArrayRegistry->end ()) it++;
    if ((*it)->array == arr)
    {
      QTACObject *obj = QTACastFromConstVoid ((*it)->objptr);
      if (obj != nullptr)
         obj->sanitizer->cgaeInc ();
    }
  }

  return static_cast <int> (arr->element.size () - 2);
}

// Put element array's position
extern "C" Q_DECL_EXPORT void
ArrayPut_imp (void *arrptr, int idx, void *elem)
{
  if (arrptr == nullptr)
    return;

  Array *arr = static_cast <Array *> (arrptr);

  if (arr->element.size () < (idx + 3))
    return;

  if (idx < 0)
    return;

  std::memcpy (arr->element[idx + 1], elem, arr->esize);
  return;
}

// Get element from array
extern "C" Q_DECL_EXPORT void *
ArrayGet_imp (void *arrptr, int index)
{
  if (arrptr == nullptr)
    return nullptr;

  Array *arr = static_cast <Array *> (arrptr);

  if (arr->element.size () == 1)
    return arr->element.at (0);

  if (index < 0)
    return arr->element.at (1);

  index ++;

  if (!(index < arr->element.size ()))
    index = arr->element.size () - 1;

  return arr->element.at (index);
}

// Get array size
extern "C" Q_DECL_EXPORT int
ArraySize_imp (void *arrptr)
{
  if (arrptr == nullptr)
    return 0;

  Array *arr = static_cast <Array *> (arrptr);

  return static_cast <int> (arr->element.size () - 1);
}

// Get index of element
extern "C" Q_DECL_EXPORT int
ArrayGetIdx_imp (void *arrptr, void *elem)
{
  if (arrptr == nullptr)
    return -1;

  int counter = 0,
      idx = -1,
      arrsize = ArraySize_imp (arrptr);

  while (counter < arrsize)
  {
    if (Q_UNLIKELY (elem == ArrayGet_imp (arrptr, counter)))
    {
      idx = counter;
      counter = arrsize;
    }
    else
      counter ++;
  }

  return idx;
}

// Array destroy
extern "C" Q_DECL_EXPORT void
ArrayDestroy_imp (void *arrptr)
{
  if (arrptr == nullptr)
    return;

  Array *arr = static_cast <Array *> (arrptr);
  int arrsize = arr->element.size ();

  foreach (void *ptr, arr->element)
    ::operator delete (ptr);
  arr->element.clear ();

  delete arr;

  foreach (ArrayRegistryRecord *r, *ArrayRegistry)
  {
    if (r->array == arr)
    {
      if (CGSCRIPT_SANITIZER)
      {
        QTACObject *obj = QTACastFromConstVoid (r->objptr);
        if (obj != nullptr)
        {
          obj->sanitizer->cgaDec ();
          obj->sanitizer->cgaeDec (arrsize);
        }
      }

      delete r;
      ArrayRegistry->removeOne (r);

      return;
    }
  }
}

// Destroy all arrays of object
extern "C" Q_DECL_EXPORT void
ArrayDestroyAll_imp (const void *objptr)
{
  if (ArrayRegistry == nullptr)
    return;

  foreach (ArrayRegistryRecord *rec, *ArrayRegistry)
    if (rec->objptr == objptr)
      ArrayDestroy_imp (rec->array);
}

// Check array existense
extern "C" Q_DECL_EXPORT int
ArrayExists_imp (const void *arrptr)
{
  if (arrptr == nullptr)
    return 0;

  const Array *arr = static_cast <const Array *> (arrptr);

  if (ArrayRegistry != nullptr)
  {
    foreach (const ArrayRegistryRecord *r, *ArrayRegistry)
      if (r->array == arr)
        return 1;
  }

  return 0;
}
