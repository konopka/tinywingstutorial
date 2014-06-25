#include "Terrain.h"

Terrain::Terrain()
{
	_stripes = nullptr;
	_fromKeyPointI = 0;
	_toKeyPointI = 0;

	_world = nullptr;
	_body = nullptr;
}

Terrain::~Terrain()
{
	if (_stripes) {
		_stripes->release();
		_stripes = nullptr;
	}

	if (_body) {
		_world->DestroyBody(_body);
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

	setShaderProgram(ShaderCache::getInstance()->programForKey(GLProgram::SHADER_NAME_POSITION_TEXTURE));
	generateHills();
	resetHillVertices();

	return true;
}

Terrain* Terrain::create(b2World *world)
{
	Terrain* ret = new Terrain();
	if (ret && ret->initWithWorld(world)) {
		return ret;
	}

	return nullptr;
}

bool Terrain::initWithWorld(b2World *world)
{
	if (!Node::init()) {
		return false;
	}

	_world = world;
	setupDebugDraw();
	generateHills();
	resetHillVertices();

	setShaderProgram(ShaderCache::getInstance()->programForKey(GLProgram::SHADER_NAME_POSITION_TEXTURE));

	return true;
}

void Terrain::setupDebugDraw()
{
	_debugDraw = new GLESDebugDraw(PTM_RATIO);
	_world->SetDebugDraw(_debugDraw);
	_debugDraw->SetFlags(GLESDebugDraw::e_shapeBit | GLESDebugDraw::e_jointBit);
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
	CC_NODE_DRAW_SETUP();

	GL::bindTexture2D(_stripes->getTexture()->getName());
	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD);

	ccDrawColor4F(1.0f, 1.0f, 1.0f, 1.0f);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, _hillVertices);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, _hillTexCoords);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)_nHillVertices);

	for (int i = MAX(_fromKeyPointI, 1); i <= _toKeyPointI; ++i) {
		DrawPrimitives::setDrawColor4F(1.0, 0, 0, 1.0);
		DrawPrimitives::drawLine(_hillKeyPoints[i - 1], _hillKeyPoints[i]);

		DrawPrimitives::setDrawColor4F(1.0, 1.0, 1.0, 1.0);

		Vec2 p0 = _hillKeyPoints[i - 1];
		Vec2 p1 = _hillKeyPoints[i];
		int hSegments = floorf((p1.x - p0.x) / kHillSegmentWidth);
		float dx = (p1.x - p0.x) / hSegments;
		float da = M_PI / hSegments;
		float ymid = (p0.y + p1.y) / 2;
		float ampl = (p0.y - p1.y) / 2;

		Vec2 pt0, pt1;
		pt0 = p0;

		for (int j = 0; j < hSegments + 1; ++j) {
			pt1.x = p0.x + j*dx;
			pt1.y = ymid + ampl * cosf(da*j);

			ccDrawLine(pt0, pt1);

			pt0 = pt1;
		}
	}

	_world->DrawDebugData();
}

void Terrain::setOffsetX(float nOffsetX)
{
	_offsetX = nOffsetX;
	setPosition(Vec2(-_offsetX*getScale(), 0));

	resetHillVertices();
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

void Terrain::resetHillVertices()
{
	Size winSize = Director::getInstance()->getWinSize();

	static int prevFromKeyPointI = -1;
	static int prevToKeyPointI = -1;

	// key points interval for drawing
	while (_hillKeyPoints[_fromKeyPointI + 1].x < _offsetX - winSize.width / 8 / getScale()) {
		_fromKeyPointI++;
	}
	while (_hillKeyPoints[_toKeyPointI].x < _offsetX + winSize.width * 12 / 8 / getScale()) {
		_toKeyPointI++;
	}

	float minY = 0;
	if (winSize.height > 480) {
		minY = (1136 - 1024) / 4;
	}

	if (prevFromKeyPointI != _fromKeyPointI || prevToKeyPointI != _toKeyPointI) {
		// vertices for visible area
		_nHillVertices = 0;
		_nBorderVertices = 0;
		Vec2 p0, p1, pt0, pt1;
		p0 = _hillKeyPoints[_fromKeyPointI];
		for (int i = _fromKeyPointI + 1; i<_toKeyPointI + 1; i++) {
			p1 = _hillKeyPoints[i];

			// triangle strip between p0 and p1
			int hSegments = floorf((p1.x - p0.x) / kHillSegmentWidth);
			float dx = (p1.x - p0.x) / hSegments;
			float da = M_PI / hSegments;
			float ymid = (p0.y + p1.y) / 2;
			float ampl = (p0.y - p1.y) / 2;
			pt0 = p0;
			_borderVertices[_nBorderVertices++] = pt0;
			for (int j = 1; j<hSegments + 1; j++) {
				pt1.x = p0.x + j*dx;
				pt1.y = ymid + ampl * cosf(da*j);
				_borderVertices[_nBorderVertices++] = pt1;

				_hillVertices[_nHillVertices] = Vec2(pt0.x, 0 + minY);
				_hillTexCoords[_nHillVertices++] = Vec2(pt0.x / 512, 1.0f);
				_hillVertices[_nHillVertices] = Vec2(pt1.x, 0 + minY);
				_hillTexCoords[_nHillVertices++] = Vec2(pt1.x / 512, 1.0f);

				_hillVertices[_nHillVertices] = Vec2(pt0.x, pt0.y);
				_hillTexCoords[_nHillVertices++] = Vec2(pt0.x / 512, 0);
				_hillVertices[_nHillVertices] = Vec2(pt1.x, pt1.y);
				_hillTexCoords[_nHillVertices++] = Vec2(pt1.x / 512, 0);

				pt0 = pt1;
			}

			p0 = p1;
		}

		prevFromKeyPointI = _fromKeyPointI;
		prevToKeyPointI = _toKeyPointI;

		resetBox2DBody();
	}
}

void Terrain::resetBox2DBody()
{
	if (_body) {
		_world->DestroyBody(_body);
	}

	b2BodyDef bd;
	bd.position.Set(0, 0);

	_body = _world->CreateBody(&bd);

	b2EdgeShape shape;

	b2Vec2 p1, p2;
	for (int i = 0; i<_nBorderVertices - 1; i++) {
		p1 = b2Vec2(_borderVertices[i].x / PTM_RATIO, _borderVertices[i].y / PTM_RATIO);
		p2 = b2Vec2(_borderVertices[i + 1].x / PTM_RATIO, _borderVertices[i + 1].y / PTM_RATIO);
		shape.Set(p1, p2);
		_body->CreateFixture(&shape, 0);
	}
}