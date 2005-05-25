#pragma warning( disable : 4267 4311 4244 4312 4530)
#include <Walker.h>
#include <Ogre.h>
#include <deque>

using namespace std;
using namespace Ogre;

Walker::Walker(SceneNode *sn, Entity *ent, const deque<Vector3> &walk)
:mWalkList(walk),
mEntity(ent),
mNode(sn),
mDirection(Vector3::ZERO),
mWalking(false),
mWalkSpeed(35.0f)
{
	// Attempt to setup the object to walk around.  If no points are
	// specified, set an idle animation.
	if ( nextLocation() )
	{
		// Set walking animation
		mAnimationState = mEntity->getAnimationState( "Walk" );
		mAnimationState->setLoop( true );
		mAnimationState->setEnabled( true );
	} // if
	else
	{
		// Set idle animation
		mAnimationState = mEntity->getAnimationState( "Idle" );
		mAnimationState->setLoop( true );
		mAnimationState->setEnabled( true );
	} // else

} 

Walker::~Walker()
{
}

//Add waypoint to the current set of waypoints
void Walker::addWaypoint(Vector3 wp)
{
	mWalkList.push_back(wp);
	if (!mWalking)
	{
		mAnimationState = mEntity->getAnimationState( "Walk" );
		mAnimationState->setLoop( true );
		mAnimationState->setEnabled( true );
		nextLocation();
	}
}
void Walker::setSpeed(Real speed)
{
	mWalkSpeed = speed;
}

/* This function is called to start the object moving to the next position
in mWalkList.
*/
bool Walker::nextLocation( )
{
	if ( mWalkList.empty() )
	{
		mWalking = false;
	}
	else
	{
		mDestination = mWalkList.front( );  // this gets the front of the deque
		mWalkList.pop_front( );             // this removes the front of the deque

		mDirection = mDestination - mNode->getPosition( );
		mDistance = mDirection.normalise( );

		Vector3 src = mNode->getOrientation( ) * Vector3::UNIT_X;
		if ( (1.0f + src.dotProduct( mDirection )) < 0.0001f )
			mNode->yaw( Degree(180) );
		else
		{
			Ogre::Quaternion quat = src.getRotationTo( mDirection );
			mNode->rotate( quat );
		} // else


		mWalking = true;
	}
	return mWalking;
} 

void Walker::frameStarted(Real dt)
{
	if ( mWalking )
	{
		Real move = mWalkSpeed * dt;
		mDistance -= move;
		if (mDistance <= 0.0f)
		{
			mNode->setPosition( mDestination );
			mDirection = Vector3::ZERO;
			if (! nextLocation( ) )
			{
				mAnimationState = mEntity->getAnimationState( "Die" );
				mAnimationState->setLoop( false );
				mAnimationState->setEnabled( true );
			} // if
		} // if
		else
		{
			mNode->translate( mDirection * move );
		} // else
	} // if

	mAnimationState->addTime( dt);
}