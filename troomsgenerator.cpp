#include "troomsgenerator.h"

TRoomsGenerator::TRoomsGenerator()
{
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
