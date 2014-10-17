#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DebugAddons.h"
#include <QList>
#include <QDebug>
#include "Types.h"
#include <stdlib.h>
#include <QStack>

MapObjectStruct ObjectList[] = {
  { 0, "Enter"     , 0, 100, {0, -1}   , {2, 0, -1}},
  { 1, "Exit"      , 0, 100, {1, -1}   , {1, -1}   },
  { 2, "BeforeExit", 0, 100, {1, 2, -1}, {2, -1}   },
  { 3, "Situation" , 0, 100, {3, -1}   , {3, -1}   },
  { 4, "Boss"      , 3, 100, {4, -1}   , {0, 4, -1}},
  { 5, "Vault"     , 4, 100, {5, -1}   , {5, -1}   }
};

typedef QList <MapObjectForAlg> MapObjAlgList;

MapObjAlgList GetList()
{
  MapObjAlgList result;
  
  MapObjectForAlg temp;
  
  temp = ObjectList[0];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[1];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[2];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[3];
  
  temp.chance = 100;
  temp.minCount = 5;
  temp.maxCount = -1;
  
  result.push_back(temp);
  
  temp = ObjectList[4];
  
  temp.chance = 40;
  temp.minCount = 0;
  temp.maxCount = 2;
  
  result.push_back(temp);
  
  temp = ObjectList[5];
  
  temp.chance = 20;
  temp.minCount = 0;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  return result;
}

typedef QList<Room> RoomList;

int GetRandom(int min, int max)
{
  int rand = qrand();
  
  double randPos = (double)rand / RAND_MAX;
  
  int result = max - (int)((double)(max - min + 1) * randPos);
  
  return result;
}

int GetRandomExitCount(int havingRooms, int needRooms, int HavingExit = 1)
{
  if (havingRooms > needRooms) return HavingExit;

  int rand = qrand();
  int min = HavingExit;
  int max = 3;
  
  double randPos = (double)rand / RAND_MAX;
  double roomPos = (double)havingRooms / needRooms;
  double roomCutMin = 0.6;
  double roomCutMax = 0.9;
  roomPos = roomCutMin + roomPos*(roomCutMax-roomCutMin); //Обрежем диапазон до roomCutMin ... roomCutMax
  double resultPos = randPos * roomPos;
  //TODO поиграться с формулами зависимости отсавшихся комнат и рандома.
  int result = max - (int)((double)(max - min + 1) * resultPos);
  
  qDebug() << "RandomExitCount:" << havingRooms << needRooms << resultPos << randPos << roomPos << (double)(max - min + 1) * resultPos << (int)((double)(max - min + 1) * resultPos) << result;
  
  return result;
}

int ExitCount(Directions exits)
{
  int having = 0;
  
  if (exits & DIR_UP) having++;
  if (exits & DIR_DOWN) having++;
  if (exits & DIR_LEFT) having++;
  if (exits & DIR_RIGHT) having++;
  
  return having;
}

Directions SplitExits(Directions exits[4])
{
  Directions splitExits = DIR_NO;
  
  for (int i = 0; i < 4; i++)
  {
    splitExits = (Directions)(splitExits | exits[i]);
  }
  
  return splitExits;
}

int ExitCount(Directions exits[4])
{
  return ExitCount(SplitExits(exits));
}

Directions SelectRandDirection(Directions having)
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
    randomIndex = GetRandom(0, 4);
    if (qrand() % 2) result = (Directions)(result | randomExits[randomIndex]);
  }
  
  return result;
}

Directions GetRandomExits(int exitCount, Directions having)
{
  qDebug() << "having" << having;

  if (ExitCount(having) >= exitCount) //Если все ходы уже есть то возвращаем все ходы.
  {
    qDebug() << "GetRandomExit";
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
  qDebug() << "result" << result;
  
  while (ExitCount(result) < exitCount)
  {
    randomIndex = GetRandom(0, 3);
    if (qrand() % 2) result = (Directions)(result | randomExits[randomIndex]);
    qDebug() << "result loop" << result << randomIndex << randomExits[randomIndex];
  }
  
  return result;
}

Room FillNextRoomCoordinate(Room room, Directions freeExits)
{
  Room result = room;
  Directions temp = SelectRandDirection(freeExits);
  
  if (temp & DIR_UP) { result.y++; result.exits = DIR_DOWN; }
  if (temp & DIR_DOWN) { result.y--; result.exits = DIR_UP; }
  if (temp & DIR_LEFT) { result.x--; result.exits = DIR_RIGHT; }
  if (temp & DIR_RIGHT) { result.x++; result.exits = DIR_LEFT; }
  
  return result;
}

Directions GetFreeExits(Room room, const RoomList list)
{
  RoomList::const_iterator it;
//  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
//  qDebug() << "StartFreeExits: " << result;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room &curRoom = *it;
    if (room.exits & DIR_UP && curRoom.x == room.x && (curRoom.y-1) == room.y) result = (Directions)(result & ~DIR_UP);
    if (room.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y+1) == room.y) result = (Directions)(result & ~DIR_DOWN);
    if (room.exits & DIR_LEFT && (curRoom.x+1) == room.x && curRoom.y == room.y) result = (Directions)(result & ~DIR_LEFT);
    if (room.exits & DIR_RIGHT && (curRoom.x-1) == room.x && curRoom.y == room.y) result = (Directions)(result & ~DIR_RIGHT);
//    qDebug() << "UP:" << (room.exits & DIR_UP) << (curRoom.x == room.x) << ((curRoom.y-1) == room.y) << (room.exits & DIR_UP && curRoom.x == room.x && (curRoom.y-1) == room.y);
//    qDebug() << "DOWN:" << (room.exits & DIR_DOWN) << (curRoom.x == room.x) << ((curRoom.y+1) == room.y) << (room.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y+1) == room.y);
//    qDebug() << "LEFT:" << (room.exits & DIR_LEFT) << ((curRoom.x+1) == room.x) << (curRoom.y == room.y) << (room.exits & DIR_LEFT && (curRoom.x+1) == room.x && curRoom.y == room.y);
//    qDebug() << "RIGHT:" << (room.exits & DIR_RIGHT) << ((curRoom.x-1) == room.x) << (curRoom.y == room.y) << (room.exits & DIR_RIGHT && (curRoom.x-1) == room.x && curRoom.y == room.y);
//    qDebug() << curRoom << result;
  }
  
//  qDebug() << "EndFreeExits: " << result;
  
  return result;
}

RoomList DrawLabirint(MapObjAlgList objList, int count)
{
  RoomList result;
  
  Room newRoom;
  
  newRoom.x = 0;
  newRoom.y = 0;
  newRoom.roomType = 0; //Вход всегда первый.
  newRoom.exits = GetRandomExits(GetRandomExitCount(0, count), DIR_NO);
  
  qDebug() << "One:" << newRoom;
  
  result.push_back(newRoom);
  
  QStack<Room> unfinishedRooms;
  unfinishedRooms.push(newRoom);
  
  while (!unfinishedRooms.empty())
  {
    Room oldRoom = unfinishedRooms.pop();
  
    Directions oldFreeExist = GetFreeExits(oldRoom, result);
  
    qDebug() << "st" << oldFreeExist << (int)oldFreeExist;
  
    if (oldFreeExist != DIR_NO)
    {
  
      newRoom = FillNextRoomCoordinate(oldRoom, GetFreeExits(oldRoom, result));
      newRoom.roomType = 3; //Пока все будут ситуациями рандом позже.
      newRoom.exits = GetRandomExits(GetRandomExitCount(result.count(), count), newRoom.exits);
    
      qDebug() << "Two:" << newRoom;
    
      result.push_back(newRoom);
    
      oldFreeExist = GetFreeExits(oldRoom, result);
    
      qDebug() << "en" << oldFreeExist << (int)oldFreeExist;
      if (oldFreeExist != DIR_NO) unfinishedRooms.push(oldRoom);
      if (GetFreeExits(newRoom, result) != DIR_NO) unfinishedRooms.push(newRoom);
      
    }
    qDebug() << "UnfinishedCount" << unfinishedRooms.count() << "result count" << result.count(); 
  }

  qDebug() << "\r\n\r\n-------------------------------------\r\n\r\n" << result;

}


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void TestOne()
{
  int min = 0;
  int max = 1000;
  
  static int up = 0, down = 0, left = 0, right = 0;
//  for (int j = min; j < max; j++)
  {

    for (int i = min; i < max; i++)
    {
      const Directions temp = GetRandomExits(3, (Directions)(DIR_DOWN | DIR_LEFT));
      if (temp & DIR_UP) up++;
      if (temp & DIR_DOWN) down++;
      if (temp & DIR_LEFT) left++;
      if (temp & DIR_RIGHT) right++;
    }
    
    int summ = up + down + left + right;
    
    double upP = (double)up/summ;
    double downP = (double)down/summ;
    double leftP = (double)left/summ;
    double rightP = (double)right/summ;
    
    qDebug() << DIR_UP << (double)up/summ << DIR_DOWN << (double)down/summ << DIR_LEFT << (double)left/summ << DIR_RIGHT << (double)right/summ;
  }
  
//  qDebug() << GetRandomExits(1, DIR_NO);

/*  qDebug() << "Min: " << min << " Max: " << max;

  for (int i = min; i < max + 1; i++)
  {
    int roomCount = GetRandomExitCount(i, max);
//    if ((roomCount == 4 || roomCount == 1) && i != 0)
      qDebug() << i << " : " << roomCount;
  }*/
}

void TestTwo()
{
  int min = 1;
  int max = 10;
  
  for (int i = min; i < max; i++)
  {
    DrawLabirint(GetList(), i);
  }
}

void TestThree()
{
  int min = 0;
  int max = 1000;

  static int up = 0, down = 0, left = 0, right = 0;
//  for (int j = min; j < max; j++)
  {

    for (int i = min; i < max; i++)
    {
      const Directions temp = SelectRandDirection((Directions)(DIR_DOWN | DIR_LEFT | DIR_UP | DIR_RIGHT));
      if (temp & DIR_UP) up++;
      if (temp & DIR_DOWN) down++;
      if (temp & DIR_LEFT) left++;
      if (temp & DIR_RIGHT) right++;
    }
    
    int summ = up + down + left + right;
    
/*    double upP = (double)up/summ;
    double downP = (double)down/summ;
    double leftP = (double)left/summ;
    double rightP = (double)right/summ;
*/    
    qDebug() << DIR_UP << (double)up/summ << DIR_DOWN << (double)down/summ << DIR_LEFT << (double)left/summ << DIR_RIGHT << (double)right/summ;
  }

}

void MainWindow::on_pushButton_clicked()
{
  TestTwo();
}
