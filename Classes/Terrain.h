#ifndef _TERRAIN_H__
#define _TERRAIN_H__

#include "cocos2d.h"
#include "Box2D\Box2D.h"

using namespace cocos2d;

#define kMaxHillKeyPoints 1000
#define kHillSegmentWidth 10
#define kMaxHillVertices 4000
#define kMaxBorderVertices 800 

class Terrain :
	public Node
{
public:
	Terrain();
	~Terrain();	

	static Terrain* create();
	bool init();

	void setStripes(Sprite *stripes);
	void setOffsetX(float nOffsetX);

	virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
private:
	Sprite *_stripes;
	int _offsetX;
	Vec2 _hillKeyPoints[kMaxHillKeyPoints];
	int _fromKeyPointI;
	int _toKeyPointI;

	int _nHillVertices;
	Vec2 _hillVertices[kMaxHillVertices];
	Vec2 _hillTexCoords[kMaxHillVertices];
	int _nBorderVertices;
	Vec2 _borderVertices[kMaxBorderVertices];

	void generateHills();
	void resetHillVertices();
};

#endif	// _TERRAIN_H__