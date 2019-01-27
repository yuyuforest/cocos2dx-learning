#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }


	//�����Զ����ͼƬ�˵�
	MenuItemImage *settingMenuItem = MenuItemImage::create(
		"button.png",
		"button.png",
		CC_CALLBACK_1(HelloWorld::menuItemSettingCallback, this));
	settingMenuItem->setPosition(Vec2(visibleSize.width * 0.7f + origin.x, visibleSize.height * 0.2f + origin.y));

    auto menu = Menu::create(closeItem, settingMenuItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	//�����ʵ���ʵ������xml�ļ����ص��ʵ���
	auto *chnStrings = Dictionary::createWithContentsOfFile("cnLabel.xml");
	//ͨ��xml�ļ��е�key��ȡvalue
	//�������
	const char *name = (chnStrings->valueForKey("name"))->getCString();
	auto label = Label::createWithSystemFont(name, "Microsoft Yahei", 30);

	label->enableOutline(Color4B::RED, 2);

    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2 - 60,
			origin.y + visibleSize.height - label->getContentSize().height));

        this->addChild(label, 1);
    }

	//���ѧ��
	const char* sid = (chnStrings->valueForKey("sid"))->getCString();
	auto sidLabel = Label::createWithTTF(sid, "fonts/Marker Felt.ttf", 30);
	sidLabel->setTextColor(Color4B::YELLOW);
	sidLabel->enableOutline(Color4B::RED, 2);

	if (sidLabel == nullptr)
	{
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else
	{
		sidLabel->setPosition(Vec2(origin.x + visibleSize.width / 2 + 60,
			origin.y + visibleSize.height - label->getContentSize().height));

		this->addChild(sidLabel);
	}

	//����ͼƬ
    auto sprite = Sprite::create("haikyuu.jpg");
    if (sprite == nullptr)
    {
        problemLoading("'haikyuu.jpg'");
    }
    else
    {
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        this->addChild(sprite, 0);
    }

    return true;
}

//�Զ����ͼƬ�˵��Ļص�����������ò˵�������һ������
void HelloWorld::menuItemSettingCallback(Ref* pSender)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto sprite = Sprite::create("HelloWorld.png");
	if (sprite == nullptr)
	{
		problemLoading("'HelloWorld.png'");
	}
	else
	{
		sprite->setPosition(Vec2(visibleSize.width * 0.8f + origin.x, visibleSize.height * 0.8f + origin.y));

		this->addChild(sprite);
	}
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
