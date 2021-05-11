#pragma once
#include <map>
#include "GeneticAlgorithm.h"
#include "Individual.h"
#include "vector.h"
#include "Placement.h"
#include "ParallelData.h"
#include "NfpPair.h" 
#include "NfpUtil.h"
#include "Gson.h"
#include "Result.h"
#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include "GeneticAlgorithm.h"
#include "Individual.h"
#include "CommonUtil.h"
#include "GeometryUtil.h"
#include "NestPath.h"
#include "Result.h"
#include "qDebug"
class Nest
{
public:

    /**
     * @brief
     *
     * @param binPath_para  托盘
     * @param parts         所有模型
     * @param config        配置
     * @param count         population遗传因子的种群数目
     */
    Nest(NestPath binPath_para, vector<NestPath> parts, Config config, int count);
    vector<vector<Placement>> startNest();
    Result launchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config);
    static vector<vector<Placement>> applyPlacement(Result best, vector<NestPath> tree);   
    /**
     * @brief 对每个模型进行rotateNum次的旋转，每次旋转360/rotateNum角度，然后只要模型能够被放进盘子，就将
     *        模型对应的index添加进去
     * @param binPolygon        底盘模型
     * @param tree              其他所有模型
     * @return vector<int>
     */
    static vector<int>  checkIfCanBePlaced(NestPath binPolygon, vector<NestPath> tree);
    void Add(NestPath np);
    NestPath getBinPath();
    vector<NestPath> getParts();
    void setBinPath(NestPath binPath);
    void setParts(vector<NestPath> parts);
    Config getConfig();
    void setConfig(Config config);

private:
    NestPath binPath;
    vector<NestPath> parts;
    Config config;
    int loopCount;
    GeneticAlgorithm *GA = NULL;
    map<string, vector<NestPath>> *nfpCache;
    static Gson *gson;
    int launchCount = 0;
    int rotationNumber = 0;
};

