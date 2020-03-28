#include "qtachart_object_sanitizer.h"
#include "qtachart_object.h"
#include "debugdialog.h"

extern DebugDialog *debugdialog;

ObjectSanitizer::~ObjectSanitizer (void)
{
  sumup ();
}

// sumup the counters to the parent's sanitizer
void
ObjectSanitizer::sumup (void)
{
  QTACObject *obj =
    static_cast <QTACObject *> (const_cast <void *> (object));

  if (CGSCRIPT_SANITIZER && obj->cgscriptdebug)
  {
    QString type = obj->modinit==nullptr ? "Object: " : "Module: ";
    QString dbg =
      "\n" + type + obj->objectName () +
      " sanitizer report:" + "\n" +
      "Array_t: alloc " + QString::number (cgaac) +
      " delloc " + QString::number (cgadc) +
      " leak " + QString::number (cgaDiff ()) + "\n" +

      "String_t: alloc " + QString::number (cgsac) +
      " dealloc " + QString::number (cgsdc) +
      " leak " + QString::number (cgsDiff ()) + "\n" +

      "ObjectHandler_t: alloc " + QString::number (cgoac) +
      " dealloc " + QString::number (cgodc) +
      " leak " + QString::number (cgoDiff ()) + "\n" +

      "Array_t elements: alloc " + QString::number (cgaeac) +
      " dealloc " + QString::number (cgaedc) +
      " leak " + QString::number (cgaeDiff ());

    debugdialog->appendText (dbg);
  }

  if (obj->modinit != nullptr)
    return;

  if  (obj->parentObject == nullptr)
    return;

  ObjectSanitizer *san = obj->parentObject->sanitizer;

  // sumup Array_t
  if (cgaac != 0) san->cgaInc (cgaac);
  if (cgadc != 0) san->cgaDec (cgadc);

  // sumup String_t
  if (cgsac != 0) san->cgsInc (cgsac);
  if (cgsac != 0) san->cgsDec (cgsdc);

  // sumup ObjectHandler_t
  if (cgoac != 0) san->cgoInc (cgoac);
  if (cgoac != 0) san->cgoDec (cgodc);

  // sumup Array_t elements
  if (cgaeac != 0) san->cgaeInc (cgaeac);
  if (cgaeac != 0) san->cgaeDec (cgaedc);

  reset ();
}