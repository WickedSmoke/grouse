#pragma once

// we keep clear from Qt's header files so we use int32_t instead of qint32
#include <cstdint>

// core class for object sanitizer
class ObjectSanitizer
{
public:
  ObjectSanitizer (const void *obj) // constructor
  {
    object = obj;
    reset ();
  }

  ~ObjectSanitizer  (); // destructor

  // increase/decrease Array_t counter;
   inline void cgaInc (int32_t c=0) { c==0 ? cgaac++ : cgaac+=c; }
   inline void cgaDec (int32_t c=0) { c==0 ? cgadc++ : cgadc+=c; }
   inline int32_t cgaDiff (void) const
  { return (cgaac - cgadc); }

  // increase/decrease String_t counter;
   inline void cgsInc (int32_t c=0) { c==0 ? cgsac++ : cgsac+=c; }
   inline void cgsDec (int32_t c=0) { c==0 ? cgsdc++ : cgsdc+=c; }
   inline int32_t cgsDiff (void) const
  { return (cgsac - cgsdc); }

  // increase/decrease ObjectHandler_t counter;
   inline void cgoInc (int32_t c=0) { c==0 ? cgoac++ : cgoac+=c; }
   inline void cgoDec (int32_t c=0) { c==0 ? cgodc++ : cgodc+=c; }
   inline int32_t cgoDiff (void) const
  { return (cgoac - cgodc); }

  // increase/decrease Array_t element counter;
   inline void cgaeInc (int32_t c=0) { c==0 ? cgaeac++ : cgaeac+=c; }
   inline void cgaeDec (int32_t c=0) { c==0 ? cgaedc++ : cgaedc+=c; }
   inline int32_t cgaeDiff (void) const
  { return (cgaeac - cgaedc); }

private:
  const void *object;            // referenced QTACObject
  int32_t cgaac;                 // CGScript Array_t alloc count
  int32_t cgadc;                 // CGScript Array_t disalloc count
  int32_t cgsac;                 // CGScript String_t alloc count
  int32_t cgsdc;                 // CGScript String_r disalloc count
  int32_t cgoac;                 // CGScript ObjectHandler_t alloc count
  int32_t cgodc;                 // CGScript ObjectHandler_t disalloc count
  int32_t cgaeac;                // CGScript Array_t element alloc count
  int32_t cgaedc;                // CGScript Array_t element disalloc count

  // reset the counters
   inline void reset (void)
  { cgaac=cgadc=cgsac=cgsdc=cgoac=cgodc=cgaeac=cgaedc=0; }

  // sumup the counters to the parent's sanitizer
  inline void sumup (void);
};