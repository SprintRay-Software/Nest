#pragma once
#include "NfpPair.h"
#include "Config.h"
#include "Bound.h"
#include "ParallelData.h"
#include "vector.h"

class NfpUtil
{
public:
	static ParallelData* NfpGenerator(NfpPair pair, Config config);

};