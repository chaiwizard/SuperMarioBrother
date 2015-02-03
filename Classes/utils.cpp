#include "utils.h"

Rect intersectionRect(Rect recta, Rect rectb)
{
	Point pos(MAX(recta.origin.x,rectb.origin.x),
		MAX(recta.origin.y,rectb.origin.y));

	Point endPos(MIN(recta.origin.x + recta.size.width,
		rectb.origin.x+rectb.size.width),
		MIN(recta.origin.y+recta.size.height,
		rectb.origin.y+rectb.size.height) );

	return Rect(pos.x,pos.y,endPos.x-pos.x,endPos.y-pos.y);
}