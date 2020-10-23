#pragma once


class Config
{
public:
	Config();
	bool isConcave();
	bool isUse_hole();

public:
	static double m_cliiper_scale;
	static double m_curve_tolerance;
	double m_spacing;
	int m_population_size;
	int m_mutation_rate;
	bool m_use_hole;

private:
	bool m_concave;
};

//double Config::CLIIPER_SCALE = 10000;
//double Config::CURVE_TOLERANCE = 0.02;