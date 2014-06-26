
#include "Hero.h"
#include "HelloWorldScene.h"

void Hero::createBody()
{
	float radius = 16.0f;
	Size size = Director::getInstance()->getWinSize();
	int screenH = size.height;

	Vec2 startPosition = ccp(0, screenH / 2 + radius);

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.linearDamping = 0.1f;
	bd.fixedRotation = true;
	bd.position.Set(startPosition.x / PTM_RATIO, startPosition.y / PTM_RATIO);
	_body = _world->CreateBody(&bd);

	b2CircleShape shape;
	shape.m_radius = radius / PTM_RATIO;

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 1.0f / CC_CONTENT_SCALE_FACTOR();
	fd.restitution = 0.0f;
	fd.friction = 0.2;

	_body->CreateFixture(&fd);
}

Hero* Hero::create(b2World *world)
{
	Hero* ret = new Hero();
	if (ret && ret->initWithWorld(world)) {
		return ret;
	}

	return nullptr;
}

bool Hero::initWithWorld(b2World *world)
{
	if (!initWithSpriteFrameName("seal1.png")) {
		return false;
	}

	_world = world;
	createBody();

	_normalAnim = Animation::create();
	_normalAnim->addSpriteFrame(SpriteFrameCache::getInstance()->spriteFrameByName("seal1.png"));
	_normalAnim->addSpriteFrame(SpriteFrameCache::getInstance()->spriteFrameByName("seal2.png"));
	_normalAnim->setDelayPerUnit(0.1);

	_nextVel = 0;

	return true;
}

void Hero::update()
{
	setPosition(Vec2(_body->GetPosition().x*PTM_RATIO, _body->GetPosition().y*PTM_RATIO));
	b2Vec2 vel = _body->GetLinearVelocity();
	b2Vec2 weightedVel = vel;

	for (int i = 0; i < NUM_PREV_VELS; ++i) {
		weightedVel += _prevVels[i];
	}

	weightedVel = b2Vec2(weightedVel.x / NUM_PREV_VELS, weightedVel.y / NUM_PREV_VELS);
	_prevVels[_nextVel++] = vel;
	if (_nextVel >= NUM_PREV_VELS) _nextVel = 0;

	float angle = Vec2(weightedVel.x, weightedVel.y).getAngle();
	if (_awake) {
		setRotation(-1 * CC_RADIANS_TO_DEGREES(angle));
	}
}

bool Hero::isAwake()
{
	return _awake;
}

void Hero::wake()
{
	_awake = true;
	_body->SetActive(true);
	_body->ApplyLinearImpulse(b2Vec2(1, 2), _body->GetPosition(), true);
}

void Hero::dive()
{
	_body->ApplyForce(b2Vec2(5, -50), _body->GetPosition(), true);
}

void Hero::limitVelocity()
{
	if (!_awake) return;

	const float minVelocityX = 5;
	const float minVelocityY = -40;
	b2Vec2 vel = _body->GetLinearVelocity();
	if (vel.x < minVelocityX) {
		vel.x = minVelocityX;
	}
	if (vel.y < minVelocityY) {
		vel.y = minVelocityY;
	}
	_body->SetLinearVelocity(vel);
}

void Hero::runNormalAnimation()
{
	if (_normalAnimate || !_awake) return;
	_normalAnimate = Animate::create(_normalAnim);
	runAction(RepeatForever::create(_normalAnimate));
}

void Hero::runForceAnimation()
{
	_normalAnimate->stop();
	_normalAnimate = nullptr;
	setDisplayFrame(SpriteFrameCache::getInstance()->spriteFrameByName("seal_downhill.png"));
}

void Hero::nodive()
{
	runNormalAnimation();
}