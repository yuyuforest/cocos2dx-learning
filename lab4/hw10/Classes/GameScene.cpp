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

	//����
	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	//��Ϸʯͷ��
	stoneLayer = Layer::create();
	this->addChild(stoneLayer, 1);
	//��Ϸ�����
	mouseLayer = Layer::create();
	mouseLayer->setPosition(0, visibleSize.height / 2);
	this->addChild(mouseLayer, 2);

	//ʯͷ����
	stone = Sprite::create("stone.png");
	stoneLayer->addChild(stone);
	stone->setPosition(560, 480);
	//this->addChild(stone, 1);

	//������
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	mouseLayer->addChild(mouse);
	mouse->setPosition(visibleSize.width / 2, 0);
	//ʹ������Ķ�����Դ
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));

	//shoot�˵���
	MenuItemFont::setFontName("Times New Roman");
	MenuItemFont::setFontSize(86);
	MenuItemFont *shootMenu = MenuItemFont::create("Shoot",
			CC_CALLBACK_1(GameScene::shootMenuCallback, this));

	//�˵�
	auto menu = Menu::create(shootMenu, NULL);
	menu->setPosition(visibleSize.width / 2 + origin.x + 300, visibleSize.height / 2 + origin.y + 170);
	this->addChild(menu, 3);

	return true;
}

//�����¼�
bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(mouseLayer->convertToNodeSpace(location));
	mouseLayer->addChild(cheese);

	//���������ƶ���֥ʿλ��
	mouse->runAction(MoveTo::create(0.5f, cheese->getPosition()));

	//֥ʿ����������ʧ
	cheese->runAction(FadeOut::create(3.0f));

	return true;
}

//shoot�˵����¼�
void GameScene::shootMenuCallback(Ref* pSender) {
	auto newStone = Sprite::create("stone.png");
	if (newStone != nullptr)
	{
		newStone->setPosition(stone->getPosition());
		//�ڵ�Ŀ��λ�ú�ʯͷӦ�����Ƴ�
		auto callbackShoot = CallFunc::create([newStone]() {
			newStone->removeFromParent();
		});

		//����ʯͷ�ᱻ�Ƴ��Ķ���ִ������
		Vec2 v1 = mouseLayer->convertToWorldSpace(mouse->getPosition());
		Vec2 newStonePos = stoneLayer->convertToNodeSpace(v1);
		MoveTo* moveTo = MoveTo::create(0.5f, newStonePos);
		auto sequence = Sequence::create(moveTo, callbackShoot, nullptr);
		stoneLayer->addChild(newStone);
		newStone->runAction(sequence);

		//������ʯ
		auto diamond = Sprite::create("diamond.png");
		diamond->setPosition(newStonePos);
		stoneLayer->addChild(diamond);

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		//���ĵ���������Ҫ����ˣ�ȥ׷�����ˣ�
		Vec2 newMousePos = Vec2(origin.x + rand() % int(visibleSize.width), origin.y + rand() % int(visibleSize.height));
		mouse->runAction(MoveTo::create(0.5f, mouseLayer->convertToNodeSpace(newMousePos)));
	}
}

