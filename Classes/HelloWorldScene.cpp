
#include "HelloWorldScene.h"


Scene* HelloWorld::createScene()
{
	auto scene = Scene::create();
	auto layer = HelloWorld::create();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	_background = nullptr;

	return true;
}

Sprite* HelloWorld::spriteWithColor(Color4F bgColor, float textureWidth, float textureHeight)
{
	// 1: Create new RenderTexture
	RenderTexture *rt = RenderTexture::create(textureWidth, textureHeight);

	// 2: Call RenderTexture:begin
	rt->beginWithClear(bgColor.r, bgColor.g, bgColor.b, bgColor.a);

	// 3: Draw into the texture

	//// gradient
	_gradient_command.init(rt->getGlobalZOrder());
	_gradient_command.func = std::bind(&HelloWorld::onDrawGradient, this, textureWidth, textureHeight);
	auto renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_gradient_command);

	//// noise cloud
	BlendFunc blendFunc;
	blendFunc.src = GL_DST_COLOR;
	blendFunc.dst = GL_ZERO;

	Sprite *noise = Sprite::create("Noise.png");
	noise->setBlendFunc(blendFunc);
	noise->setPosition(Vec2(textureWidth / 2, textureHeight / 2));
	noise->visit();

	// 4: Call CCRenderTexture:end
	rt->end();

	// 5: Create a new Sprite from the texture
	return Sprite::createWithTexture(rt->getSprite()->getTexture());
}

void HelloWorld::onDrawGradient(float textureWidth, float textureHeight)
{
	setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins();

	float gradientAlpha = 0.8f;

	int nVertices = 0;
	std::vector<Vec2> vertices;
	std::vector<Color4F> colors;

	vertices.push_back(Vec2(0, 0));
	colors.push_back(Color4F( 0, 0, 0, 0 ));

	vertices.push_back(Vec2(textureWidth, 0));
	colors.push_back(Color4F( 0.0f, 0.0f, 0.0f, 0.0f ));

	vertices.push_back(Vec2(0, textureHeight));
	colors.push_back(Color4F( 0, 0, 0, gradientAlpha ));

	vertices.push_back(Vec2(textureWidth, textureHeight));
	colors.push_back(Color4F( 0, 0, 0, gradientAlpha ));

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, colors.data());
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size());
}

Sprite* HelloWorld::stripedSpriteWithColor(Color4F c1, Color4F c2, float textureWidth, float textureHeight, int nStripes)
{
	// 1: Create new RenderTexture
	RenderTexture *rt = RenderTexture::create(textureWidth, textureHeight);

	// 2: Call RenderTexture:begin
	rt->beginWithClear(c1.r, c1.g, c1.b, c1.a);

	// 3: Draw into the texture

	// we have to run custom command on renderer
	_stripes_command.init(rt->getGlobalZOrder());
	_stripes_command.func = std::bind(&HelloWorld::onDrawStripes, this, c2, textureWidth, textureHeight, nStripes);
	auto renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_stripes_command);

	// gradient
	_gradient_command.init(rt->getGlobalZOrder());
	_gradient_command.func = std::bind(&HelloWorld::onDrawGradient, this, textureWidth, textureHeight);
	renderer->addCommand(&_gradient_command);

	// gradient
	_tophighlight_command.init(rt->getGlobalZOrder());
	_tophighlight_command.func = std::bind(&HelloWorld::onDrawTopHighlight, this, textureWidth, textureHeight);
	renderer->addCommand(&_tophighlight_command);

	// noise cloud
	BlendFunc blendFunc;
	blendFunc.src = GL_DST_COLOR;
	blendFunc.dst = GL_ZERO;

	Sprite *noise = Sprite::create("Noise.png");
	noise->setBlendFunc(blendFunc);
	noise->setPosition(Vec2(textureWidth / 2, textureHeight / 2));
	noise->visit();

	// 4: Call CCRenderTexture:end
	rt->end();

	// 5: Create a new Sprite from the texture
	return Sprite::createWithTexture(rt->getSprite()->getTexture());
}

void HelloWorld::onDrawStripes(Color4F c2, float textureWidth, float textureHeight, int nStripes)
{
	std::vector<Vec2> vertices;
	std::vector<Color4F> colors;

	float x1 = -textureHeight;
	float x2;
	float y1 = textureHeight;
	float y2 = 0;
	float dx = textureWidth / nStripes * 2;
	float stripeWidth = dx / 2;
	for (int i = 0; i < nStripes; i++) {
		x2 = x1 + textureHeight;

		vertices.push_back(Vec2(x1, y1));
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));

		vertices.push_back(Vec2(x1 + stripeWidth, y1));
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));

		vertices.push_back(Vec2(x2, y2));
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));

		vertices.push_back(vertices[vertices.size() - 2]);
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));

		vertices.push_back(vertices[vertices.size() - 2]);
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));

		vertices.push_back(Vec2(x2 + stripeWidth, y2));
		colors.push_back(Color4F( c2.r, c2.g, c2.b, c2.a ));
		x1 += dx;
	}

	setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins();

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, 0, colors.data());
	GL::blendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
}

void HelloWorld::onDrawTopHighlight(float textureWidth, float textureHeight)
{
	float borderHeight = textureHeight / 16;
	float borderAlpha = 0.3f;

	std::vector<Vec2> vertices;
	std::vector<Color4F> colors;

	vertices.push_back(Vec2(0, 0));
	colors.push_back(Color4F( 1, 1, 1, borderAlpha ));
	
	vertices.push_back(Vec2(textureWidth, 0));
	colors.push_back(Color4F( 1, 1, 1, borderAlpha ));

	vertices.push_back(Vec2(0, borderHeight));
	colors.push_back(Color4F( 0, 0, 0, 0 ));

	vertices.push_back(Vec2(textureWidth, borderHeight));
	colors.push_back(Color4F( 0, 0, 0, 0 ));

	setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins();

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, 0, colors.data());
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size());
}

Color4F HelloWorld::randomBrightColor()
{
	while (true) {
		float requiredBrightness = 192;
		Color4B randomColor =
			Color4B(rand() % 255,
			rand() % 255,
			rand() % 255,
			255);
		if (randomColor.r > requiredBrightness ||
			randomColor.g > requiredBrightness ||
			randomColor.b > requiredBrightness) {
			return Color4F(randomColor);
		}
	}
}

void HelloWorld::genBackground()
{
	if (_background) {
		_background->removeFromParentAndCleanup(true);
	}

	Size winSize = Director::getInstance()->getWinSize();

	Color4F bgColor = randomBrightColor();
	_background = spriteWithColor(bgColor, 512, 512);
	_background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	_background->getTexture()->setTexParameters(&tp);

	//_background->setContentSize( CCSize(winSize.width  , winSize.height) );
	_background->setScale(winSize.width / _background->getContentSize().width, winSize.height / _background->getContentSize().height);

	addChild(_background, 0);
	_background->setVisible(true);

	Color4F color3 = randomBrightColor();
	Color4F color4 = randomBrightColor();
	Sprite *stripes = stripedSpriteWithColor(color3, color4, 512, 512, 4);
	Texture2D::TexParams tp2 = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };
	stripes->getTexture()->setTexParameters(&tp2);
	_terrain->setStripes(stripes);
}

void HelloWorld::setupWorld()
{
	b2Vec2 gravity = b2Vec2(0.0f, -7.0f);
	bool doSleep = true;
	_world = new b2World(gravity);
	_world->SetAllowSleeping(doSleep);
}

void HelloWorld::createTestBodyAtPostition(Vec2 position)
{
	b2BodyDef testBodyDef;
	testBodyDef.type = b2_dynamicBody;
	testBodyDef.position.Set(position.x / PTM_RATIO, position.y / PTM_RATIO);
	b2Body * testBody = _world->CreateBody(&testBodyDef);

	b2CircleShape testBodyShape;
	b2FixtureDef testFixtureDef;
	testBodyShape.m_radius = 25.0 / PTM_RATIO;
	testFixtureDef.shape = &testBodyShape;
	testFixtureDef.density = 1.0;
	testFixtureDef.friction = 0.2;
	testFixtureDef.restitution = 0.5;
	testBody->CreateFixture(&testFixtureDef);
}

void HelloWorld::onEnter()
{
	Layer::onEnter();

	setupWorld();

	_terrain = Terrain::create(_world);
	addChild(_terrain, 1);

	genBackground();

	// Enable Touches/Mouse.
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	listener->onTouchesCancelled = CC_CALLBACK_2(HelloWorld::onTouchesCancelled, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	scheduleUpdate();

	_hero = Hero::create(_world);
	_hero->autorelease();
	_terrain->getBatchNode()->addChild(_hero);	
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	genBackground();

	_tapDown = true;

	Vec2 touchLocation = _terrain->convertTouchToNodeSpace(touches.front());
	createTestBodyAtPostition(touchLocation);
}

void HelloWorld::onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
	_tapDown = false;
}

void HelloWorld::onTouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
{
	_tapDown = false;
}

void HelloWorld::update(float dt)
{
	if (_tapDown) {
		if (!_hero->isAwake()) {
			_hero->wake();
			_tapDown = false;
		}
		else {
			_hero->dive();
		}
	}
	else {
		_hero->nodive();
	}

	_hero->limitVelocity();

	static double UPDATE_INTERVAL = 1.0f / 60.0f;
	static double MAX_CYCLES_PER_FRAME = 5;
	static double timeAccumulator = 0;

	timeAccumulator += dt;
	if (timeAccumulator > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL)) {
		timeAccumulator = UPDATE_INTERVAL;
	}

	int32 velocityIterations = 3;
	int32 positionIterations = 2;
	while (timeAccumulator >= UPDATE_INTERVAL) {
		timeAccumulator -= UPDATE_INTERVAL;
		_world->Step(UPDATE_INTERVAL,
			velocityIterations, positionIterations);
		_world->ClearForces();

	}

	//float PIXELS_PER_SECOND = 100;
	//static float offset = 0;
	//offset += PIXELS_PER_SECOND * dt;
	_hero->update();

	Size winSize = Director::getInstance()->getWinSize();
	float scale = (winSize.height * 3 / 4) / _hero->getPosition().y;
	if (scale > 1) scale = 1;
	_terrain->setScale(scale);

	float offset = _hero->getPosition().x;

	Size textureSize = _background->getTextureRect().size;
	//_background->setTextureRect(Rect(offset, 0, textureSize.width, textureSize.height));
	_terrain->setOffsetX(offset);
}