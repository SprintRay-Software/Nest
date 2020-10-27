#include "Config.h"

double Config::cliiperScale = 10000;
double Config::curveTolerance = 0.02;

Config::Config()
{
    spacing = 0;
    populationSize = 10;
    mutationRate = 10;
    concave = false;
    useHole = false;
}

bool Config::isConcave()
{
    return concave;
}

bool Config::isUseHole()
{
    return useHole;
}

