/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef _LUNARLANDER_H_
#include "env.h"
#include <random>
#include <math.h>
#include <algorithm>

/*
 * LunarLander Game Implementation
 *
 * Ported from: https://github.com/openai/gym/blob/master/gym/env_s/box2d/lunar_lander.py
 *
 * STYLE:
 * trailing underscores = class member variable
 * (nice so you know what belongs to the class, and what is just a param or temp)
 *
 */


const float FPS    = 50.0;
const float SCALE  = 30.0;   // affects how fast-paced the game is, forces should be adjusted as well

const float MAIN_ENGINE_POWER  = 13.0;
const float SIDE_ENGINE_POWER  =  0.6;

const float INITIAL_RANDOM = 1000.0;   // Set 1500 to make game harder

const int LEG_AWAY = 20;
const int LEG_DOWN = 18;
const int LEG_W = 2;
const int LEG_H = 8;
const int LEG_SPRING_TORQUE = 40;

const float SIDE_ENGINE_HEIGHT = 14.0;
const float SIDE_ENGINE_AWAY   = 12.0;

const int VIEWPORT_W = 600;
const int VIEWPORT_H = 400;

class ContactDetector;

/*
 * LunarLander to match OpenAI Gym-like API
 */

class LunarLander : public Env
{
    friend class ContactDetector; // make it so ContactDectector can access private member variables
public:
    EnvData reset();
    EnvData step(std::vector<float>);
    void render();
private:
    /*
     * Clean up all resources for next episode
     */
    void destroy(void);

    /*
     *
     */
    void destroyParticles(bool all);

    /*
     *
     */

    b2Body* createParticle(float mass, float x, float y, float* power);

    b2World* world_; // main Box2D world object

    b2Body* moon_; // body of lunar surface
    std::list<b2Body*> particles_; // particles that shoot out of the rocket (just for looks)
    std::vector<std::tuple<b2Vec2, b2Vec2, b2Vec2, b2Vec2> > skyPolys_; // I think is for drawing the sky
    b2Body* lander_; // body of lunar lander
    b2Body* leftLeg_; // left leg of lunar lander
    b2Body* rightLeg_; // left leg of lunar lander
    b2RevoluteJoint* rightLegJoint_;
    b2RevoluteJoint* leftLegJoint_;
    bool leftLegGroundContact_; // if the left leg is touching the ground
    bool rightLegGroundContact_; // if the right leg is touching the ground
    ContactDetector* contactDetector_; // Box2D object for handling collisions
    bool gameOver_; // this will get set when the thing crashes or something, so we know when to end things
    float prevShaping_; // used for setting reward
    std::vector<b2Body*> drawList_;
    bool continuous_;
    b2Vec2 LANDERPOLY[6]; // lander poly shape
    sf::RenderWindow *viewer_;


    b2FixtureDef particleFixtureDef;
    b2FixtureDef groundDef;
    b2FixtureDef landerFixtureDef;
    b2FixtureDef leftLegFixtureDef;
    b2FixtureDef rightLegFixtureDef;
    b2BodyDef leftLegBodyDef;
    b2BodyDef rightLegBodyDef;
    b2BodyDef particleBodyDef;
    b2BodyDef moonBodyDef;
    b2BodyDef landerBodyDef;

    int helipadY_;
    int helipadX1_;
    int helipadX2_;

    //textures
    sf::Texture bgTex;
    sf::Texture shipBodyTex;
    sf::Texture legTex;
    sf::Texture sparkTex;
    sf::Texture flagTex;

    std::random_device rd_;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen_;

public:
    LunarLander(bool continuous);
    ~LunarLander();
    //std::pair<float, float> observationRange;
    float lastReward;
};


#endif
#define _LUNARLANDER_H_
