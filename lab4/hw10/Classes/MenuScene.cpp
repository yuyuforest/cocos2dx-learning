#include "MenuScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	//吹口哨动画
	auto whistle = Sprite::createWithSpriteFrameName("miner-face-whistle-0.png");
	Animate* whistleAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("whistleAnimation"));
	whistle->runAction(RepeatForever::create(whistleAnimate));
	whistle->setPosition(163 + origin.x, 364 + origin.y);
	this->addChild(whistle, 1);

	//按钮
	auto title = Sprite::create("gold-miner-text.png");
	title->setPosition(origin.x + visibleSize.width / 2, visibleSize.height / 2 + origin.y + 180);
	this->addChild(title, 1);

	//作为按钮背景的金子
	auto gold = Sprite::create("menu-start-gold.png");
	gold ->setPosition(origin.x + visibleSize.width / 2 + 200, visibleSize.height / 2 + origin.y - 150);
	this->addChild(gold, 1);

	auto startMenu = MenuItemImage::create(
		"start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuScene::startMenuCallback, this));
	auto menu = Menu::create(startMenu, NULL);
	menu->setPosition(origin.x + visibleSize.width / 2 + 200, visibleSize.height / 2 + origin.y - 100);
	this->addChild(menu, 1);

    return true;
}

void MenuScene::startMenuCallback(Ref* pSender) {
	auto scene = GameScene::create();
	Director::getInstance()->replaceScene(TransitionPageTurn::create(0.5f, scene, true));
}