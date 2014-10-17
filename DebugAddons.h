#ifndef DEBUGADDONS_H
#define DEBUGADDONS_H
#include "Types.h"

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const EndArray &obj);
#else
QDebug operator<<(QDebug debug, const EndArray &obj);
#endif

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const MapObjectForAlg &obj);
#else
QDebug operator<<(QDebug debug, const MapObjectForAlg &obj);
#endif

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const Directions &obj);
#else
QDebug operator<<(QDebug debug, const Directions &obj);
#endif

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const Room &obj);
#else
QDebug operator<<(QDebug debug, const Room &obj);
#endif



#endif // DEBUGADDONS_H
