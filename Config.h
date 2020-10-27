#pragma once


class Config
{
public:
	Config();
	bool isConcave();
    bool isUseHole();

public:
    static double cliiperScale;
    static double curveTolerance;
    double spacing;
    int populationSize;
    int mutationRate;
    bool useHole;

private:
    bool concave;
};
