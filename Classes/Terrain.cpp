#include "Terrain.h"

Terrain::Terrain()
{
	_stripes = nullptr;
}

Terrain::~Terrain()
{
	if (_stripes) {
		_stripes->release();
		_stripes = nullptr;
	}
}

Terrain* Terrain::create()
{
	Terrain* ret = new Terrain();
	if (ret && ret->init()) {
		return ret;
	}

	return nullptr;
}

bool Terrain::init()
{
	if (!Node::init()) {
		return false;
	}

	generateHills();

	return true;
}

void Terrain::generateHills()
{
	Size winSize = Director::getInstance()->getWinSize();
	float x = 0;
	float y = winSize.height / 2;
	for (int i = 0; i < kMaxHillKeyPoints; ++i) {
		_hillKeyPoints[i] = Vec2(x, y);
		x += winSize.width / 2;
		y = (rand() % (int)winSize.height);
	}
}

void Terrain::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	for (int i = 1; i < kMaxHillKeyPoints; ++i) {
		DrawPrimitives::drawLine(_hillKeyPoints[i - 1], _hillKeyPoints[i]);
	}
}

void Terrain::setOffsetX(float nOffsetX)
{
	_offsetX = nOffsetX;
	setPosition(Vec2(-_offsetX*getScale(), 0));
}

void Terrain::setStripes(Sprite *stripes)
{
	if (_stripes) {
		_stripes->release();
	}

	_stripes = stripes;

	if (_stripes) {
		_stripes->retain();
	}
}
