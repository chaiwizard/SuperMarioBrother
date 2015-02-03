#include "GameLayer.h"
#include "utils.h"


Scene* GameLayer::createScene()
{
	auto scene = Scene::create();

	auto layer = GameLayer::create();

	scene->addChild(layer);

	return scene;
}

bool GameLayer::init()
{
	if(!LayerColor::initWithColor(Color4B(100,100,240,255))){
		return false;
	}

	m_gameOver = false;
	m_map = CCTMXTiledMap::create("level1.tmx");
	addChild(m_map);
	m_walls = m_map->getLayer("walls");
	m_hazards = m_map->getLayer("hazards");

	m_player = Player::create("koalio_stand.png");
	m_player->setPosition(Point(100,100));
	m_map->addChild(m_player,15);

#ifdef KOALO_DEBUG
	Rect box = m_player->getBoundingBox();

	Point coord = tileCoordForPosition(m_player->getPosition());
#endif

	scheduleUpdate();

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(GameLayer::onTouchesBegan,this);
	listener->onTouchesMoved = CC_CALLBACK_2(GameLayer::onTouchesMoved,this);
	listener->onTouchesEnded = CC_CALLBACK_2(GameLayer::onTouchesEnded,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);

	return true;
}

void GameLayer::update(float dt)
{
	if(m_gameOver) return;
	m_player->update(dt);
	checkForAndResolveCollisionsForPlayer();
	handleHazardsCollisions();
	checkForWin();
	setViewPointCenter(m_player->getPosition());
}

void GameLayer::setViewPointCenter(Point pos)
{
	Size size = Director::getInstance()->getVisibleSize();

	int x = MAX(pos.x, size.width/2);
	int y = MAX(pos.y,size.height/2);

	x = MIN(x,m_map->getMapSize().width * m_map->getMapSize().width - size.width/2);
	y = MIN(y,m_map->getMapSize().height * m_map->getMapSize().height - size.height/2);

	Point actualPos(x,y);
	Point centOfView(size.width/2,size.height/2);

	Point viewpoint = centOfView - actualPos;

	m_map->setPosition(viewpoint);
}

void GameLayer::handleHazardsCollisions()
{
	if(m_gameOver) return;
	int indices[8] = {7,1,3,5,0,2,6,8};
	int i = 0;
	for(i=0;i<8;i++){
		int tileIndex = indices[i];

		Rect playerRect = m_player->collisionBoundingBox();
		Point desiredPos = m_player->getDesiredPosition();
		Point playerCoord = tileCoordForPosition(desiredPos);

		int tileColumn = tileIndex % 3; //1
		int tileRow = tileIndex / 3; //2 
		Point tileCoord(playerCoord.x + tileColumn - 1,
			playerCoord.y + tileRow - 1);


		int gid = m_hazards->getTileGIDAt(tileCoord);
		if(gid != 0){
			//log("collision");
			Rect tileRect = tileRectFromTileCoords(tileCoord);
			if(playerRect.intersectsRect(tileRect)){
				//log("gameover");
				gameOver(false);
			}
		}
	}
}

bool GameLayer::checkForSink(Point coord)
{
	bool ret = false;
	Size mapSize = m_map->getMapSize();
	if(coord.y >= mapSize.height-1){
		ret = true;
	}

	return ret;
}


void GameLayer::checkForAndResolveCollisionsForPlayer()
{
	int indices[8] = {7,1,3,5,0,2,6,8};
	int i=0;
	m_player->setOnGround(false);
	for(i=0;i<8;i++){
		int tileIndex = indices[i];

		Rect playerRect = m_player->collisionBoundingBox();
		Point desiredPos = m_player->getDesiredPosition();
		Point playerCoord = tileCoordForPosition(desiredPos);

		if(checkForSink(playerCoord)){
			gameOver(false);
			return;
		}

		int tileColumn = tileIndex % 3; //1
		int tileRow = tileIndex / 3; //2 
		Point tileCoord(playerCoord.x + tileColumn - 1,
			playerCoord.y + tileRow - 1);


		int gid = m_walls->getTileGIDAt(tileCoord);
		if(gid != 0){
			//log("collision");
			Rect tileRect = tileRectFromTileCoords(tileCoord);
			if(playerRect.intersectsRect(tileRect)){
				Rect intersection = intersectionRect(playerRect,tileRect);

				if(tileIndex == 7){
					m_player->setVelocity(Point(m_player->getVelocity().x, 0));
					m_player->setOnGround(true);
					desiredPos.y += intersection.size.height;
					m_player->setDesiredPostion(desiredPos);
				}else if(tileIndex == 1){
					desiredPos.y -= intersection.size.height;
					m_player->setDesiredPostion(desiredPos);
				}else if(tileIndex == 3){
					desiredPos.x += intersection.size.width;
					m_player->setDesiredPostion(desiredPos);
				}else if(tileIndex == 5){
					desiredPos.x -= intersection.size.width;
					m_player->setDesiredPostion(desiredPos);
				}else{
					if(intersection.size.width > intersection.size.height){
						m_player->setVelocity(Point(m_player->getVelocity().x,0));

						float intersectionHeight = 0;
						if(tileIndex > 4){
							intersectionHeight = intersection.size.height;
							m_player->setOnGround(true);
						}else{
							intersectionHeight = -intersection.size.height;
						}

						desiredPos.y += intersectionHeight;
						m_player->setDesiredPostion(desiredPos);
					}else{
						float intersectionWidth = 0;
						if(tileIndex ==6 || tileIndex == 0){
							intersectionWidth = intersection.size.height;
						}else{
							intersectionWidth = -intersection.size.height;
						}

						desiredPos.x += intersectionWidth;
						m_player->setDesiredPostion(desiredPos);
					}
				}

			}
		}
		m_player->setPosition(desiredPos);
	}

}

Point GameLayer::tileCoordForPosition(Point position)
{
	Size mapSize = m_map->getMapSize();
	Size tileSize = m_map->getTileSize();

	int x = position.x / tileSize.width;

	int y = (mapSize.height * tileSize.height - position.y) / tileSize.height;

	return Point(x,y);
}

Rect GameLayer::tileRectFromTileCoords(Point position)
{
	Size mapSize = m_map->getMapSize();
	Size tileSize = m_map->getTileSize();

	float levelHeight = mapSize.height * tileSize.height;
	float x = position.x * tileSize.width;
	float y = levelHeight - ((position.y + 1) * tileSize.height);

	return Rect(x,y,tileSize.width,tileSize.height);

}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{
	Size winSize = Director::getInstance()->getVisibleSize();
	for(auto touch : touches){
		Point location = touch->getLocation();
		if( location.x < winSize.width / 2){
			m_player->setForwardMarch(true);
		}else{
			m_player->setJump(true);
		}
	}
}

void GameLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
	Size winSize = Director::getInstance()->getVisibleSize();
	int halfwidth = winSize.width/2;
	for(auto touch : touches){
		Point location = touch->getLocation();
		Point preLocation = touch->getPreviousLocation();

		if(location.x > halfwidth && preLocation.x <= halfwidth){
			m_player->setJump(true);
			m_player->setForwardMarch(false);
		}else if(preLocation.x > halfwidth && location.x <= halfwidth){
			m_player->setJump(false);
			m_player->setForwardMarch(true);
		}
	}

}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *event)
{
	Size winSize = Director::getInstance()->getVisibleSize();
	for(auto touch : touches){
		Point location = touch->getLocation();
		if( location.x < winSize.width / 2){
			m_player->setForwardMarch(false);
		}else{
			m_player->setJump(false);
		}
	}
}

void GameLayer::gameOver(bool win)
{
	unscheduleUpdate();
	m_gameOver = true;


	Size winSize = Director::getInstance()->getVisibleSize();

	std::string text;
	if(win){
		text = "You Win!";
	}else{
		text = "You Have Died!";
	}

	auto label = Label::create(text,"fonts/Marker Felt.ttf",32);
	label->setPosition(Point(winSize.width/2,winSize.height/1.5));
	addChild(label);

	auto replayItem = MenuItemImage::create("replay.png","replay.png",
		CC_CALLBACK_1(GameLayer::replayCallback,this));
	replayItem->setAnchorPoint(Point::ZERO);
	Size imgSize = replayItem->getContentSize();
	replayItem->setPosition(Point((winSize.width-imgSize.width)/2,
		(winSize.height - imgSize.height)/2));

	auto menu = Menu::create(replayItem,NULL);
	menu->setPosition(Point::ZERO);
	addChild(menu);
}

void GameLayer::replayCallback(Ref* sender)
{
	auto scene = GameLayer::createScene();
	Director::getInstance()->replaceScene(scene);
}

void GameLayer::checkForWin()
{
	if(m_player->getPosition().x > 3120){
		gameOver(true);
	}
}
	