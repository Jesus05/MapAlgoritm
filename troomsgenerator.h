#ifndef TROOMSGENERATOR_H
#define TROOMSGENERATOR_H
#include "Types.h"
#include <QList>

typedef QList <MapObjectForAlg> MapObjAlgList;
typedef QList<class Room> RoomList;

class TRoomsGenerator
{
  public:
    TRoomsGenerator();
    static RoomList DrawLabirint(MapObjAlgList objList, int count);
    static Directions GetRandomExits(int exitCount, Directions having);
    static Directions SelectRandDirection(Directions having);
    static QList<Tag> GetNearestRoomTags(const Room &room, const RoomList &list);
    static MapObjAlgList GetObjListForRoom(const MapObjAlgList &objList, const Room &room, const RoomList &list);
  private:
    static int GetRandomExitCount(int havingRooms, int needRooms, int HavingExit = 1/*, int unfinishedRooms = 0*/);
    static int ExitCount(Directions exits);
    static Directions SplitExits(Directions exits[4]);
    static int ExitCount(Directions exits[4]);
    static Room FillNextRoomCoordinate(Room room, Directions freeExits);
    static Directions GetFreeExits(Room room, const RoomList list);
    static Directions GetNeighborExits(Room room, const RoomList &list);
    static Directions GetRejectNeighborNoExits(Room room, const RoomList &list);
    static bool IsFreePlace(const Room &room, const RoomList &list);
    static Tag GetRandomRoomType(const MapObjAlgList &objList/*, const Room &room, const RoomList &list*/);
};

#endif // TROOMSGENERATOR_H
