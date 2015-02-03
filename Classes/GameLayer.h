#ifndef GAME_LAYER_H_
#define GAME_LAYER_H_

#include "cocos2d.h"
#include "Player.h"

using namespace cocos2d;

class GameLayer : LayerColor {
public:
	static Scene* createScene();

	CREATE_FUNC(GameLayer);
	bool init();

	void update(float dt);

	void checkForAndResolveCollisionsForPlayer();
	void handleHazardsCollisions();

	void onTouchesBegan(const std::vector<Touch*>& touches, Event *event);
    void onTouchesMoved(const std::vector<Touch*>& touches, Event *event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event *event);


private:
	CCTMXTiledMap *m_map;
	CCTMXLayer *m_walls;
	CCTMXLayer *m_hazards;

	void gameOver(bool win);
	Point tileCoordForPosition(Point position);
	Rect tileRectFromTileCoords(Point position);
	void setViewPointCenter(Point pos);
	void replayCallback(Ref* sender);
	bool checkForSink(Point coord);
	void checkForWin();


	Player *m_player;
	bool m_gameOver;
};


#endif