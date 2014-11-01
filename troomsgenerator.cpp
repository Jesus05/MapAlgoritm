#include "troomsgenerator.h"
#include "trandomizer.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QStack>
#include <QQueue>

TRoomsGenerator::TRoomsGenerator()
{
}

TRoomsGenerator::~TRoomsGenerator()
{
  RoomList::iterator it;

  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    Room *curRoom = *it;
    delete curRoom;
  }
  m_rooms.clear();
}

int TRoomsGenerator::GetRandomExitCount(int havingRooms, int needRooms, int HavingExit)
{
  if (havingRooms > needRooms) return HavingExit;

  int rand = qrand();
  int min = HavingExit + 1;
  int max = 3;

  double randPos = (double)rand / RAND_MAX;
  double roomPos = (double)havingRooms / needRooms;
  double roomCutMin = 0.8;
  double roomCutMax = 1.0;
  roomPos = roomCutMin + roomPos*(roomCutMax-roomCutMin); //Обрежем диапазон до roomCutMin ... roomCutMax
  double resultPos = randPos * roomPos;
  //TODO поиграться с формулами зависимости отсавшихся комнат и рандома.
  int result = max - (int)((double)(max - min + 1) * resultPos);

//  qDebug() << "RandomExitCount:" << havingRooms << needRooms << resultPos << randPos << roomPos << (double)(max - min + 1) * resultPos << (int)((double)(max - min + 1) * resultPos) << result;

  return result;
}

int TRoomsGenerator::ExitCount(Directions exits)
{
  int having = 0;

  if (exits & DIR_UP) having++;
  if (exits & DIR_DOWN) having++;
  if (exits & DIR_LEFT) having++;
  if (exits & DIR_RIGHT) having++;

  return having;
}

Directions TRoomsGenerator::SplitExits(Directions exits[])
{
  Directions splitExits = DIR_NO;

  for (int i = 0; i < 4; i++)
  {
    splitExits = (Directions)(splitExits | exits[i]);
  }

  return splitExits;
}

int  TRoomsGenerator::ExitCount(Directions exits[4])
{
  return ExitCount(SplitExits(exits));
}

Directions TRoomsGenerator::SelectRandDirection(Directions having)
{
  Directions randomExits[4] = {DIR_NO, DIR_NO, DIR_NO, DIR_NO};
  int randomIndex = 0;

  #define ADD_DIR(dir) if (having & dir) randomExits[randomIndex++] = dir;

  ADD_DIR(DIR_DOWN)
  ADD_DIR(DIR_UP)
  ADD_DIR(DIR_RIGHT)
  ADD_DIR(DIR_LEFT)

  #undef ADD_DIR

  Directions result = DIR_NO;

  while (ExitCount(result) < 1)
  {
    randomIndex = TRandomizer::GetRandom(0, 4);
    if (qrand() % 2) result = (Directions)(result | randomExits[randomIndex]);
  }

  return result;
}

Directions TRoomsGenerator::GetRandomExits(int exitCount, Directions having)
{
  //  qDebug() << "having" << having;

    if (ExitCount(having) >= exitCount) //Если все ходы уже есть то возвращаем все ходы.
    {
  //    qDebug() << "GetRandomExit";
      return having;
    }

    if (exitCount > 4) //Максимально может быть 4 выхода, если запросили больше ошибка.
    {
      return DIR_NO;
    }

    Directions randomExits[4] = {DIR_NO, DIR_NO, DIR_NO, DIR_NO};
    int randomIndex = 0;
    {  /* Корридорность - предпочтительный выход напротив входа */
      if (having & DIR_UP && !(having & DIR_DOWN)) randomExits[randomIndex++] = DIR_DOWN;
      if (having & DIR_DOWN && !(having & DIR_UP)) randomExits[randomIndex++] = DIR_UP;
      if (having & DIR_LEFT && !(having & DIR_RIGHT)) randomExits[randomIndex++] = DIR_RIGHT;
      if (having & DIR_RIGHT && !(having & DIR_LEFT)) randomExits[randomIndex++] = DIR_LEFT;
    }
    Directions selected = SplitExits(randomExits);

    #define ADD_DIR(dir) if (!(selected & dir) && !(having & dir)) randomExits[randomIndex++] = dir;

    ADD_DIR(DIR_DOWN)
    ADD_DIR(DIR_UP)
    ADD_DIR(DIR_RIGHT)
    ADD_DIR(DIR_LEFT)

    #undef ADD_DIR

    Directions result = having;
  //  qDebug() << "result" << result;

    while (ExitCount(result) < exitCount)
    {
      randomIndex = TRandomizer::GetRandom(0, 3);
      if (qrand() % 2) result = (Directions)(result | randomExits[randomIndex]);
  //    qDebug() << "result loop" << result << randomIndex << randomExits[randomIndex];
    }

    return result;
}

void TRoomsGenerator::FillNextRoomCoordinate(Directions freeExits, Room *retRoom) const
{
  Room *result = retRoom;
  Directions temp = SelectRandDirection(freeExits);
  
  if (temp & DIR_UP) { result->y++; result->exits = DIR_DOWN; }
  if (temp & DIR_DOWN) { result->y--; result->exits = DIR_UP; }
  if (temp & DIR_LEFT) { result->x--; result->exits = DIR_RIGHT; }
  if (temp & DIR_RIGHT) { result->x++; result->exits = DIR_LEFT; }
}

Directions TRoomsGenerator::GetFreeExits(Room room) const
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "GetFreeExits"
  RoomList::const_iterator it;
//  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
//  qDebug() << "StartFreeExits: " << result;
  
  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    const Room *curRoom = *it;
    if (room.exits & DIR_UP && curRoom->x == room.x && (curRoom->y-1) == room.y) result = (Directions)(result & ~DIR_UP);
    if (room.exits & DIR_DOWN && curRoom->x == room.x && (curRoom->y+1) == room.y) result = (Directions)(result & ~DIR_DOWN);
    if (room.exits & DIR_LEFT && (curRoom->x+1) == room.x && curRoom->y == room.y) result = (Directions)(result & ~DIR_LEFT);
    if (room.exits & DIR_RIGHT && (curRoom->x-1) == room.x && curRoom->y == room.y) result = (Directions)(result & ~DIR_RIGHT);
//    qDebug() << "UP:" << (room.exits & DIR_UP) << (curRoom.x == room.x) << ((curRoom.y-1) == room.y) << (room.exits & DIR_UP && curRoom.x == room.x && (curRoom.y-1) == room.y);
//    qDebug() << "DOWN:" << (room.exits & DIR_DOWN) << (curRoom.x == room.x) << ((curRoom.y+1) == room.y) << (room.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y+1) == room.y);
//    qDebug() << "LEFT:" << (room.exits & DIR_LEFT) << ((curRoom.x+1) == room.x) << (curRoom.y == room.y) << (room.exits & DIR_LEFT && (curRoom.x+1) == room.x && curRoom.y == room.y);
//    qDebug() << "RIGHT:" << (room.exits & DIR_RIGHT) << ((curRoom.x-1) == room.x) << (curRoom.y == room.y) << (room.exits & DIR_RIGHT && (curRoom.x-1) == room.x && curRoom.y == room.y);
//    qDebug() << curRoom << result;
  }
  
//  qDebug() << "EndFreeExits: " << result;
  
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
  #undef FUNC_NAME
  
  return result;
}

Directions TRoomsGenerator::GetNeighborExits(Room room) const
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "GetNeighborExits"
  RoomList::const_iterator it;
//  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
//  qDebug() << "GetNeighborExits: " << result;
  
  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    const Room *curRoom = *it;
    if (curRoom->exits & DIR_DOWN && curRoom->x == room.x && (curRoom->y-1) == room.y) result = (Directions)(result | DIR_UP);
    if (curRoom->exits & DIR_UP && curRoom->x == room.x && (curRoom->y+1) == room.y) result = (Directions)(result | DIR_DOWN);
    if (curRoom->exits & DIR_RIGHT && (curRoom->x+1) == room.x && curRoom->y == room.y) result = (Directions)(result | DIR_LEFT);
    if (curRoom->exits & DIR_LEFT && (curRoom->x-1) == room.x && curRoom->y == room.y) result = (Directions)(result | DIR_RIGHT);
//    qDebug() << "UP:" << (curRoom.exits & DIR_DOWN) << (curRoom.x == room.x) << ((curRoom.y-1) == room.y) << (curRoom.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y-1) == room.y);
//    qDebug() << "DOWN:" << (curRoom.exits & DIR_UP) << (curRoom.x == room.x) << ((curRoom.y+1) == room.y) << (curRoom.exits & DIR_UP && curRoom.x == room.x && (curRoom.y+1) == room.y);
//    qDebug() << "LEFT:" << (curRoom.exits & DIR_RIGHT) << ((curRoom.x+1) == room.x) << (curRoom.y == room.y) << (curRoom.exits & DIR_RIGHT && (curRoom.x+1) == room.x && curRoom.y == room.y);
//    qDebug() << "RIGHT:" << (curRoom.exits & DIR_LEFT) << ((curRoom.x-1) == room.x) << (curRoom.y == room.y) << (curRoom.exits & DIR_LEFT && (curRoom.x-1) == room.x && curRoom.y == room.y);
//    qDebug() << curRoom << result;
  }
  
//  qDebug() << "GetNeighborExits: " << result;
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
  #undef FUNC_NAME
  return result;
}

Directions TRoomsGenerator::GetRejectNeighborNoExits(Room room) const
{
  QElapsedTimer time;
  time.start();
#define FUNC_NAME "GetRejectNeighborNoExits"
  RoomList::const_iterator it;
  //  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
  //  qDebug() << "GetNeighborRejectExits: " << result;
  
  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    const Room *curRoom = *it;
    if ((curRoom->exits & DIR_DOWN) == 0 && curRoom->x == room.x && (curRoom->y-1) == room.y) result = (Directions)(result & ~DIR_UP);
    if ((curRoom->exits & DIR_UP) == 0 && curRoom->x == room.x && (curRoom->y+1) == room.y) result = (Directions)(result & ~DIR_DOWN);
    if ((curRoom->exits & DIR_RIGHT) == 0 && (curRoom->x+1) == room.x && curRoom->y == room.y) result = (Directions)(result & ~DIR_LEFT);
    if ((curRoom->exits & DIR_LEFT) == 0 && (curRoom->x-1) == room.x && curRoom->y == room.y) result = (Directions)(result & ~DIR_RIGHT);
    //    qDebug() << "UP:" << ((curRoom.exits & DIR_DOWN) == 0 && curRoom.x == room.x && (curRoom.y-1) == room.y) << ((curRoom.exits & DIR_DOWN) == 0 && curRoom.x == room.x && (curRoom.y-1) == room.y);
    //    qDebug() << "DOWN:" << ((curRoom.exits & DIR_UP) == 0 && curRoom.x == room.x && (curRoom.y+1) == room.y) << ((curRoom.exits & DIR_UP) == 0 && curRoom.x == room.x && (curRoom.y+1) == room.y);
    //    qDebug() << "LEFT:" << ((curRoom.exits & DIR_RIGHT) == 0 && (curRoom.x+1) == room.x && curRoom.y == room.y) << ((curRoom.exits & DIR_RIGHT) == 0 && (curRoom.x+1) == room.x && curRoom.y == room.y);
    //    qDebug() << "RIGHT:" << ((curRoom.exits & DIR_LEFT) == 0 && (curRoom.x-1) == room.x && curRoom.y == room.y) << ((curRoom.exits & DIR_LEFT) == 0 && (curRoom.x-1) == room.x && curRoom.y == room.y);
    //    qDebug() << curRoom << result;
  }
  
  //  qDebug() << "GetNeighborRejectExits: " << result;
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
#undef FUNC_NAME
  
  return result;
}

/*Room &TRoomsGenerator::GetNeighborRooms(Room &room)
{
  QElapsedTimer time;
  time.start();
#define FUNC_NAME "GetNeighborRooms"
  RoomList::iterator it;
  //  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();

  //  qDebug() << "GetNeighborRejectExits: " << result;

  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    Room &curRoom = *it;
    if (curRoom.x == room.x && (curRoom.y-1) == room.y) { room.up = &curRoom;
    if (curRoom.x == room.x && (curRoom.y+1) == room.y)
    if ((curRoom.x+1) == room.x && curRoom.y == room.y)
    if ((curRoom.x-1) == room.x && curRoom.y == room.y)
    //    qDebug() << "UP:" << ((curRoom.exits & DIR_DOWN) == 0 && curRoom.x == room.x && (curRoom.y-1) == room.y) << ((curRoom.exits & DIR_DOWN) == 0 && curRoom.x == room.x && (curRoom.y-1) == room.y);
    //    qDebug() << "DOWN:" << ((curRoom.exits & DIR_UP) == 0 && curRoom.x == room.x && (curRoom.y+1) == room.y) << ((curRoom.exits & DIR_UP) == 0 && curRoom.x == room.x && (curRoom.y+1) == room.y);
    //    qDebug() << "LEFT:" << ((curRoom.exits & DIR_RIGHT) == 0 && (curRoom.x+1) == room.x && curRoom.y == room.y) << ((curRoom.exits & DIR_RIGHT) == 0 && (curRoom.x+1) == room.x && curRoom.y == room.y);
    //    qDebug() << "RIGHT:" << ((curRoom.exits & DIR_LEFT) == 0 && (curRoom.x-1) == room.x && curRoom.y == room.y) << ((curRoom.exits & DIR_LEFT) == 0 && (curRoom.x-1) == room.x && curRoom.y == room.y);
    //    qDebug() << curRoom << result;
  }

  //  qDebug() << "GetNeighborRejectExits: " << result;

  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
#undef FUNC_NAME

  return result;
}*/



bool TRoomsGenerator::IsFreePlace() const
{
  QElapsedTimer  time;
  time.start();
#define FUNC_NAME "IsFreePlace"
  RoomList::const_iterator it;
  
  for (it = m_rooms.begin(); it != m_rooms.end(); it++)
  {
    const Room *curRoom = *it;
    if (curRoom->x == m_curRoom->x && curRoom->y == m_curRoom->y)
    {
      if (time.elapsed() > 0)
      {
        qDebug() << FUNC_NAME << time.elapsed() << "true";
      }
      return false;
    }
  }
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed() << "false";
  }
#undef FUNC_NAME
  
  return true;
}

QList<Tag> TRoomsGenerator::GetNearestRoomTags(const Room &room, const RoomList &list)
{
  QElapsedTimer  time;
  time.start();
#define FUNC_NAME "GetNearestRoomTags"
  RoomList::const_iterator it;
  
  QList<Tag> result;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room *curRoom = *it;
    if (curRoom->exits & DIR_DOWN && curRoom->x == room.x && (curRoom->y-1) == room.y) result.push_back(curRoom->roomType);
    if (curRoom->exits & DIR_UP && curRoom->x == room.x && (curRoom->y+1) == room.y) result.push_back(curRoom->roomType);
    if (curRoom->exits & DIR_RIGHT && (curRoom->x+1) == room.x && curRoom->y == room.y) result.push_back(curRoom->roomType);
    if (curRoom->exits & DIR_LEFT && (curRoom->x-1) == room.x && curRoom->y == room.y) result.push_back(curRoom->roomType);
  }
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
#undef FUNC_NAME
  
  return result;
}

MapObjAlgList TRoomsGenerator::GetObjListForRoom(const MapObjAlgList &objList, const Room &room, const RoomList &list)
{
  const QList<Tag> tags = GetNearestRoomTags(room, list);
  MapObjAlgList result;
  
  MapObjAlgList::const_iterator it;
  for (it = objList.begin(); it != objList.end(); it++)
  {
    const MapObjectForAlg &obj = *it;
    bool needAdd = true;
    for (int i = 0; i < EndArraySize && (obj.rejectList[i] != -1); i++)
    {
      if(tags.contains(obj.rejectList[i])) {needAdd = false; break;}
    }
    if (needAdd)
    {
      //      qDebug() << "obj.maxCount" << obj.maxCount << "obj.havingCount" << obj.havingCount;
      if (obj.maxCount > obj.havingCount)
      {
        result.push_back(obj);
      }
    }
  }
  
  return result;
}

Tag TRoomsGenerator::GetRandomRoomType(const MapObjAlgList &objList)
{
  MapObjAlgList::const_iterator it;
  
  int chanceSumm = 0;
  
  QVector<int> edge;
  edge.push_back(0);
  
  for (it = objList.begin(); it != objList.end(); it++)
  {
    const MapObjectForAlg &obj = *it;
    chanceSumm += obj.chance * (obj.maxCount - obj.havingCount);
    edge.push_back(chanceSumm);
  }
  
  const int rand = TRandomizer::GetRandom(0, chanceSumm);
  
  qDebug() << "GetRandomRoomType" /*<< objList */<< edge << rand;
  
  for (int i = 0; i < objList.count(); i++)
  {
    qDebug() << "Loop " << i << objList.at(i).tag << edge.at(i) << rand << (edge.at(i) < rand);
    if (rand < edge.at(i))
    {
      qDebug() << "Return" << i << objList.at(i).tag;
      return objList.at(i).tag;
    }
  }
  
  qDebug() << "NoResturn!!!";
  
  return 3; //Если не получилось получить рандомный тип комнаты возвращаем ситуацию.
}

RoomList TRoomsGenerator::DrawLabirint(MapObjAlgList objList, int count)
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "DrawLabirint"
  
  m_curRoom = new Room();

  m_curRoom->x = 0;
  m_curRoom->y = 0;
//  m_curRoom.roomType = 0; //Вход всегда первый.
  m_curRoom->roomType = 3; //Пока все будут ситуациями рандом позже.
  m_curRoom->index = 0;
  
  m_curRoom->exits = GetRandomExits(GetRandomExitCount(0, count, ExitCount(DIR_NO)), DIR_NO);
  
//  qDebug() << "One:" << newRoom;
  
  m_rooms.push_back(m_curRoom);
  objList[m_curRoom->roomType].havingCount++;
  
  QStack<const Room*> unfinishedRooms;
  unfinishedRooms.push(m_curRoom);
  
  
  while (!unfinishedRooms.empty())
  {
    QElapsedTimer time2;
    time2.start();
    #define FUNC_NAME2 "DrawLabirint OneRoom"
  
    const Room *oldRoom = unfinishedRooms.pop();
  
    Directions oldFreeExist = GetFreeExits(*oldRoom);
    
    if (oldFreeExist & ~DIR_ALL) 
    {
      qDebug() << "st" << oldFreeExist << (int)oldFreeExist;
      qFatal("stFatal");
    }
  
    if (oldFreeExist != DIR_NO)
    {
      m_curRoom = new Room(*oldRoom);
      FillNextRoomCoordinate(GetFreeExits(*oldRoom), m_curRoom);
      m_curRoom->roomType = 3; //Пока все будут ситуациями рандом позже.
      m_curRoom->exits = (Directions)(m_curRoom->exits | GetNeighborExits(*m_curRoom));
      m_curRoom->exits = GetRandomExits(GetRandomExitCount(m_rooms.count(), count, ExitCount(m_curRoom->exits)), m_curRoom->exits);
      m_curRoom->exits = GetRejectNeighborNoExits(*m_curRoom);
//      qDebug() << "GNRE2" << GetRejectNeighborNoExits(newRoom, result);
//      qDebug() << "Two:" << newRoom;
      
      if (IsFreePlace())
      {
        m_curRoom->index = oldRoom->index + 1;
        m_rooms.push_back(m_curRoom);
//        objList[m_curRoom->roomType].havingCount++;
        if (GetFreeExits(*m_curRoom) != DIR_NO) unfinishedRooms.push(m_curRoom);
//        qDebug() << "Accepted\r\n-------------------\r\n";
//        debugPointer->DrawRooms(result);
//        QMessageBox::information(0, "Accepted", "Accepted");
      }
      else
      {
        delete m_curRoom;
      }
      
      oldFreeExist = GetFreeExits(*oldRoom);
//      qDebug() << "en" << oldFreeExist << (int)oldFreeExist;
      if (oldFreeExist & ~DIR_ALL)
      {
        qFatal("enFatal");
        qDebug() << "en" << oldFreeExist << (int)oldFreeExist;
      }
      if (oldFreeExist != DIR_NO) unfinishedRooms.push(oldRoom);
    }
    
    qDebug() << "UnfinishedCount" << unfinishedRooms.count() << "result count" << m_rooms.count(); 
    if (time2.elapsed() > 0)
    {
      qDebug() << FUNC_NAME2 << time2.elapsed();
    }
    #undef FUNC_NAME2
  }

//  qDebug() << "\r\n\r\n-------------------------------------\r\n\r\n" << result;

  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
  #undef FUNC_NAME

  return m_rooms;
}
