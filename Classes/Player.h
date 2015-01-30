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
	Rect collisionBoundingBox();

	Point getDesiredPosition() const 
	{ 
		return desiredPosition; 
	}

	void setDesiredPosition(Point inPos){
		desiredPosition = inPos;
	}

	void setVelocity(Point vel) { 
		velocity = vel; 
	}

	Point getVelocity() const {
		return velocity;
	}

	void setOnGround(bool status) { 
		onGround = status; 
	}


	void setForwardMarch(bool status){
		forwardMarch = status;
	}

	void setJump(bool status){
		mightAsWellJump = status;
	}

private:
	Point desiredPosition;
	Point velocity;
	bool onGround;

	bool forwardMarch;
	bool mightAsWellJump;
};

#endif