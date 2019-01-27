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

  // Debug ģʽ
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


  auto edgeSp = Sprite::create();  //����һ������
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox�ǲ��ܸ�����ײӰ���һ�ָ��壬����������������������ı߽�
  boundBody->setCategoryBitmask(0x00000001);
  boundBody->setCollisionBitmask(0xFFFFFFFF);
  boundBody->setContactTestBitmask(0x00000004);
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //λ����������Ļ����
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // Ԥ������Ч

  addSprite();    // ��ӱ����͸��־���
  addListener();  // ��Ӽ����� 
  addPlayer();    // ��������
  BrickGenerated();  // ����ש��


  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;
  spHolded = false;
  spFactor = 0;
  return true;
}

// �ؽ����ӣ��̶��������
// Todo
void HitBrick::setJoint() {
	joint = PhysicsJointPin::construct(player->getPhysicsBody(), ball->getPhysicsBody(),
		Vec2(0, ball->getBoundingBox().getMidY() - player->getBoundingBox().getMidY()),
		Vec2::ZERO);

	m_world->addJoint(joint);
}


// Ԥ������Ч
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// ��ӱ����͸��־���
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

// ��Ӽ�����
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// ������ɫ
void HitBrick::addPlayer() {

  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // ���ð�ĸ�������
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
  // ������ĸ�������
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

// ʵ�ּ򵥵�����Ч��
// Todo
void HitBrick::update(float dt) {
	if (spHolded && spFactor <= 600) {
		spFactor += 50;
	}
}



// ����ש��
// Todo
void HitBrick::BrickGenerated() {

for (int i = 0; i < 3; i++) {
	int cw = 0;
	while (cw <= visibleSize.width) {
		auto box = Sprite::create("box.png");
		// Ϊש�����ø�������
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


// ����
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  if(player->getBoundingBox().getMinX() > 3) 
		  player->getPhysicsBody()->setVelocity(Vec2(-400.0f, 0));
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // �����ƶ�
    // Todo
	  if (player->getBoundingBox().getMaxX() < visibleSize.width - 3) 
		  player->getPhysicsBody()->setVelocity(Vec2(400.0f, 0));
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // ��ʼ����
	  if (onBall) {
		  spHolded = true;
		  auto particle = ParticleSystemQuad::create("fire.plist");	//�������Ч��
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

// �ͷŰ���
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  auto physicsBody = ball->getPhysicsBody();
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // ֹͣ�˶�
    // Todo
	  player->getPhysicsBody()->setVelocity(Vec2::ZERO);
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // ����������С����
	  if (onBall) {
		  //С����
		  m_world->removeAllJoints();
		  ball->removeAllChildren();	//��ȥ����Ч��
		  physicsBody->setVelocity(Vec2(0, spFactor));
		  onBall = false;
		  spHolded = false;
	  }
    break;

  default:
    break;
  }
}

// ��ײ���
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
	auto n1 = contact.getShapeA()->getBody()->getNode();
	auto n2= contact.getShapeB()->getBody()->getNode();

	if (n1 && n2) {
		
		int t1 = n1->getTag();
		int t2 = n2->getTag();

		if (t1 == 1 || t2 == 1) {	//ײ����
			auto particle = ParticleSystemQuad::create("spark.plist");	//�������Ч��
			particle->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
			particle->setPosition(350, 200);
			particle->setScale(5.0f);
			ball->addChild(particle, -1);
			GameOver();
		}
		if (t1 == 4 && t2 == 3) {	//��ײ��ש
			n1->removeFromParentAndCleanup(true);
		}
		else if (t1 == 3 && t2 == 4) {	
			n2->removeFromParentAndCleanup(true);
		}
		else if (t1 == 2 || t2 == 2) {	//��ײ���߽�
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

  auto label2 = Label::createWithTTF("����", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("�˳�", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);

}

// ���������水ť��Ӧ����
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// �˳�
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
