#ifndef __THUNDER_H__
#define __THUNDER_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;
class Thunder : public cocos2d::Layer {
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(Thunder);
private:
	void preloadMusic();
	void playBgm();

	void addTouchListener();
	void addKeyboardListener();

	void addEnemy(int n);
	void newEnemy();
	void update(float f);
	void fire();
	void explosion();
	void meet(EventCustom* event);
	void addCustomListener();
	void movePlane(char c);
	void Boom(Sprite* enemy);
	void stopAc();
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchMoved(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	void meet1(EventCustom* event);
	void meetBorder(EventCustom* event);


	bool isMove;
	bool isClick;
	char movekey;
	Size visibleSize;
	Sprite* player;
	list<Sprite*> bullets;
	list<Sprite*> enemies;
	cocos2d::Vector<SpriteFrame*> explode;
	int stoneType;
	// show bullets and enemies number
	Label* bulletsNum;
	Label* enemiesNum;

};

#endif // __THUNDER_H__
