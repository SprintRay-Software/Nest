#include "Placement.h"


Placement::Placement(int bid, Segments translate, double rotate, string name)
{
    this->bid = bid;
    this->translate = translate;
    this->rotate = rotate;
    this->name = name;
}

Placement::Placement()
{
}

