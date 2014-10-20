#include "trandomizer.h"
#include <stdlib.h>
#include <Qt>

TRandomizer::TRandomizer()
{
}

int TRandomizer::GetRandom(int min, int max)
{
  int rand = qrand();

  double randPos = (double)rand / RAND_MAX;

  int result = max - (int)((double)(max - min + 1) * randPos);

  return result;
}
