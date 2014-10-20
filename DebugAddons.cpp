#include <QDebug>
#include <DebugAddons.h>

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const EndArray &obj)
#else
QDebug operator<<(QDebug debug, const EndArray &obj)
#endif
{
    const bool oldSetting = debug.autoInsertSpaces();
    for (int i = 0; i < EndArraySize && obj[i] != -1; i++)
    {
      debug << obj[i] << ", ";
    }
    debug.setAutoInsertSpaces(oldSetting);
    return debug.maybeSpace();
}

#if defined(FORCE_UREF)
QDebug &operator<<(QDebug debug, const MapObjectForAlg &obj)
#else
QDebug operator<<(QDebug debug, MapObjectForAlg const &obj)
#endif
{
    const bool oldSetting = debug.autoInsertSpaces();
    debug << "\r\nMapObjectForAlg:";
    debug << "\r\n Tag: " << obj.tag;
    debug << "\r\n Name: " << obj.name;
    debug << "\r\n MinHrdness: " << obj.minHardness;
    debug << "\r\n MaxHrdness: " << obj.maxHardness;
    debug << "\r\n AllowList: " << obj.allowList;
    debug << "\r\n RejectList: " << obj.rejectList;
    debug << "\r\n Chance: " << obj.chance;
    debug << "\r\n MinCount: " << obj.minCount;
    debug << "\r\n MaxCount: " << obj.maxCount;
    debug << "\r\n HavingCount: " << obj.havingCount;
    debug << "\r\n\r\n";
    debug.setAutoInsertSpaces(oldSetting);
    return debug.maybeSpace();
}


QDebug operator<<(QDebug debug, const Directions &obj)
{
  const bool oldSetting = debug.autoInsertSpaces();
  debug << ("[");
  (obj & DIR_UP) ? debug << "UP" : debug;
  (obj & DIR_DOWN) ? debug << "DOWN" : debug;
  (obj & DIR_LEFT) ? debug << "LEFT" : debug;
  (obj & DIR_RIGHT) ? debug << "RIGHT" : debug;
  debug << " value: " << (int)obj;
  debug << ("]");
  debug.setAutoInsertSpaces(oldSetting);
  return debug.maybeSpace();
}


QDebug operator<<(QDebug debug, const Room &obj)
{
  const bool oldSetting = debug.autoInsertSpaces();
  debug << "#";
  debug << "x:" << obj.x;
  debug << "y:" << obj.y;
  debug << "type:" << obj.roomType;
  debug << "exits:" << obj.exits;
  debug << "#";
  debug.setAutoInsertSpaces(oldSetting);
  return debug.maybeSpace();
}
