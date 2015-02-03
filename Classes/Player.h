#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "cocos2d.h"

using namespace cocos2d;

class Player : public Sprite{
public:
	Player();
	~Player();

	static Player* create(std::string filename);

	void update(float dt);

	void setVelocity(Point value) {
		velocity = value;
	}

	Point getVelocity() const {
		return velocity;
	}

	void setOnGround(bool value){
		m_onGround = value;
	}

	bool getOnGround() const {
		return m_onGround;
	}

	void setDesiredPostion(Point pos)
	{
		desiredPosition = pos;	
	}

	Point getDesiredPosition() const {
		return desiredPosition;
	}

	Rect collisionBoundingBox();

	bool getForwardMarch() const { 
		return forwardMarch;
	}

	void setForwardMarch(bool status){
		forwardMarch = status;
	}

	void setJump(bool status){
		mightAswellJump = status;
	}

private:
	Point velocity;
	bool m_onGround;
	Point desiredPosition;

	bool forwardMarch;
	bool mightAswellJump;
};

#endif