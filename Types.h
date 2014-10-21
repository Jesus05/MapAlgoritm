#ifndef TYPES_H
#define TYPES_H
#include <cstring>

typedef int Tag;

const int EndArraySize = 256;

typedef int EndArray[EndArraySize];

struct MapObjectStruct
{
  Tag tag;
  char name[256];
  int minHardness;
  int maxHardness;
  EndArray allowList;
  EndArray rejectList;
};

struct MapObjectForAlg : public MapObjectStruct
{
  MapObjectForAlg operator =(MapObjectStruct other)
  {
    tag = other.tag;
    memcpy(name, other.name, sizeof(other.name));
    minHardness = other.minHardness;
    maxHardness = other.maxHardness;
    memcpy((int*)allowList, (int*)other.allowList, sizeof(other.allowList));
    memcpy((int*)rejectList, (int*)other.rejectList, sizeof(other.rejectList));
    chance = 0;
    minCount = 0;
    maxCount = 0;
    havingCount = 0;
    return *this;
  }

  int chance;
  int minCount;
  int maxCount;
  int havingCount;
};

enum Directions
{
  DIR_NO = 0, 
  DIR_UP = 1, 
  DIR_DOWN = 2, 
  DIR_LEFT = 4, 
  DIR_RIGHT = 8, 
  DIR_UP_AND_RIGHT = DIR_UP | DIR_RIGHT, 
  DIR_UP_AND_DOWN = DIR_UP | DIR_DOWN, 
  DIR_UP_AND_LEFT = DIR_UP | DIR_LEFT,
  DIR_DOWN_AND_RIGHT = DIR_DOWN | DIR_RIGHT,
  DIR_DOWN_AND_LEFT = DIR_DOWN | DIR_LEFT,
  DIR_DOWN_AND_UP = DIR_DOWN | DIR_UP,
  DIR_ALL = DIR_DOWN | DIR_UP | DIR_LEFT | DIR_RIGHT
};

struct Pointer
{
  int x;
  int y;
};

struct Room : public Pointer
{
  Room() { exits = DIR_NO; roomType = -1; }
  Directions exits;
  Tag roomType;
};

#endif // TYPES_H
