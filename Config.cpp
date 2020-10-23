#include "Config.h"

double Config::m_cliiper_scale = 10000;
double Config::m_curve_tolerance = 0.02;

Config::Config()
{
	m_spacing = 0;
	m_population_size = 10;
	m_mutation_rate = 10;
	m_concave = false;
	m_use_hole = false;
}

bool Config::isConcave()
{
	return m_concave;
}

bool Config::isUse_hole()
{
	return m_use_hole;
}

