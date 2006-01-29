/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    ogreOpcodeExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __ogreOpcodeExampleApp_h_
#define __ogreOpcodeExampleApp_h_

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;

#include "OgreOpcodeExample.h"

inline Quaternion makeRandomRotation()
{
	//  From
	//  J. Arvo.
	//  Fast random rotation matices.
	//  In D. Kirk, editor, Graphics Gems III, pages 117-120. Academic Press, 1992.
	Vector3 r(Math::UnitRandom(),Math::UnitRandom(),Math::UnitRandom());
	Real s1r = sqrt(Real(1.0)-r.x);
	Real sr  = sqrt(r.x);
	return Quaternion(
		s1r * sin(Math::TWO_PI*r.y),
		s1r * cos(Math::TWO_PI*r.y),
		sr  * sin(Math::TWO_PI*r.z),
		sr  * cos(Math::TWO_PI*r.z)
		);
}

using namespace Ogre;

class OgreOpcodeExampleApp : public OgreOpcodeExample
{
public:
	OgreOpcodeExampleApp(void);
	virtual ~OgreOpcodeExampleApp(void);

protected:
	virtual void createScene(void);
private:
	void parseDotScene( const String &SceneName);
};

#endif // #ifndef __ogreOpcodeExampleApp_h_
