/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#include "lunarlander.h"
#include <iostream>

/** Eliminates extreme values for lander */
float clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

/** Custom handler for collisions so we can handle them in our game */
class ContactDetector : public b2ContactListener
{
public:
    /** Lander environment */
    LunarLander* env_;
    ContactDetector() {
    }
    /** Set lander environment */
    void SetEnv(LunarLander* env) {
        env_ = env;
    }

    /** Event handler called when two Box2D bodies collide (contact) */
    void BeginContact(b2Contact* contact) {
//        std::cout<<"here"<<std::endl;
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();
        // Lander crashed into the ground
        if (env_->lander_ == bodyA || env_->lander_ == bodyB) {
            //std::cout << "end game\n";
            env_->gameOver_ = true;
        }
        // legs are touching the ground (so the bot knows it has reached the surface)
        if (env_->leftLeg_ == bodyA || env_->leftLeg_ == bodyB) {
            env_->leftLegGroundContact_ = true;
        }
        if (env_->rightLeg_ == bodyA || env_->rightLeg_ == bodyB) {
            env_->rightLegGroundContact_ = true;
        }
    }
    /** Event handler called when two Box2D bodies stop contacting */
    void EndContact(b2Contact* contact) {
        // these
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();

        // check if either leg ended contact with the ground
        if (env_->leftLeg_ == bodyA || env_->leftLeg_ == bodyB) {
            env_->leftLegGroundContact_ = false;
        }
        if (env_->rightLeg_ == bodyA || env_->rightLeg_ == bodyB) {
            env_->rightLegGroundContact_ = false;
        }
    }
};

/* Lunar lander game with Box2d and SFML */
LunarLander::LunarLander(bool continuous): gen_(std::random_device()()) {
    // create lander shape
    LANDERPOLY[0] = b2Vec2(-14, +17);
    LANDERPOLY[1] = b2Vec2(-17, 0);
    LANDERPOLY[2] = b2Vec2(-17, -10);
    LANDERPOLY[3] = b2Vec2(+17, -10);
    LANDERPOLY[4] = b2Vec2(+17, 0);
    LANDERPOLY[5] = b2Vec2(+14, +17);
    for (int i = 0; i < 6; i++) {
        LANDERPOLY[i] *= 1.0/SCALE;
    }

    // set up action space
    actionSpace.clear();
    if (continuous) {
        // Action is two floats [main engine, left-right engines].
        // Main engine: -1..0 off, 0..+1 throttle from 50% to 100% power. Engine can't work with less than 50% power.
        // Left-right:  -1.0..-0.5 fire left engine, +0.5..+1.0 fire right engine, -0.5..0.5 off
        actionSpace.push_back(1.0);
        actionSpace.push_back(1.0);
    }
    else {
        // discrete takes a single action of 4 possible values (0,1,2,3 = noop, left engine, main engine, right engine)
        actionSpace.push_back(4);
    }

    world_ = new b2World(b2Vec2(0.f, -10.f));
    moon_ = nullptr;
    lander_ = nullptr;
    leftLeg_ = nullptr;
    rightLeg_ = nullptr;
    contactDetector_ = nullptr;
    viewer_ = nullptr;

    //load textures
    bgTex.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/Assets/spaceBGTex.png");
    shipBodyTex.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/Assets/shipBodyTex.png");
    legTex.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/Assets/legtex.png");
    sparkTex.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/Assets/sparkTex.png");
    flagTex.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/Assets/flagTex.png");
}

LunarLander::~LunarLander() {
    destroy();
    free(world_);
}

/** Creates b2Body particles when lander gas is being used */
b2Body* LunarLander::createParticle(float mass, float x, float y, float* power) {
    particleBodyDef.type = b2_dynamicBody;
    particleBodyDef.position = b2Vec2(x, y);
    particleBodyDef.angle = 0.0;
    b2CircleShape* circle = new b2CircleShape;
    circle->m_p = b2Vec2(0.0, 0.0);
    circle->m_radius = 2.0/SCALE;
    particleFixtureDef.shape = circle;
    particleFixtureDef.density = mass;
    particleFixtureDef.friction = 0.1;
    particleFixtureDef.filter.categoryBits = 0x0100;
    particleFixtureDef.filter.maskBits = 0x001;
    particleFixtureDef.restitution = 0.3;

    b2Body* particle = world_->CreateBody(&particleBodyDef);
    particle->SetUserData(power);
    particle->CreateFixture(&particleFixtureDef);

    particles_.push_back(particle);

    destroyParticles(false);

    return particle;
}

/** Destroy lander gas b2Body particles */
void LunarLander::destroyParticles(bool all) {
    if(particles_.size() == 0)
        return;
    if (particles_.front() == nullptr) {
        return;
    }

    float* power = (float*) (particles_.front()->GetUserData());

    while (!particles_.empty() && (all || *power < 0.0)) {
        // clean user data
        free(power);
        // destroy the particle body
        b2Body* particle = particles_.front();
        world_->DestroyBody(particle);
        particles_.pop_front();
        if (!particles_.empty()) {
            power = (float*) (particles_.front()->GetUserData());
        }
    }
}

/** Destroy game bodies */
void LunarLander::destroy() {
    // if things haven't been created, we don't need to destroy them.
    //std::cout << "destroy" << std::endl;
    if (contactDetector_ == nullptr) {
        return;
    }

    world_->DestroyBody(lander_);
    world_->DestroyBody(moon_);
    world_->DestroyBody(leftLeg_);
    world_->DestroyBody(rightLeg_);
    moon_ = nullptr;
    lander_ = nullptr;
    leftLeg_ = nullptr;
    rightLeg_ = nullptr;
    destroyParticles(true);

    free(contactDetector_);
    contactDetector_ = nullptr;
}

/** Reset lunar lander game */
EnvData LunarLander::reset() {
    destroy();
    contactDetector_ = new ContactDetector;
    contactDetector_->SetEnv(this);
    world_->SetContactListener(contactDetector_);
    gameOver_ = false;
    prevShaping_ = 0;

    int H = VIEWPORT_H/SCALE;
    int W = VIEWPORT_W/SCALE;

    // generate terrain
    const int CHUNKS = 11;
    float height[CHUNKS+1];
    float chunkX[CHUNKS];

    // random sample a bunch of heights 0 to H/2
    std::uniform_real_distribution<> heightSampler(0, H/2);
    int i;
    for (i = 0; i < 12; i++) {
        height[i] = heightSampler(gen_);
    }
    for (i = 0; i < 11; i++) {
        chunkX[i] = i * (W/(CHUNKS-1));
    }

    // get x location and y height for where we are aiming to land
    helipadX1_ = chunkX[CHUNKS/2 - 1];
    helipadX2_ = chunkX[CHUNKS/2 + 1];
    helipadY_ = H/4;

    // set the region near the goal to be flat
    height[CHUNKS/2 - 2] = helipadY_;
    height[CHUNKS/2 - 1] = helipadY_;
    height[CHUNKS/2 + 0] = helipadY_;
    height[CHUNKS/2 + 1] = helipadY_;
    height[CHUNKS/2 + 2] = helipadY_;

    float smoothY[CHUNKS];
    for (i = 1; i < CHUNKS-1; i++) {
        smoothY[i] = 0.33*(height[i-1] + height[i+0] + height[i+1]);
    }
    smoothY[0] = smoothY[1];
    smoothY[CHUNKS-1] = smoothY[CHUNKS-2];


    // create moon
    moonBodyDef.type = b2_staticBody;
    moon_ = world_->CreateBody(&moonBodyDef);
    b2EdgeShape* moonEdgeShape = new b2EdgeShape;
    b2Vec2 v1(0.0f, 0.0f);
    b2Vec2 v2((float) W, 0.0f);
    moonEdgeShape->Set(v1, v2);
    moon_->CreateFixture(moonEdgeShape, 1.0);

    skyPolys_.clear();
    for (i = 0; i < CHUNKS-1; i++) {
        b2Vec2 p1(chunkX[i], smoothY[i]);
        b2Vec2 p2(chunkX[i+1], smoothY[i+1]);
        b2EdgeShape* groundEdge = new b2EdgeShape;
        groundEdge->Set(p1, p2);
        groundDef.shape = groundEdge;
        groundDef.density = 0;
        groundDef.friction = 0.1;
        moon_->CreateFixture(&groundDef);

        // stuff for drawing sky
        b2Vec2 skyTop1(p1.x, (float) H);
        b2Vec2 skyTop2(p2.x, (float) H);

        std::tuple<b2Vec2, b2Vec2, b2Vec2, b2Vec2> polyTupe = std::make_tuple(p1, p2, skyTop2, skyTop1);
        skyPolys_.push_back(polyTupe);
    }

    // create lander
    float initLanderY = VIEWPORT_H / SCALE;
    b2BodyDef* landerBodyDef = new b2BodyDef;
    landerBodyDef->type = b2_dynamicBody;
    landerBodyDef->position = b2Vec2((VIEWPORT_W / SCALE)/2.0, initLanderY);
    landerBodyDef->angle = 0.0;

    b2FixtureDef* landerFixtureDef = new b2FixtureDef;
    b2PolygonShape* landerShape_ = new b2PolygonShape;
    landerShape_->Set(LANDERPOLY, 6);
    landerFixtureDef->shape = landerShape_;
    landerFixtureDef->density = 5.0;
    landerFixtureDef->friction = 0.1;
    landerFixtureDef->filter.categoryBits = 0x0010;
    landerFixtureDef->filter.maskBits = 0x001; // collide only with the ground
    landerFixtureDef->restitution = 0.0; // 0.99 bouncy

    lander_ = world_->CreateBody(landerBodyDef);
    lander_->CreateFixture(landerFixtureDef);

    std::uniform_real_distribution<> forceSampler(-INITIAL_RANDOM, INITIAL_RANDOM);
    lander_->ApplyForceToCenter(b2Vec2(forceSampler(gen_), forceSampler(gen_)), true);

    leftLegBodyDef.type = b2_dynamicBody;
    leftLegBodyDef.position = b2Vec2(VIEWPORT_W / SCALE / 2.0 - ((-1)*LEG_AWAY)/SCALE, initLanderY);
    leftLegBodyDef.angle = (-1)*0.05;
    b2PolygonShape* leftLegShape = new b2PolygonShape;
    leftLegShape->SetAsBox(LEG_W/SCALE, LEG_H/SCALE);
    leftLegFixtureDef.shape = leftLegShape;
    leftLegFixtureDef.density = 1.0;
    leftLegFixtureDef.restitution = 0.0;
    leftLegFixtureDef.filter.categoryBits = 0x0020;
    leftLegFixtureDef.filter.maskBits = 0x001;

    leftLeg_ = world_->CreateBody(&leftLegBodyDef);
    leftLeg_->CreateFixture(&leftLegFixtureDef);


    b2RevoluteJointDef leftLegRJD;
    leftLegRJD.bodyA = lander_;
    leftLegRJD.bodyB = leftLeg_;
    leftLegRJD.localAnchorA = b2Vec2(0, 0);
    leftLegRJD.localAnchorB = b2Vec2((-1)*LEG_AWAY/SCALE, LEG_DOWN/SCALE);
    leftLegRJD.enableMotor = true;
    leftLegRJD.enableLimit = true;
    leftLegRJD.maxMotorTorque = LEG_SPRING_TORQUE;
    leftLegRJD.motorSpeed += 0.3*(-1);

    leftLegRJD.lowerAngle = 0.9 - 0.5;
    leftLegRJD.upperAngle = 0.9;

    leftLegJoint_ = (b2RevoluteJoint*) world_->CreateJoint(&leftLegRJD);

    rightLegBodyDef.type = b2_dynamicBody;
    rightLegBodyDef.position = b2Vec2(VIEWPORT_W / SCALE / 2.0 - ((1)*LEG_AWAY)/SCALE, initLanderY);
    rightLegBodyDef.angle = (1)*0.05;
    b2PolygonShape* rightLegShape = new b2PolygonShape;
    rightLegShape->SetAsBox(LEG_W/SCALE, LEG_H/SCALE);
    rightLegFixtureDef.shape = rightLegShape;
    rightLegFixtureDef.density = 1.0;
    rightLegFixtureDef.restitution = 0.0;
    rightLegFixtureDef.filter.categoryBits = 0x0020;
    rightLegFixtureDef.filter.maskBits = 0x001;

    rightLeg_ = world_->CreateBody(&rightLegBodyDef);
    rightLeg_->CreateFixture(&rightLegFixtureDef);


    b2RevoluteJointDef rightLegRJD;
    rightLegRJD.bodyA = lander_;
    rightLegRJD.bodyB = rightLeg_;
    rightLegRJD.localAnchorA = b2Vec2(0, 0);
    rightLegRJD.localAnchorB = b2Vec2((1)*LEG_AWAY/SCALE, LEG_DOWN/SCALE);
    rightLegRJD.enableMotor = true;
    rightLegRJD.enableLimit = true;
    rightLegRJD.maxMotorTorque = LEG_SPRING_TORQUE;
    rightLegRJD.motorSpeed += 0.3*(1);

    rightLegRJD.lowerAngle = -0.9;
    rightLegRJD.upperAngle = -0.9 + 0.5;

    rightLegJoint_ = (b2RevoluteJoint*) world_->CreateJoint(&rightLegRJD);

    drawList_.clear();
    drawList_.push_back(lander_);
    drawList_.push_back(rightLeg_);
    drawList_.push_back(leftLeg_);

    std::vector<float> action; // do nothing
    if (continuous_) {
        action.push_back(0.0);
        action.push_back(0.0);
    }
    else {
        action.push_back(0);
    }
    return step(action);
}

/** Advances the game based on the action */
EnvData LunarLander::step(std::vector<float> action) {
    EnvData envData;

    if (continuous_) {
        //assert(action.size() == 2);
    }
    else {
        assert(action.size() == 1);
        assert(action.front() < actionSpace.front());
    }

    // Engines
    b2Vec2 tip(sin(lander_->GetAngle()), cos(lander_->GetAngle()));
    b2Vec2 side(-tip.y, tip.x);

    std::uniform_real_distribution<> dispersionSampler(-1.0, 1.0);
    b2Vec2 dispersion(dispersionSampler(gen_) / SCALE, dispersionSampler(gen_) / SCALE);

    // allocate a float like this so it doesn't get destroyed when we leave scope
    // (we need it later in the particle body)
    float* mPower = new float;
    mPower[0] = 0.0;


    // Main engine
    if ((continuous_ && action.front() > 0.0) || (!continuous_ && ((int)action.front()) == 2)) {
        if (continuous_) {
            mPower[0] = 0.5 * clip(action.front(), 0.0, 1.0);
            assert (mPower[0] >= 0.5 && mPower[0] <= 1.0);
        }
        else {
            mPower[0] = 1.0;
        }

        // 4 is move a bit downwards, +-2 for randomness
        float ox = tip.x * (4/SCALE + 2*dispersion.x + side.x*dispersion.y);
        float oy = -tip.y * (4/SCALE + 2*dispersion.x - side.y*dispersion.y);

        b2Vec2 impulsePos(lander_->GetPosition().x + ox, lander_->GetPosition().y + oy);
        b2Body* particle = createParticle(3.5, impulsePos.x, impulsePos.y, mPower);

        // newton's 3rd law. cool.
        particle->ApplyLinearImpulse(b2Vec2(ox*MAIN_ENGINE_POWER*mPower[0], oy*MAIN_ENGINE_POWER*mPower[0]), impulsePos, true);
        lander_->ApplyLinearImpulse(b2Vec2(-ox*MAIN_ENGINE_POWER*mPower[0], -oy*MAIN_ENGINE_POWER*mPower[0]), impulsePos, true);
    }

    // Side engines
    // allocate a float like this so it doesn't get destroyed when we leave scope
    // (we need it later in the particle body)
    float* sPower = new float;
    sPower[0] = 0.0;

    if ( (continuous_ && std::abs(action.back() > 0.5)) || ((!continuous_ && ((int)action.front() == 1 || (int)action.front() == 3) )) ) {
        int direction;
        if (continuous_) {
            direction = 1 + -2*std::signbit(action.back());
            sPower[0] = clip(std::abs(action.back()), 0.5, 1.0);
            assert(sPower[0] >= 0.5 && sPower[0] <= 1.0);
        }
        else {
            direction = ((int)action.front())-2;
            sPower[0] = 1.0;
        }


        float ox = tip.x * dispersion.x + side.x*(3*dispersion.y + direction * SIDE_ENGINE_AWAY/SCALE);
        float oy = -tip.y * dispersion.x - side.y*(3*dispersion.y + direction * SIDE_ENGINE_AWAY/SCALE);

        b2Vec2 landerXY = lander_->GetPosition();
        b2Vec2 impulsePos(landerXY.x + ox - tip.x*17.0/SCALE, landerXY.y + oy + tip.y*SIDE_ENGINE_HEIGHT/SCALE);

        b2Body* particle = createParticle(0.7, impulsePos.x, impulsePos.y, sPower);

        // newton's 3rd law. cool.
        particle->ApplyLinearImpulse(b2Vec2(ox*SIDE_ENGINE_POWER*sPower[0],  oy*SIDE_ENGINE_POWER*sPower[0]), impulsePos, true);
        lander_->ApplyLinearImpulse(b2Vec2(-ox*SIDE_ENGINE_POWER*sPower[0], -oy*SIDE_ENGINE_POWER*sPower[0]), impulsePos, true);
    }

    world_->Step(1.0/FPS, 6*30, 2*30);

    b2Vec2 pos = lander_->GetPosition();
    b2Vec2 vel = lander_->GetLinearVelocity();

    //std::cout << "pos: " << pos.x << " " << pos.y << std::endl;
    std::vector<float> state;
    state.push_back((pos.x - VIEWPORT_W/SCALE/2.0) / (VIEWPORT_W/SCALE/2.0)); // x position
    state.push_back((pos.y - (helipadY_+LEG_DOWN/SCALE)) / (VIEWPORT_H/SCALE/2.0)); // y position (above landing)
    state.push_back(vel.x*(VIEWPORT_W/SCALE/2)/FPS);
    state.push_back(vel.y*(VIEWPORT_H/SCALE/2)/FPS);
    state.push_back(lander_->GetAngle());
    state.push_back(20.0*lander_->GetAngularVelocity()/FPS);
    state.push_back((float) leftLegGroundContact_);
    state.push_back((float) rightLegGroundContact_);
    assert(state.size() == 8);

    float reward = 0.0;

    // Punish (give negative reward) for large position xy and velocity xy values
    // Add ten points for leg contact
    float shaping = - 100*std::sqrt(state.at(0)*state.at(0) + state.at(1)*state.at(1)) \
                    - 100*std::sqrt(state.at(2)*state.at(2) + state.at(3)*state.at(3)) \
                    - 100*std::abs(state.at(4)) + 10*state[6] + 10*state[7];

    // don't keep giving the same large reward for this. give it like once
    if (prevShaping_ != 0.0) {
        reward = shaping - prevShaping_;
    }
    prevShaping_ = shaping;

    reward -= mPower[0]*0.30; // less fuel spent is better, about -30 for heuristic landing
    reward -= sPower[0]*0.03;

    bool done = false;

    if (gameOver_ || std::abs(state.at(0)) >= 1.0) {
        done = true;
        reward = -100; // we crashed or left the boundaries :(
    }
    if (!lander_->IsAwake()) {
        done = true;
        reward = 100; // we landed safely :)
    }

    envData.observation = state;
    envData.reward = reward;
    envData.done = done;
    return envData;
}

/** Renders the game with SFML based on box2D bodies*/
void LunarLander::render() {

    // set viewer if null
    if (viewer_ == nullptr){
        viewer_ = new sf::RenderWindow(sf::VideoMode(600, 400), "test");
    }

    sf::RectangleShape bgShape;
    bgShape.setSize(sf::Vector2f(600,400));
    bgShape.setTexture(&bgTex);
    viewer_->display();
    viewer_->clear(sf::Color().Black);

    viewer_->draw(bgShape);

    //draw moon
    for (int i = 0; i < skyPolys_.size(); i++){
        // draw separate polys
        sf::ConvexShape convex;
        convex.setPointCount(4);
        convex.setFillColor(sf::Color(223, 233, 239));

        std::tuple<b2Vec2, b2Vec2, b2Vec2, b2Vec2> polyTupe = skyPolys_[i];
        convex.setPoint(0, sf::Vector2f(SCALE*std::get<0>(polyTupe).x, VIEWPORT_H-SCALE*std::get<0>(polyTupe).y));
        convex.setPoint(1, sf::Vector2f(SCALE*std::get<1>(polyTupe).x, VIEWPORT_H-SCALE*std::get<1>(polyTupe).y));
        convex.setPoint(2, sf::Vector2f(SCALE*std::get<2>(polyTupe).x, 400.f));
        convex.setPoint(3, sf::Vector2f(SCALE*std::get<3>(polyTupe).x, 400.f));
        viewer_->draw(convex);
    }

    //draw flag poles
    sf::RectangleShape flag1, flag2;
    flag1.setFillColor(sf::Color().White);
    flag2.setFillColor(sf::Color().White);
    flag1.setSize(sf::Vector2f(1, -(helipadY_ + 50)));
    flag2.setSize(sf::Vector2f(1, -(helipadY_ + 50)));
    flag1.setPosition(sf::Vector2f(SCALE*helipadX1_, VIEWPORT_H-helipadY_*SCALE));
    flag2.setPosition(sf::Vector2f(SCALE*helipadX2_, VIEWPORT_H-helipadY_*SCALE));
    viewer_->draw(flag1);
    viewer_->draw(flag2);

    //draw flag triangles
    sf::RectangleShape flagTri1, flagTri2;
    flagTri1.setSize(sf::Vector2f(20,10));
    flagTri2.setSize(sf::Vector2f(20,10));
    flagTri1.setPosition(sf::Vector2f(SCALE*helipadX1_, flag1.getSize().y+VIEWPORT_H-helipadY_*SCALE));
    flagTri2.setPosition(sf::Vector2f(SCALE*helipadX2_, flag2.getSize().y+VIEWPORT_H-helipadY_*SCALE));
    flagTri1.setTexture(&flagTex);
    flagTri2.setTexture(&flagTex);
    viewer_->draw(flagTri1);
    viewer_->draw(flagTri2);

    //draw lander
    sf::ConvexShape ship;
    ship.setPointCount(6);
    ship.setTexture(&shipBodyTex);
    ship.setPoint(0, sf::Vector2f(-14,  -17));
    ship.setPoint(5, sf::Vector2f(-17,  0  ));
    ship.setPoint(4, sf::Vector2f(-17, +10 ));
    ship.setPoint(3, sf::Vector2f(+17, +10 ));
    ship.setPoint(2, sf::Vector2f(+17,  0  ));
    ship.setPoint(1, sf::Vector2f(+14,  -17));
    ship.setPosition(lander_->GetPosition().x*SCALE, 10+VIEWPORT_H-(lander_->GetPosition().y*SCALE));
    ship.rotate(-lander_->GetAngle()*SCALE);
    viewer_->draw(ship);

    //draw legs
    sf::RectangleShape leftLegShape;
    leftLegShape.setSize(sf::Vector2f(2*LEG_W, 2*LEG_H));
    leftLegShape.setPosition(leftLeg_->GetPosition().x*SCALE, 5+VIEWPORT_H-(leftLeg_->GetPosition().y*SCALE));
    leftLegShape.setTexture(&legTex);
    leftLegShape.rotate(-SCALE*leftLeg_->GetAngle());
    viewer_->draw(leftLegShape);

    sf::RectangleShape rightLegShape;
    rightLegShape.setSize(sf::Vector2f(2*LEG_W, 2*LEG_H));
    rightLegShape.setPosition(rightLeg_->GetPosition().x*SCALE, 5+VIEWPORT_H-(rightLeg_->GetPosition().y*SCALE));
    rightLegShape.setTexture(&legTex);
    rightLegShape.rotate(-SCALE*rightLeg_->GetAngle());
    viewer_->draw(rightLegShape);

    //draw particles
    for (std::list<b2Body*>::iterator it=particles_.begin(); it != particles_.end(); ++it)
    {
        b2Body* body = (*it);
        float* power = (float*) body->GetUserData();
        *power -= 0.15;

        sf::CircleShape shape(5);
        shape.setFillColor(sf::Color(255, 255, 255, std::max((int) 0.2*255, (int) (128*(*power)))));
        shape.setTexture(&sparkTex);
        shape.setPosition(sf::Vector2f(SCALE*body->GetPosition().x, VIEWPORT_H-(body->GetPosition().y*SCALE)));
        viewer_->draw(shape);
    }

    // Declare a new font
    sf::Font font;
    // Load it from a file
    font.loadFromFile("../cs3505-f17-a8-edu-app-matwilso/arial.ttf");
    sf::Text rewardText;
    std::string rewardString = "Last Reward: " + std::to_string(lastReward);
    rewardText.setString(rewardString);
    rewardText.setColor(sf::Color().White);
    rewardText.setStyle(sf::Text::Bold);
    rewardText.setCharacterSize(25);
    rewardText.setFont(font);
    viewer_->draw(rewardText);

    //keyboard events
    sf::Event event;
            while (viewer_->pollEvent(event))
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                {
                    viewer_->close();
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up){
                        extern bool input_up, input_left, input_right;
                        input_up = true;
                        input_left = false;
                        input_right = false;
                    }
                    else if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left){
                        extern bool input_up, input_left, input_right;
                        input_up = false;
                        input_left = true;
                        input_right = false;
                    }
                    else if(event.key.code == sf::Keyboard::D|| event.key.code == sf::Keyboard::Right){
                        extern bool input_up, input_left, input_right;
                        input_up = false;
                        input_left = false;
                        input_right = true;
                    }
                }
                else{
                    extern bool input_up, input_left, input_right;
                    input_up = false;
                    input_left = false;
                    input_right = false;
                }
    }

}
