
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
	_command.init(rt->getGlobalZOrder());
	_command.func = CC_CALLBACK_0(HelloWorld::onDrawGradient, this);
	auto renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_command);

	//// noise cloud
	Sprite *noise = Sprite::create("Noise.png");
	noise->setBlendFunc(BlendFunc{ GL_DST_COLOR, GL_ZERO });
	noise->setPosition(Vec2(textureWidth / 2, textureHeight / 2));
	noise->visit();

	// 4: Call CCRenderTexture:end
	rt->end();

	// 5: Create a new Sprite from the texture
	return Sprite::createWithTexture(rt->getSprite()->getTexture());
}

void HelloWorld::onDrawGradient()
{
	setGLProgram(ShaderCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins();

	Size winSize = Director::getInstance()->getWinSize();

	float gradientAlpha = 0.7f;

	int nVertices = 0;
	std::vector<Vec2> vertices;
	std::vector<Color4F> colors;

	vertices.push_back(Vec2(0, 0));
	colors.push_back(Color4F{ 0, 0, 0, 0 });

	vertices.push_back(Vec2(winSize.width, 0));
	colors.push_back(Color4F{ 0, 0, 0, 0 });

	vertices.push_back(Vec2(0, winSize.height));
	colors.push_back(Color4F{ 0, 0, 0, gradientAlpha });

	vertices.push_back(Vec2(winSize.width, winSize.height));
	colors.push_back(Color4F{ 0, 0, 0, gradientAlpha });

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
	_command.init(rt->getGlobalZOrder());
	_command.func = std::bind(&HelloWorld::onDrawStripes, this, c2, textureWidth, textureHeight, nStripes);  //CC_CALLBACK_0(HelloWorld::onDrawStripes, this);
	auto renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_command);

	// noise cloud
	Sprite *noise = Sprite::create("Noise.png");
	noise->setBlendFunc(BlendFunc{ GL_DST_COLOR, GL_ZERO });
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
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });

		vertices.push_back(Vec2(x1 + stripeWidth, y1));
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });

		vertices.push_back(Vec2(x2, y2));
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });

		vertices.push_back(vertices[vertices.size() - 2]);
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });

		vertices.push_back(vertices[vertices.size() - 2]);
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });

		vertices.push_back(Vec2(x2 + stripeWidth, y2));
		colors.push_back(Color4F{ c2.r, c2.g, c2.b, c2.a });
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

	Color4F bgColor = randomBrightColor();
	Color4F color2 = randomBrightColor();
	//_background = spriteWithColor(bgColor, 512, 512);

	int nStripes = ((rand() % 4) + 1) * 2;
	_background = stripedSpriteWithColor(bgColor, color2, 512, 512, nStripes);

	Size winSize = Director::getInstance()->getWinSize();
	_background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

	Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
	_background->getTexture()->setTexParameters(&tp);

	addChild(_background);
}

void HelloWorld::onEnter()
{
	Layer::onEnter();

	genBackground();

	// Enable Touches/Mouse.
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	scheduleUpdate();
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	genBackground();
}

void HelloWorld::update(float dt)
{
	float PIXELS_PER_SECOND = 100;
	static float offset = 0;
	offset += PIXELS_PER_SECOND * dt;

	Size textureSize = _background->getTextureRect().size;
	_background->setTextureRect(Rect(offset, 0, textureSize.width, textureSize.height));
}