#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemiesNum = Label::createWithTTF("enemies: 0", "fonts/arial.TTF", 20);
	enemiesNum->setColor(Color3B(255, 255, 255));
	enemiesNum->setPosition(50, 60);
	this->addChild(enemiesNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	TextureCache::getInstance()->addImageAsync("bullet.png", NULL);

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	// Todo
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explode.wav");
	audio->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	// Todo
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemies.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemies.push_back(enemy);
			addChild(enemy, 1);
		}
	}
	stoneType = 3;
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	// Todo
	for (auto stone : enemies) {
		stone->setPosition(stone->getPositionX(), stone->getPositionY() - 50);
	}

	stoneType++;
	stoneType %= 3;
	if (stoneType == 0) stoneType = 3;
	char enemyPath[20];
	sprintf(enemyPath, "stone%d.png", stoneType);
	double width = visibleSize.width / 6,
		height = visibleSize.height - 50;
	for (int i = 0; i < 5; i++) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width / 2 + width * i, height);
		enemies.push_back(enemy);
		addChild(enemy, 1);
	}
}

// 移动飞船
void Thunder::movePlane(char c) {
	// Todo

	Vec2 distance = Vec2::ZERO;	//要移动的向量
	float length = 15.0f;		//步长
	switch (c) {
	case 'A':
		distance = Vec2(-length, 0);
		break;
	case 'D':
		distance = Vec2(length, 0);
		break;
	default:
		break;
	}


	Vec2 pos = player->getPosition();
	pos.add(distance);

	player->runAction(MoveBy::create(0.1f, distance));
}

//发射子弹
void Thunder::fire() {
	//auto bullet = Sprite::create("bullet.png");
	auto bullet = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey("bullet.png"));
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	float height = bullet->getBoundingBox().getMaxY() - bullet->getBoundingBox().getMinY();
	float newH = visibleSize.height + height / 2;

	// 子弹飞出
	MoveTo* moveTo = MoveTo::create(0.8f, Vec2(bullet->getPositionX(), newH));
	bullet->runAction(moveTo);

	// 移除飞出屏幕外的子弹
	// Todo
}

// 切割爆炸动画帧
void Thunder::explosion() {
	// Todo

	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");

	// 切割第一排
	explode.reserve(9);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(195 * i + 2, 0, 138, 138)));
		explode.pushBack(frame);
	}
	// 切割第二排
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(195 * i + 2, 195, 138, 138)));
		explode.pushBack(frame);
	}
	auto explodeAnimation = Animation::createWithSpriteFrames(explode, 0.1f);
	AnimationCache::getInstance()->addAnimation(explodeAnimation, "explode");
}

// 陨石爆炸
void Thunder::Boom(Sprite* enemy) {
	Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("explode"));
	SimpleAudioEngine::getInstance()->playEffect("music/explode.wav", false);
	enemy->runAction(animate);
}

void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemies: %d", enemies.size());
	enemiesNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);


	// 防止飞船飞出窗口
	float x = player->getPosition().x;
	float x1 = x;
	float x2 = x;
	float width = player->getContentSize().width;
	x1 -= width;	//pos1记录player的左边
	x2 += width;	//pos2记录player的右边

	//窗口的范围
	int xMin = 0;
	int xMax = visibleSize.width;

	//如果新的player的位置在可视窗口内，就移动player
	if (x1 < xMin && movekey == 'A' || x2 > xMax && movekey == 'D') {
		player->stopAllActions();
	}
	else if (isMove){
		this->movePlane(movekey);
	}

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemies) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);
	EventCustom e2("meet1");
	_eventDispatcher->dispatchEvent(&e2);
	EventCustom e3("meetBorder");
	_eventDispatcher->dispatchEvent(&e3);
}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	// Todo

	auto iterEnemy = enemies.begin();
	// 遍历陨石
	for (; iterEnemy != enemies.end(); iterEnemy++) {
		if (*iterEnemy == nullptr) continue;
		auto iterBullet = bullets.begin();
		// 遍历子弹
		for (; iterBullet != bullets.end(); iterBullet++) {
			if (*iterBullet == nullptr || *iterEnemy == nullptr) continue;
			float distance = (*iterEnemy)->getPosition().getDistance((*iterBullet)->getPosition());
			if (distance < 25) {
				(*iterBullet)->removeFromParentAndCleanup(true);
				*iterBullet = nullptr;
				Boom(*iterEnemy);
				*iterEnemy = nullptr;
			}
		}
	}
	enemies.remove(nullptr);
}

// 自定义子弹飞出窗口事件
void Thunder::meet1(EventCustom* event) {
	for (auto iter = bullets.begin(); iter != bullets.end(); iter++) {
		if (*iter != nullptr) {
			float height = (*iter)->getBoundingBox().size.height;
			float newH = visibleSize.height + height / 2;
			if ((*iter)->getPositionY() >= newH) {
				(*iter)->removeFromParentAndCleanup(true);
				*iter = nullptr;
			}
		}
	}
	bullets.remove(nullptr);
}

// 检测陨石是否即将出界
void Thunder::meetBorder(EventCustom* event) {
	bool over = false;
	for (auto stone : enemies) {
		if (stone->getPositionY() < 50) {
			over = true;
			break;
		}
	}
	if (over) stopAc();
}

// 结束游戏
void Thunder::stopAc() {
    // Todo
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explode.wav", false);

	Sprite* p = player;
	CallFunc* callback = CallFunc::create([p](){
		p->removeFromParentAndCleanup(true); 
	});
	Sequence* seq = Sequence::create(Animate::create(AnimationCache::getInstance()->getAnimation("explode")), 
		callback, nullptr);
	player->runAction(seq);
	
	unscheduleAllSelectors();
	_eventDispatcher->removeAllEventListeners();
	MenuItemImage* gameOver = MenuItemImage::create("gameOver.png", "gameOver.png");
	gameOver->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(gameOver, 3);

	audio->stopBackgroundMusic();
	audio->unloadEffect("music/explode.wav");
	audio->unloadEffect("music/fire.wav");

	TextureCache::getInstance()->removeAllTextures();
}

// 添加自定义监听器
void Thunder::addCustomListener() {
	// Todo

	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
	auto meet1Listener = EventListenerCustom::create("meet1", CC_CALLBACK_1(Thunder::meet1, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meet1Listener, 1);
	auto meetBorderListener = EventListenerCustom::create("meetBorder", CC_CALLBACK_1(Thunder::meetBorder, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetBorderListener, 1);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	// Todo

	//监听键盘，由键盘操纵移动
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 按键事件
void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

// 释放按键事件
void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	// Todo

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30){
		isClick = true;
		fire();
	}
    // Todo
	return isClick;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	// Todo
	if (isClick) {
		Vec2 delta = touch->getDelta();
		player->setPositionX(player->getPositionX() + delta.x);
	}
}
