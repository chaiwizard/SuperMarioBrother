#include "Player.h"

Player::Player()
	:desiredPosition(0,0),velocity(0,0),onGround(false),forwardMarch(false),mightAsWellJump(false)
{
	
}

Player::~Player()
{
}

Player* Player::create(std::string name)
{
	Player* ret = new Player();
	if(ret && ret->initWithFile(name)){
		ret->autorelease();
		return ret;
	}else{
		delete ret;
		ret = NULL;
	}

	return NULL;
}

#define CLAMP(x,min,max) MAX(min,MIN(x,max))

void Player::update(float dt)
{
	Point gravity(0,-450);
	Point gravityStep = gravity * dt;


	Point forwardMove(800,0);
	Point forwardMoveStep = forwardMove * dt;

	if(!onGround){
		velocity = velocity + gravityStep;
	}

	velocity = Point(velocity.x * 0.9, velocity.y);

	Point jumpForce(0.0, 400.0);
	float jumpCutoff = 200.0;
 
	if (mightAsWellJump && onGround) {
		velocity = velocity + jumpForce;
	} else if (!mightAsWellJump &&  (velocity.y > jumpCutoff) ){
		velocity = Point(velocity.x, jumpCutoff);
	}
	

	if(forwardMarch){
		velocity += forwardMoveStep;
	}


	Point minMovement = Point(0,-450);
	Point maxMovement = Point(120,300);

	velocity = Point(CLAMP(velocity.x,minMovement.x,maxMovement.x),
		CLAMP(velocity.y,minMovement.y,maxMovement.y));

	Point velocityStep = velocity * dt;

	desiredPosition = getPosition() + velocityStep;
}

Rect Player::collisionBoundingBox()
{
	Rect rect = getBoundingBox();

	Rect bounding(rect.origin.x+2,rect.origin.y,
		rect.size.width-4,rect.size.height);

	//Point diff = desiredPosition-getPosition();
	Point diff = ccpSub(desiredPosition,getPosition());
	return Rect(bounding.origin.x+diff.x,bounding.origin.y+diff.y,
		bounding.size.width,bounding.size.height);
}
