#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DebugAddons.h"
#include <QList>
#include <QDebug>
#include "Types.h"
#include <stdlib.h>
#include <QStack>
#include <qlogging.h>
#include <QMessageBox>
#include <QWheelEvent>
#include <QImage>
#include <QElapsedTimer>

MainWindow *debugPointer;

MapObjectStruct ObjectList[] = {
  { 0, "Enter"     , 0, 100, {   -1}, {1, 2, 4,          -1}},
  { 1, "Exit"      , 0, 100, {   -1}, {0,                -1}},
  { 2, "BeforeExit", 0, 100, {   -1}, {0, 1, 2, 3, 4, 5, -1}}, //Думаю ее вообще лучше ставить постпроцессом.
  { 3, "Situation" , 0, 100, {   -1}, {                  -1}},
  { 4, "Boss"      , 3, 100, {   -1}, {0,                -1}},
  { 5, "Vault"     , 4, 100, {   -1}, {                  -1}}
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
  temp.maxCount = 12;
  
  result.push_back(temp);
  
  temp = ObjectList[4];
  
  temp.chance = 100;
  temp.minCount = 0;
  temp.maxCount = 2;
  
  result.push_back(temp);
  
  temp = ObjectList[5];
  
  temp.chance = 100;
  temp.minCount = 0;
  temp.maxCount = 1;
  
  result.push_back(temp);
  return result;
}

int GetRandom(int min, int max)
{
  int rand = qrand();
  
  double randPos = (double)rand / RAND_MAX;
  
  int result = max - (int)((double)(max - min + 1) * randPos);
  
  return result;
}

int GetRandomExitCount(int havingRooms, int needRooms, int HavingExit = 1/*, int unfinishedRooms = 0*/)
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
    randomIndex = GetRandom(0, 3);
    if (qrand() % 2) result = (Directions)(result | randomExits[randomIndex]);
//    qDebug() << "result loop" << result << randomIndex << randomExits[randomIndex];
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
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "GetFreeExits"
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
  
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
  #undef FUNC_NAME
  
  return result;
}

Directions GetNeighborExits(Room room, const RoomList &list)
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "GetNeighborExits"
  RoomList::const_iterator it;
//  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
//  qDebug() << "GetNeighborExits: " << result;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room &curRoom = *it;
    if (curRoom.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y-1) == room.y) result = (Directions)(result | DIR_UP);
    if (curRoom.exits & DIR_UP && curRoom.x == room.x && (curRoom.y+1) == room.y) result = (Directions)(result | DIR_DOWN);
    if (curRoom.exits & DIR_RIGHT && (curRoom.x+1) == room.x && curRoom.y == room.y) result = (Directions)(result | DIR_LEFT);
    if (curRoom.exits & DIR_LEFT && (curRoom.x-1) == room.x && curRoom.y == room.y) result = (Directions)(result | DIR_RIGHT);
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

Directions GetRejectNeighborNoExits(Room room, const RoomList &list)
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "GetRejectNeighborNoExits"
  RoomList::const_iterator it;
//  const RoomList::iterator left = list.end(), right = list.end(), down = list.end(), up = list.end();
  Directions result = room.exits;
  
//  qDebug() << "GetNeighborRejectExits: " << result;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room &curRoom = *it;
    if ((curRoom.exits & DIR_DOWN) == 0 && curRoom.x == room.x && (curRoom.y-1) == room.y) result = (Directions)(result & ~DIR_UP);
    if ((curRoom.exits & DIR_UP) == 0 && curRoom.x == room.x && (curRoom.y+1) == room.y) result = (Directions)(result & ~DIR_DOWN);
    if ((curRoom.exits & DIR_RIGHT) == 0 && (curRoom.x+1) == room.x && curRoom.y == room.y) result = (Directions)(result & ~DIR_LEFT);
    if ((curRoom.exits & DIR_LEFT) == 0 && (curRoom.x-1) == room.x && curRoom.y == room.y) result = (Directions)(result & ~DIR_RIGHT);
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

bool IsFreePlace(const Room &room, const RoomList &list)
{
  QElapsedTimer  time;
  time.start();
  #define FUNC_NAME "IsFreePlace"
  RoomList::const_iterator it;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room &curRoom = *it;
    if (curRoom.x == room.x && curRoom.y == room.y) 
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

QList<Tag> GetNearestRoomTags(const Room &room, const RoomList &list)
{
  QElapsedTimer  time;
  time.start();
  #define FUNC_NAME "GetNearestRoomTags"
  RoomList::const_iterator it;
  
  QList<Tag> result;
  
  for (it = list.begin(); it != list.end(); it++)
  {
    const Room &curRoom = *it;
    if (curRoom.exits & DIR_DOWN && curRoom.x == room.x && (curRoom.y-1) == room.y) result.push_back(curRoom.roomType);
    if (curRoom.exits & DIR_UP && curRoom.x == room.x && (curRoom.y+1) == room.y) result.push_back(curRoom.roomType);
    if (curRoom.exits & DIR_RIGHT && (curRoom.x+1) == room.x && curRoom.y == room.y) result.push_back(curRoom.roomType);
    if (curRoom.exits & DIR_LEFT && (curRoom.x-1) == room.x && curRoom.y == room.y) result.push_back(curRoom.roomType);
  }
  
  if (time.elapsed() > 0)
  {
    qDebug() << FUNC_NAME << time.elapsed();
  }
  #undef FUNC_NAME
  
  return result;
}

MapObjAlgList GetObjListForRoom(const MapObjAlgList &objList, const Room &room, const RoomList &list)
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

Tag GetRandomRoomType(const MapObjAlgList &objList/*, const Room &room, const RoomList &list*/)
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
  
  const int rand = GetRandom(0, chanceSumm);
  
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

RoomList DrawLabirint(MapObjAlgList objList, int count)
{
  QElapsedTimer time;
  time.start();
  #define FUNC_NAME "DrawLabirint"
  
  RoomList result;
  
  Room newRoom;
  
  newRoom.x = 0;
  newRoom.y = 0;
  newRoom.roomType = 0; //Вход всегда первый.
  
  newRoom.exits = GetRandomExits(GetRandomExitCount(0, count, ExitCount(DIR_NO)), DIR_NO);
  
//  qDebug() << "One:" << newRoom;
  
  result.push_back(newRoom);
  objList[newRoom.roomType].havingCount++;
  
  QStack<Room> unfinishedRooms;
  unfinishedRooms.push(newRoom);
  
  
  while (!unfinishedRooms.empty())
  {
    QElapsedTimer time2;
    time2.start();
    #define FUNC_NAME2 "DrawLabirint OneRoom"
  
    Room oldRoom = unfinishedRooms.pop();
  
    Directions oldFreeExist = GetFreeExits(oldRoom, result);
    
    if (oldFreeExist & ~DIR_ALL) 
    {
      qDebug() << "st" << oldFreeExist << (int)oldFreeExist;
      qFatal("stFatal");
    }
  
    if (oldFreeExist != DIR_NO)
    {
      newRoom = FillNextRoomCoordinate(oldRoom, GetFreeExits(oldRoom, result));
//      newRoom.roomType = 3; //Пока все будут ситуациями рандом позже.
      newRoom.roomType = GetRandomRoomType(GetObjListForRoom(objList, newRoom, result));
//      qDebug() << newRoom << "re" << newRoom.exits << "GNE" << GetNeighborExits(newRoom, result) << "GNRE" << GetRejectNeighborNoExits(newRoom, result);
      newRoom.exits = (Directions)(newRoom.exits | GetNeighborExits(newRoom, result));
      newRoom.exits = GetRandomExits(GetRandomExitCount(result.count(), count, ExitCount(newRoom.exits)), newRoom.exits);
      newRoom.exits = GetRejectNeighborNoExits(newRoom, result);
//      qDebug() << "GNRE2" << GetRejectNeighborNoExits(newRoom, result);
//      qDebug() << "Two:" << newRoom;
      
      if (IsFreePlace(newRoom, result))
      {
        result.push_back(newRoom);
        objList[newRoom.roomType].havingCount++;
        if (GetFreeExits(newRoom, result) != DIR_NO) unfinishedRooms.push(newRoom);
//        qDebug() << "Accepted\r\n-------------------\r\n";
//        debugPointer->DrawRooms(result);
//        QMessageBox::information(0, "Accepted", "Accepted");
      }
      
      oldFreeExist = GetFreeExits(oldRoom, result);
//      qDebug() << "en" << oldFreeExist << (int)oldFreeExist;
      if (oldFreeExist & ~DIR_ALL)
      {
        qFatal("enFatal");
        qDebug() << "en" << oldFreeExist << (int)oldFreeExist;
      }
      if (oldFreeExist != DIR_NO) unfinishedRooms.push(oldRoom);
    }
    
    qDebug() << "UnfinishedCount" << unfinishedRooms.count() << "result count" << result.count(); 
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

  return result;
}


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->graphicsView->setScene(new QGraphicsScene);
  ui->graphicsView->setSceneRect(0, 0, 1, 1);
  m_minX = 0;
  m_minY = 0;
  m_maxX = 1;
  m_maxY = 1;
  debugPointer = this;
  ui->graphicsView->viewport()->installEventFilter(this);
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
    
//    double upP = (double)up/summ;
//    double downP = (double)down/summ;
//    double leftP = (double)left/summ;
//    double rightP = (double)right/summ;
    
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
  int min = 100;
  int max = 101;
  
  for (int i = min; i < max; i++)
  {
    qDebug() << "BeforeDraw";
    DrawLabirint(GetList(), i);
    qDebug() << "AfterDraw";
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
  TestFour();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);
//  if (ui->graphicsView->scene() != NULL)
  {
    ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
    ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
  }
}

bool MainWindow::eventFilter(QObject */*target*/, QEvent *event)
{
  if (QEvent::Wheel == event->type())
  {
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
    if (wheelEvent != NULL)
    {
      double d = 1.0 + 10.0 / wheelEvent->delta();
      ui->graphicsView->scale(d, d);
      event->accept();
      return true;
    }
  }
  return false;
}


void PrintAllRoomNeighborTags(const RoomList &rooms)
{
  RoomList::const_iterator it;
    for (it = rooms.begin(); it != rooms.end(); it++)
  {
    const Room &room = *it;
    const QList<Tag> &temp = GetNearestRoomTags(room, rooms);
    const MapObjAlgList &temp2 = GetObjListForRoom(GetList(), room, rooms);
    qDebug() << "TagList: " << room << temp << temp2;
  }
}

void MainWindow::TestFour()
{
  int roomCount = ui->leCount->text().toInt();
  m_minX = 0;
  m_minY = 0;
  m_maxX = 0;
  m_maxY = 0;

  int randSeed = ui->leSeed->text().toInt(); //qrand();
  qDebug() << "randSeed" << randSeed;
  qsrand(randSeed);

  qDebug() << "BeforeDraw";
  RoomList rooms = DrawLabirint(GetList(), roomCount);
  qDebug() << "AfterDraw" << "room count" << rooms.count();
  
  DrawRooms(rooms);
  
//  PrintAllRoomNeighborTags(rooms);
  
  on_pushButton_2_clicked();
}

QColor GetColorByTag(const Tag &tag)
{
  switch (tag) {
    case 0: return Qt::blue; break; //Enter
    case 1: return Qt::darkBlue; break; //Exit
    case 2: return Qt::darkCyan; break; //BeforeExit
    case 3: return Qt::yellow; break; //Situation
    case 4: return Qt::darkGray; break; //Boss
    case 5: return Qt::magenta; break; //Vault
    default: return Qt::black; break;
  }
}

void MainWindow::DrawRooms(const RoomList &rooms)
{
  ui->graphicsView->scene()->clear();
  ui->graphicsView->setSceneRect(0, 0, 1, 1);
  
  RoomList::const_iterator it;
  
  for (it = rooms.begin(); it != rooms.end(); it++)
  {
    const Room &room = *it;
    const int lminx = room.x * 20, lminy = -room.y * 20, lmaxx = (room.x+1) * 20, lmaxy = (-room.y+1) * 20;
    ui->graphicsView->scene()->addRect(lminx + 5, lminy + 5, 10, 10, QPen(), QBrush(GetColorByTag(room.roomType)));
    m_minX = qMin(m_minX, lminx);
    m_minY = qMin(m_minY, lminy);
    m_maxX = qMax(m_maxX, lmaxx);
    m_maxY = qMax(m_maxY, lmaxy);
    
    if (room.exits & DIR_LEFT) ui->graphicsView->scene()->addLine(lminx, lminy + 10, lminx + 5, lminy + 10);
    if (room.exits & DIR_UP) ui->graphicsView->scene()->addLine(lminx + 10, lminy, lminx + 10, lminy + 5);
    if (room.exits & DIR_DOWN) ui->graphicsView->scene()->addLine(lminx + 10, lminy + 20, lminx + 10, lminy + 15);
    if (room.exits & DIR_RIGHT) ui->graphicsView->scene()->addLine(lminx + 20, lminy + 10, lminx + 15, lminy + 10);
    
//    qDebug() << "minX:" << minX << "minY:" << minY << "maxX" << maxX << "maxY" << maxY;
  }
  
//  qDebug() << "minX:" << minX << "minY:" << minY << "maxX" << maxX << "maxY" << maxY;
  
  ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
  ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
}

void MainWindow::on_pushButton_2_clicked()
{
  ui->lLastSeed->setText(ui->leSeed->text());
//  ui->pushButton_2->setText(QString("Generate seed. old seed:") + ui->leSeed->text());
  ui->leSeed->setText(QString::number(qrand()));
}

void MainWindow::on_pushButton_3_clicked()
{
  QImage image(ui->graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  ui->graphicsView->scene()->render(&painter);
  image.save(QString("Labirint seed ") + ui->lLastSeed->text() + " count " + ui->leCount->text() + ".png");
  ui->graphicsView->scene()->items().count();
}
