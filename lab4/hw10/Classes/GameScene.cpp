#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}


	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//背景
	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	//游戏石头层
	stoneLayer = Layer::create();
	this->addChild(stoneLayer, 1);
	//游戏老鼠层
	mouseLayer = Layer::create();
	mouseLayer->setPosition(0, visibleSize.height / 2);
	this->addChild(mouseLayer, 2);

	//石头精灵
	stone = Sprite::create("stone.png");
	stoneLayer->addChild(stone);
	stone->setPosition(560, 480);
	//this->addChild(stone, 1);

	//老鼠精灵
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	mouseLayer->addChild(mouse);
	mouse->setPosition(visibleSize.width / 2, 0);
	//使用老鼠的动画资源
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));

	//shoot菜单项
	MenuItemFont::setFontName("Times New Roman");
	MenuItemFont::setFontSize(86);
	MenuItemFont *shootMenu = MenuItemFont::create("Shoot",
			CC_CALLBACK_1(GameScene::shootMenuCallback, this));

	//菜单
	auto menu = Menu::create(shootMenu, NULL);
	menu->setPosition(visibleSize.width / 2 + origin.x + 300, visibleSize.height / 2 + origin.y + 170);
	this->addChild(menu, 3);

	return true;
}

//触摸事件
bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(mouseLayer->convertToNodeSpace(location));
	mouseLayer->addChild(cheese);

	//老鼠动作：移动到芝士位置
	mouse->runAction(MoveTo::create(0.5f, cheese->getPosition()));

	//芝士动作：逐渐消失
	cheese->runAction(FadeOut::create(3.0f));

	return true;
}

//shoot菜单项事件
void GameScene::shootMenuCallback(Ref* pSender) {
	auto newStone = Sprite::create("stone.png");
	if (newStone != nullptr)
	{
		newStone->setPosition(stone->getPosition());
		//在到目的位置后，石头应当被移除
		auto callbackShoot = CallFunc::create([newStone]() {
			newStone->removeFromParent();
		});

		//创建石头会被移除的动作执行序列
		Vec2 v1 = mouseLayer->convertToWorldSpace(mouse->getPosition());
		Vec2 newStonePos = stoneLayer->convertToNodeSpace(v1);
		MoveTo* moveTo = MoveTo::create(0.5f, newStonePos);
		auto sequence = Sequence::create(moveTo, callbackShoot, nullptr);
		stoneLayer->addChild(newStone);
		newStone->runAction(sequence);

		//留下钻石
		auto diamond = Sprite::create("diamond.png");
		diamond->setPosition(newStonePos);
		stoneLayer->addChild(diamond);

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		//热心的老鼠先生要结婚了！去追逐爱情了！
		Vec2 newMousePos = Vec2(origin.x + rand() % int(visibleSize.width), origin.y + rand() % int(visibleSize.height));
		mouse->runAction(MoveTo::create(0.5f, mouseLayer->convertToNodeSpace(newMousePos)));
	}
}

