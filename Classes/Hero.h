#ifndef __HERO_H__
#define __HERO_H__

#include "cocos2d.h"
#include "Box2D\Box2D.h"

#define NUM_PREV_VELS   5

using namespace cocos2d;

class  Hero : public Sprite {
public:
	static Hero* create(b2World *world);
	bool initWithWorld(b2World *world);
	void update();

	bool isAwake();
	void wake();
	void dive();
	void limitVelocity();

	void nodive();
	void runForceAnimation();
	void runNormalAnimation();
private:
	b2World *_world;
	b2Body *_body;
	bool _awake;

	b2Vec2 _prevVels[NUM_PREV_VELS];
	int _nextVel;

	Animation *_normalAnim;
	Animate *_normalAnimate;

	void createBody();
};

#endif