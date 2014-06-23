
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


	// 4: Call CCRenderTexture:end
	rt->end();

	// 5: Create a new Sprite from the texture
	return Sprite::createWithTexture(rt->getSprite()->getTexture());
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
	_background = spriteWithColor(bgColor, 512, 512);
	
	Size winSize = Director::getInstance()->getWinSize();
	_background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

	addChild(_background, -1);
}

void HelloWorld::onEnter()
{
	Layer::onEnter();

	genBackground();

	// Enable Touches/Mouse.
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	genBackground();
}
