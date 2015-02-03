#include "Player.h"

Player::Player()
	:velocity(0,0),m_onGround(false),desiredPosition(0,0),
	forwardMarch(false),mightAswellJump(false)
{
}

Player::~Player()
{
}

Player* Player::create(std::string filename)
{
	Player* ret = new Player();
	if(ret && ret->initWithFile(filename)){
		ret->autorelease();
		return ret;
	}else{
		delete ret;
		ret = NULL;
	}

	return ret;
}

#define CLAMP(x,min,max) MAX(min,MIN(x,max))

void Player::update(float dt)
{
	Point gravity(0,-450);//g=9.8
	Point gravityStep = gravity * dt;

	if(!m_onGround){
		velocity = velocity + gravityStep;
	}

	Point forwardMove(800,0);
	Point forwardStep = forwardMove * dt;

	velocity = Point(velocity.x * 0.9, velocity.y);

	if(forwardMarch){
		velocity += forwardStep;
	}

	Point jumpForce(0,310);
	int jumpCutoff = 150;

	if(mightAswellJump && m_onGround){
		velocity = velocity + jumpForce;
	}else if( !mightAswellJump && (velocity.x > jumpCutoff)){
		velocity = Point(velocity.x, jumpCutoff);
	}


	velocity = Point(CLAMP(velocity.x,0,120),
		CLAMP(velocity.y,-450,300));

	Point step = velocity * dt;

	desiredPosition = getPosition() + step;
}

Rect Player::collisionBoundingBox()
{
	Rect rect = getBoundingBox();

	Rect smallBox(rect.origin.x+2,rect.origin.y,
		rect.size.width - 4,rect.size.height);

	Point diff = desiredPosition - getPosition();

	return Rect(smallBox.origin.x + diff.x, smallBox.origin.y + diff.y,
		smallBox.size.width,smallBox.size.height);
}