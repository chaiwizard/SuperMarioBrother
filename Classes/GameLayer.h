#ifndef GAME_LAYER_H_
#define GAME_LAYER_H_

#include "cocos2d.h"
#include "Player.h"

using namespace cocos2d;

class GameLayer : public LayerColor {
public:
	
	static Scene* createScene();

	CREATE_FUNC(GameLayer);
	
	bool init();

	void update(float dt);

	void checkForAndResolveCollisionsForPlayer();
	void handleHazardCollisions();

	void onTouchesBegan(const std::vector<Touch*>& touches, Event *event); 
    void onTouchesMoved(const std::vector<Touch*>& touches, Event *event); 
    void onTouchesEnded(const std::vector<Touch*>& touches, Event *event); 

	void gameOver(bool win);
	void checkForWinning();

	void replayCallback(Ref* pSender);

private:

	Rect tileRectFromTileCoords(Point tileCoords);
	Point tileCoordForPosition(Point position);
	void setViewPointCenter(Point position);

	CCTMXTiledMap *m_map;
	CCTMXLayer *m_background;
	CCTMXLayer *m_walls;
	CCTMXLayer *m_hazards;

	Player *m_player;
	bool m_gameOver;
};


#endif