#pragma warning( disable : 4267 )

#ifndef __Walker_H__
#define __Walker_H__

#include <Ogre.h>
#include <deque>
using namespace std;
using namespace Ogre;

class Walker
{
public:

	Walker(SceneNode *sn, Entity *ent, const deque<Vector3> &walk);
	virtual ~Walker();
	
	//Add waypoint to the current set of waypoints
	void addWaypoint(Vector3 wp);
	void setSpeed(Real speed);

	/* This function is called to start the object moving to the next position
	in mWalkList.
	*/
	bool nextLocation( );

	void frameStarted(Real dt);

protected:
	Real mDistance;                  // The distance the object has left to travel
	Vector3 mDirection;              // The direction the object is moving
	Vector3 mDestination;            // The destination the object is moving towards
	Real mWalkSpeed;    // The speed at which the object is moving
	bool mWalking;

	AnimationState *mAnimationState; // The current animation state of the object

	Entity *mEntity;                 // The Entity we are animating
	SceneNode *mNode;                // The SceneNode that the Entity is attached to
	std::deque<Vector3> mWalkList;   // The list of points we are walking to
};

#endif