///////////////////////////////////////////////////////////////////////////////
///  @file OgreCapsule.h
///  @brief This class represents a Capsule, which is defined by 2 endpoints and a radius.
///			You can interpret it as a sphere that is sweept along a line.
///
///  @author The OgreOpcode Team @date 23-02-2005
///  
///////////////////////////////////////////////////////////////////////////////
///  
///  This file is part of OgreOpcode.
///  
///  A lot of the code is based on the Nebula Opcode Collision module, see docs/Nebula_license.txt
///  
///  OgreOpcode is free software; you can redistribute it and/or
///  modify it under the terms of the GNU Lesser General Public
///  License as published by the Free Software Foundation; either
///  version 2.1 of the License, or (at your option) any later version.
///  
///  OgreOpcode is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
///  Lesser General Public License for more details.
///  
///  You should have received a copy of the GNU Lesser General Public
///  License along with OgreOpcode; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///  
///////////////////////////////////////////////////////////////////////////////
#ifndef __OgreOpcodeCapsule_h__
#define __OgreOpcodeCapsule_h__

#include "OgreOpcodeMath.h"

namespace OgreOpcode
{
	namespace Details
    {
		/// Represents a Capsule defined by 2 endpoints and a radius
		/// TODO: add methods to this capsule.
		class _OgreOpcode_Export Capsule
		{
		public:

			/// Gets the length of this line segment
			Real length() const {  return (start - end).length(); }
			/// Gets the squared length of this line segment
			Real length2() const {  return (start - end).squaredLength(); }

			/// Gets the surface area of this capsule
			Real area() const
			{					
				return Math::TWO_PI*radius*(2.0*radius + length() );
			}

			/// Gets the volume are this capsule
			Real volume() const
			{
				return Math::PI*radius*radius*( 1.333333333333333*length() );
			}


			/** The start point of this capsule.
			 */
			Vector3 start;
			/** The end point of this capsule.
			 */
			Vector3 end;
			/** The radius of this capsule.
			 */
			Real radius;			
		};
	}
}

#endif
