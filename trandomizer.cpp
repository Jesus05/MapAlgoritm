#include "trandomizer.h"
#include <stdlib.h>
#include <Qt>

TRandomizer::TRandomizer()
{
}

int TRandomizer::GetRandom(int min, int max)
{
  int rand = qrand();

  double randPos = (double)rand / (RAND_MAX + 1);

  int result = max - (int)((double)(max - min + 1) * randPos);

//  result = qMax(result, min);
//  result = qMin(result, max);

  return result;
}


//int TRandomizer::GetRandomTest(int min, int max, int rand)
//{
////  int rand = qrand();

//  double randPos = (double)rand / (RAND_MAX + 1);

//  int result = max - (int)((double)(max - min + 1) * randPos);

////  result = qMax(result, min);
////  result = qMin(result, max);

//  return result;
//}
