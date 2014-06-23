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

	float minDX = 160;
	float minDY = 60;
	int rangeDX = 80;
	int rangeDY = 40;

	float x = -minDX;
	float y = winSize.height / 2;

	float dy, ny;
	float sign = 1; // +1 - going up, -1 - going  down
	float paddingTop = 20;
	float paddingBottom = 20;

	for (int i = 0; i<kMaxHillKeyPoints; i++) {
		_hillKeyPoints[i] = Vec2(x, y);

		if (i == 0) {
			x = 0;
			y = winSize.height / 2;
		}
		else {
			x += rand() % rangeDX + minDX;

			while (true) {
				dy = rand() % rangeDY + minDY;
				ny = y + dy*sign;

				if (ny < winSize.height - paddingTop && ny > paddingBottom) {
					break;
				}
			}

			y = ny;
		}

		sign *= -1;
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
