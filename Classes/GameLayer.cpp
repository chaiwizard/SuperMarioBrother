#include "GameLayer.h"


static Rect interscetionRect(Rect recta, Rect rectb)
{
	Point pos(MAX(recta.origin.x,rectb.origin.x),
		MAX(recta.origin.y,rectb.origin.y));
	Point endPos(MIN(recta.origin.x+recta.size.width,
		rectb.origin.x+rectb.size.height),
		MIN(recta.origin.y+recta.size.height,
		rectb.origin.y+rectb.size.height));

	return Rect(pos.x,pos.y,endPos.x-pos.x,endPos.y-pos.y);
}


Scene* GameLayer::createScene()
{
	auto scene = Scene::create();

	auto layer = GameLayer::create();

	scene->addChild(layer);

	return scene;
}


bool GameLayer::init()
{
	if(!LayerColor::initWithColor(Color4B(102,102,242,255))){
		return false;
	}
	m_gameOver = false;
	Size winSize = Director::getInstance()->getVisibleSize();

	m_map = CCTMXTiledMap::create("level1.tmx");
	addChild(m_map,0);

	m_background = m_map->getLayer("background");
	m_walls = m_map->getLayer("walls");
	m_hazards = m_map->getLayer("hazards");


	Size tileSize = m_map->getTileSize();
	Size mapSize = m_map->getMapSize();

	m_player = Player::create("koalio_stand.png");
	
	m_player->setPosition(100,100);
	m_map->addChild(m_player,15);

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

	float timeInterval = dt;
	if(timeInterval > 0.02){
		timeInterval = 0.02;
	}

	m_player->update(timeInterval);
	checkForAndResolveCollisionsForPlayer();
	handleHazardCollisions();
	checkForWinning();

	setViewPointCenter(m_player->getPosition());
}

//根据位置获取Player所在的tile map的坐标
Point GameLayer::tileCoordForPosition(Point position)
{
	Size mapSize = m_map->getMapSize();
	Size tileSize = m_map->getTileSize();

	int x = (position.x) / tileSize.width;
	int y = (mapSize.height*tileSize.height - position.y) / tileSize.height;

	return Point(x,y);
}

void GameLayer::handleHazardCollisions()
{
	if(m_gameOver) return;

	int indices[8] = {7, 1, 3, 5, 0, 2, 6, 8};
	int i = 0;
	for(i=0; i<8; i++){
		int tileIndex = indices[i];
    
		Rect playerRect = m_player->collisionBoundingBox();
		Point desiredPos = m_player->getDesiredPosition();
		Point playerCoord = tileCoordForPosition(desiredPos);
    	
		int tileColumn = tileIndex % 3;
		int tileRow = tileIndex / 3;
		Point tileCoord(playerCoord.x + (tileColumn - 1),
			playerCoord.y + (tileRow - 1));

		int gid = m_hazards->getTileGIDAt(tileCoord);

		if(gid != 0){
			Rect tileRect = tileRectFromTileCoords(tileCoord);
			if(playerRect.intersectsRect(tileRect)){
				gameOver(false);
			}
		}
	}
}


void GameLayer::checkForAndResolveCollisionsForPlayer()
{
	int indices[8] = {7, 1, 3, 5, 0, 2, 6, 8};
	m_player->setOnGround(false);

	for(int i=0;i<8;++i){
		int tileIndex = indices[i];

		Rect playerRect = m_player->collisionBoundingBox();
		Point desiredPos = m_player->getDesiredPosition();
		Point playerCoord = tileCoordForPosition(desiredPos);

		//if (playerCoord.y >= self.map.mapSize.height - 1) {
		if(playerCoord.y >= 19){
			gameOver(false);
			return;
		}

#if 0
		log("player boundingbox:%f,%f,%f,%f",
			playerRect.origin.x,playerRect.origin.y,
			playerRect.size.width,playerRect.size.height);
		log("desiredPos=%f,%f",desiredPos.x,desiredPos.y);
		log("playerCoord.x=%f,playerCoord.y=%f,i=%d",playerCoord.x,playerCoord.y,i);
#endif

		int tileColumn = tileIndex % 3;
		int tileRow = tileIndex / 3;
		Point tileCoord(playerCoord.x + (tileColumn - 1),
			playerCoord.y + (tileRow - 1));

		int gid = m_walls->getTileGIDAt(tileCoord);


		//log("tileCoord.x=%f,%f",tileCoord.x,tileCoord.y);


		if(gid != 0){
			Rect tileRect = tileRectFromTileCoords(tileCoord);
#if 0
			log("playerRect:%f,%f,%f,%f",playerRect.origin.x,playerRect.origin.y,
				playerRect.size.width,playerRect.size.height);
			log("tileRect:%f,%f,%f,%f",tileRect.origin.x, tileRect.origin.y,
				tileRect.size.width,tileRect.size.height);
#endif

			if(playerRect.intersectsRect(tileRect)){				
				Rect intersection = interscetionRect(playerRect,tileRect);
				//below the player
				if(tileIndex == 7){
					  //tile is directly below Koala
					desiredPos.y += intersection.size.height;
					m_player->setVelocity(Point(m_player->getVelocity().x,0));
					m_player->setOnGround(true);
					m_player->setDesiredPosition(desiredPos);
				}else if(tileIndex == 1){
					 //tile is directly above Koala
					desiredPos.y -= intersection.size.height;
					m_player->setDesiredPosition(desiredPos);
				}else if(tileIndex == 3){
					//tile is left of Koala
					desiredPos.x += intersection.size.width;
					m_player->setDesiredPosition(desiredPos);
				}else if(tileIndex == 5) {
					//tile is right of Koala
					desiredPos.x -= intersection.size.width;
					m_player->setDesiredPosition(desiredPos);
				}else {
					if (intersection.size.width > intersection.size.height) {
						 //tile is diagonal, but resolving collision vertically
						m_player->setVelocity(Point(m_player->getVelocity().x, 0.0)); 
						
						float intersectionHeight = 0;
						if (tileIndex > 4) {
							intersectionHeight = intersection.size.height;
							m_player->setOnGround(true);
						} else {
							intersectionHeight = -intersection.size.height;
						}
						//player.desiredPosition = CGPointMake(player.desiredPosition.x, player.desiredPosition.y + intersection.size.height );
						desiredPos.y += intersection.size.height;
						m_player->setDesiredPosition(desiredPos);
					} else {
						float intersectionWidth;
						if (tileIndex == 6 || tileIndex == 0) {
							intersectionWidth = intersection.size.width;
						} else {
							intersectionWidth = -intersection.size.width;
						}
					
						//player.desiredPosition = CGPointMake(player.desiredPosition.x  + intersectionWidth, player.desiredPosition.y);
						desiredPos.x += intersectionWidth;
						m_player->setDesiredPosition(desiredPos);
					}
				}
			}
		}
	}
	m_player->setPosition(m_player->getDesiredPosition());
}

Rect GameLayer::tileRectFromTileCoords(Point tileCoords){
	Size mapSize = m_map->getMapSize();
	Size tileSize = m_map->getTileSize();
	float levelHeightInPixels = mapSize.height * tileSize.height;
  
  
	Point origin (tileCoords.x * tileSize.width, 
		levelHeightInPixels - ((tileCoords.y + 1) * tileSize.height));

	return Rect(origin.x, origin.y, tileSize.width, tileSize.height);
}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *event){

	Size winSize = Director::getInstance()->getVisibleSize();

	for(auto touch : touches){
		Point location = touch->getLocation();
		if(location.x > winSize.width/2){
			m_player->setJump(true);
		}else{
			m_player->setForwardMarch(true);
		}
	}
}

void GameLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *event){
	Size winSize = Director::getInstance()->getVisibleSize();
	int halfwidth = winSize.width /2;

	for(auto touch : touches){

		Point location = touch->getLocation();
		Point preLocation = touch->getPreviousLocation();

		if (location.x > halfwidth && preLocation.x <= halfwidth) {
			m_player->setJump(true);
			m_player->setForwardMarch(false);
		} else if (preLocation.x > halfwidth && location.x <= halfwidth) {
			m_player->setJump(false);
			m_player->setForwardMarch(true);
		}
	}

}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *event){
	
	Size winSize = Director::getInstance()->getVisibleSize();

	for(auto touch : touches){
		Point location = touch->getLocation();
		if(location.x > winSize.width/2){
			m_player->setJump(false);
		}else{
			m_player->setForwardMarch(false);
		}
	}
}


void GameLayer::setViewPointCenter(Point position)
{
	Size size = Director::getInstance()->getVisibleSize();

	int x = MAX(position.x,size.width/2);
	int y = MAX(position.y,size.height/2);

	x = MIN(x,m_map->getTileSize().width*m_map->getMapSize().width-size.width/2);
	y = MIN(y,m_map->getTileSize().height*m_map->getMapSize().height-size.height/2);
	Point actualPosition(x,y);
	Point centOfView(size.width/2,size.height/2);
	Point viewpoint = centOfView - actualPosition;

	m_map->setPosition(viewpoint);
}

void GameLayer::gameOver(bool win)
{
	unscheduleUpdate();

	Size size = Director::getInstance()->getVisibleSize();
	m_gameOver = true;

	std::string text;

	if(win){
		text = "You Win!";
	}else{
		text = "You Have Died!";
	}


	auto label = Label::create(text,"fonts/Marker Felt.ttf",32);
	label->setPosition(Point(size.width/2,size.height/1.5));
	addChild(label);


	auto replayItem = MenuItemImage::create("replay.png","replay.png",
		 CC_CALLBACK_1(GameLayer::replayCallback, this));
	replayItem->setAnchorPoint(Point::ZERO);

	Size imgSize = replayItem->getContentSize();
    
	replayItem->setPosition(Point( (size.width - 180) / 2.0,
		(size.height - 66) / 2.0));

    // create menu, it's an autorelease object
    auto menu = Menu::create(replayItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
}

void GameLayer::replayCallback(Ref* sender)
{
	auto scene = GameLayer::createScene();
	Director::getInstance()->replaceScene(scene);
}

void GameLayer::checkForWinning()
{
	if(m_player->getPosition().x > 3130.0){
		gameOver(true);
	}
}