#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug 模式
  //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();


  auto edgeSp = Sprite::create();  //创建一个精灵
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
  boundBody->setCategoryBitmask(0x00000001);
  boundBody->setCollisionBitmask(0xFFFFFFFF);
  boundBody->setContactTestBitmask(0x00000004);
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // 预加载音效

  addSprite();    // 添加背景和各种精灵
  addListener();  // 添加监听器 
  addPlayer();    // 添加球与板
  BrickGenerated();  // 生成砖块


  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;
  spHolded = false;
  spFactor = 0;
  return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	joint = PhysicsJointPin::construct(player->getPhysicsBody(), ball->getPhysicsBody(),
		Vec2(0, ball->getBoundingBox().getMidY() - player->getBoundingBox().getMidY()),
		Vec2::ZERO);

	m_world->addJoint(joint);
}


// 预加载音效
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);


  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.5f, 1.0f));
  shipbody->setCategoryBitmask(0x00000002);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0x00000008);
  shipbody->setDynamic(false); 
  ship->setPhysicsBody(shipbody);
  ship->setTag(1);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {

  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // 设置板的刚体属性
  // Todo
  auto barBody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(1.1f, 1.0f, 0.5f));
  barBody->setCategoryBitmask(0x00000004);
  barBody->setCollisionBitmask(0x00000008);
  barBody->setContactTestBitmask(0x00000001);
  barBody->setDynamic(false);
  player->setPhysicsBody(barBody);


  player->setTag(2);
  this->addChild(player, 3);

  ball = Sprite::create("ball.png");
  ball->setScale(0.1f, 0.1f);
  float radius = ball->getBoundingBox().size.width / 2;
  ball->setPosition(Vec2(xpos, player->getBoundingBox().getMaxY() + radius));
  // 设置球的刚体属性
  // Todo
  auto ballBody = PhysicsBody::createCircle(ball->getContentSize().height / 2, PhysicsMaterial(5.0f, 1, 0.3f));
  ballBody->setCategoryBitmask(0x00000008);
  ballBody->setCollisionBitmask(0xFFFFFFFF);
  ballBody->setContactTestBitmask(0x00000012);

  ballBody->setRotationEnable(false);
  ball->setPhysicsBody(ballBody);

  ball->setTag(3);
  addChild(ball, 3);

  
}

// 实现简单的蓄力效果
// Todo
void HitBrick::update(float dt) {
	if (spHolded && spFactor <= 600) {
		spFactor += 50;
	}
}



// 生成砖块
// Todo
void HitBrick::BrickGenerated() {

for (int i = 0; i < 3; i++) {
	int cw = 0;
	while (cw <= visibleSize.width) {
		auto box = Sprite::create("box.png");
		// 为砖块设置刚体属性
		// Todo
		auto boxBody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(1000.0f, 1.0f, 1.0f));
		boxBody->setCategoryBitmask(0x00000010);
		boxBody->setCollisionBitmask(0x00000008);
		boxBody->setContactTestBitmask(0x00000008);
		boxBody->setDynamic(false);
		box->setPhysicsBody(boxBody);

		box->setTag(4);

		float w = box->getContentSize().width;
		float h = box->getContentSize().height;

		if (cw == 0) cw += w;
		box->setPosition(cw, visibleSize.height - h / 2 - i * h);
		this->addChild(box,2);
		cw += w;
	}
 }
}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  if(player->getBoundingBox().getMinX() > 3) 
		  player->getPhysicsBody()->setVelocity(Vec2(-400.0f, 0));
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // 左右移动
    // Todo
	  if (player->getBoundingBox().getMaxX() < visibleSize.width - 3) 
		  player->getPhysicsBody()->setVelocity(Vec2(400.0f, 0));
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
	  if (onBall) {
		  spHolded = true;
		  auto particle = ParticleSystemQuad::create("fire.plist");	//添加粒子效果
		  particle->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
		  particle->setPosition(350, 200);
		  particle->setScale(2.0f);
		  ball->addChild(particle, -1);
	  }
      break;
  default:
    break;
  }
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  auto physicsBody = ball->getPhysicsBody();
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // 停止运动
    // Todo
	  player->getPhysicsBody()->setVelocity(Vec2::ZERO);
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // 蓄力结束，小球发射
	  if (onBall) {
		  //小球发射
		  m_world->removeAllJoints();
		  ball->removeAllChildren();	//移去粒子效果
		  physicsBody->setVelocity(Vec2(0, spFactor));
		  onBall = false;
		  spHolded = false;
	  }
    break;

  default:
    break;
  }
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
	auto n1 = contact.getShapeA()->getBody()->getNode();
	auto n2= contact.getShapeB()->getBody()->getNode();

	if (n1 && n2) {
		
		int t1 = n1->getTag();
		int t2 = n2->getTag();

		if (t1 == 1 || t2 == 1) {	//撞到船
			auto particle = ParticleSystemQuad::create("spark.plist");	//添加粒子效果
			particle->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
			particle->setPosition(350, 200);
			particle->setScale(5.0f);
			ball->addChild(particle, -1);
			GameOver();
		}
		if (t1 == 4 && t2 == 3) {	//球撞到砖
			n1->removeFromParentAndCleanup(true);
		}
		else if (t1 == 3 && t2 == 4) {	
			n2->removeFromParentAndCleanup(true);
		}
		else if (t1 == 2 || t2 == 2) {	//板撞到边界
			player->getPhysicsBody()->setVelocity(Vec2::ZERO);
		}
	}

	return true;
}


void HitBrick::GameOver() {

	ball->getPhysicsBody()->setVelocity(Vec2::ZERO);

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);

}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
