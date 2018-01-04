/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef _ENV_H_

#include "mainwindow.h"
#include <QApplication>
#include "Box2D/Box2D.h"
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "QtMath"
#include "tuple"
#include "vector"

struct EnvData {
    std::vector<float> observation;
    float reward;
    bool done;
};

class Env
{
public:
    //virtual EnvData reset();
    //virtual EnvData step(std::vector<float>);
    //virtual void render(void);
    //TODO: close() maybe

    std::vector<float> actionSpace;
    std::vector<float> observationSpace;
    std::vector<float> rewardRange;

};


#endif
#define _ENV_H_
