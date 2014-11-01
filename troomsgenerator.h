#ifndef TROOMSGENERATOR_H
#define TROOMSGENERATOR_H
#include "Types.h"

typedef QList <MapObjectForAlg> MapObjAlgList;

class TRoomsGenerator
{
  public:
    TRoomsGenerator();
    ~TRoomsGenerator();
    RoomList DrawLabirint(MapObjAlgList objList, int count);
    static Directions GetRandomExits(int exitCount, Directions having);
    static Directions SelectRandDirection(Directions having);
    static QList<Tag> GetNearestRoomTags(const Room &room, const RoomList &list);
    static MapObjAlgList GetObjListForRoom(const MapObjAlgList &objList, const Room &room, const RoomList &list);
  private:
    RoomList m_rooms;
    Room *m_curRoom;
    int GetRandomExitCount(int havingRooms, int needRooms, int HavingExit = 1/*, int unfinishedRooms = 0*/);
    static int ExitCount(Directions exits);
    static Directions SplitExits(Directions exits[4]);
    int ExitCount(Directions exits[4]);
    void FillNextRoomCoordinate(Directions freeExits, Room *retRoom) const;
    Directions GetFreeExits(Room room) const;
    Directions GetNeighborExits(Room room) const;
    Directions GetRejectNeighborNoExits(Room room) const;
    Room &GetNeighborRooms(Room &room);
    bool IsFreePlace() const;
    Tag GetRandomRoomType(const MapObjAlgList &objList/*, const Room &room, const RoomList &list*/);
};

#endif // TROOMSGENERATOR_H
