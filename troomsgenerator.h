#ifndef TROOMSGENERATOR_H
#define TROOMSGENERATOR_H
#include "Types.h"

class TRoomsGenerator
{
  public:
    TRoomsGenerator();
  private:
    int GetRandomExitCount(int havingRooms, int needRooms, int HavingExit = 1/*, int unfinishedRooms = 0*/);
    int ExitCount(Directions exits);
    Directions SplitExits(Directions exits[4]);
    int ExitCount(Directions exits[4]);
    Directions SelectRandDirection(Directions having);
    Directions GetRandomExits(int exitCount, Directions having);
};

#endif // TROOMSGENERATOR_H
