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
    double spacing;                 //间隙   模型之间的
    int populationSize;             //遗传群落的大小
    int mutationRate;               //突变率   很低
    bool useHole;

private:
    bool concave;
};
