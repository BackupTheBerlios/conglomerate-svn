///////////////////////////////////////////////////////////////////////////////
///  @file OgreTriangle.h
///  @brief This class represents a Triangle, which is composed by
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
#ifndef __OgreOpcodeTriangle_h__
#define __OgreOpcodeTriangle_h__

namespace OgreOpcode
{
	namespace Details
	{
		/// Represents a Triangle defined by three points (vertices).
		/// This class is strongly based on OPCODE's triangle class, but is designed to work with
                /// Ogre's vectors and (will) haves more utility methods.
		class _OgreOpcode_Export Triangle
		{
		public:

                       /** Fast, but unsafe default constructor
                        */
                       Triangle()
                       {
                       }
                       
                       /** Constructor
                        */
                       Triangle( const Vector3& p0, const Vector3& p1, const Vector3& p2 )
                       {
                                 vertices[0] = p0;
                                 vertices[1] = p1;
                                 vertices[2] = p2;
                       }

                       /** Copy-constructor
                        */
                       Triangle( const Triangle& tri )
                       {
                                 vertices[0] = tri.vertices[0];
                                 vertices[1] = tri.vertices[1];
                                 vertices[2] = tri.vertices[2];
                       }
                       
                       /// Dummy destructor :P
                       ~Triangle() {}
                       
                       
                       // TODO: Add more methods for this class.


		
		       Vector3 vertices[3];
		};
	}
}
