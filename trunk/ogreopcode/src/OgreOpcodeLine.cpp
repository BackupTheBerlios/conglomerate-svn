///////////////////////////////////////////////////////////////////////////////
///  @file OgreOpcodeLine.cpp
///  @brief <TODO: insert file description here>
///
///  @author The OgreOpcode Team @date 28-05-2005
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

#include "OgreOpcodeLine.h"
#include "OgreOpcodeRay.h"
#include "OgreCapsule.h"
#include "OgreOrientedBox.h"

namespace OgreOpcode
{
	namespace Details
    {
		//------------------------------------------------------------------------
		// Line class
		//------------------------------------------------------------------------
		Real Line::squaredDistance( const Vector3& point, Real *t ) const
		{
			Vector3 dir = end - start;
			Vector3 kDiff = point - start;
			Real fT = kDiff | dir;

			if ( fT <= 0.0 )
			{
				fT = 0.0;
			}
			else
			{
				Real fSqrLen= dir.squaredLength();
				if ( fT >= fSqrLen )
				{
					fT = 1.0;
					kDiff -= dir;
				}
				else
				{
					fT /= fSqrLen;
					kDiff -= fT*dir;
				}
			}

			if ( t ) *t = fT;

			return kDiff.squaredLength();
		}
		// --------------------------------------------------------------------
		Real Line::distance( const Vector3& point, Real *t ) const
		{
			return Math::Sqrt( squaredDistance(point,t) );
		}
		//------------------------------------------------------------------------
		// Follows from Magic-Software  at http://www.magic-software.com
		//------------------------------------------------------------------------
		Real Line::squaredDistance( const Line& line1, Real* p0, Real* p1 ) const
		{
			const Line& line0 = *this;
			Vector3 dir0 = line0.getDirection();
			Vector3 dir1 = line1.getDirection();

			Vector3 kDiff = line0.start - line1.start;
			Real fA00 = dir0.squaredLength();
			Real fA01 = -(dir0|dir1);
			Real fA11 = dir1.squaredLength();
			Real fB0 = kDiff | dir0;
			Real fC = kDiff.squaredLength();
			Real fDet = Math::Abs(fA00*fA11-fA01*fA01);
			Real fB1, fS, fT, fSqrDist, fTmp;

#if (OGRE_DOUBLE_PRECISION == 1)
			if ( fDet >= DBL_EPSILON )
#else
			if ( fDet >= FLT_EPSILON )
#endif
			{
				// line segments are not parallel
				fB1 = -(kDiff|dir1);
				fS = fA01*fB1-fA11*fB0;
				fT = fA01*fB0-fA00*fB1;
			    
				if ( fS >= 0.0 )
				{
					if ( fS <= fDet )
					{
						if ( fT >= 0.0 )
						{
							if ( fT <= fDet )  // region 0 (interior)
							{
								// minimum at two interior points of 3D lines
								Real fInvDet = (1.0)/fDet;
								fS *= fInvDet;
								fT *= fInvDet;
								fSqrDist = fS*(fA00*fS+fA01*fT+(2.0)*fB0) +
									fT*(fA01*fS+fA11*fT+(2.0)*fB1)+fC;
							}
							else  // region 3 (side)
							{
								fT = 1.0;
								fTmp = fA01+fB0;
								if ( fTmp >= 0.0 )
								{
									fS = 0.0;
									fSqrDist = fA11+(2.0)*fB1+fC;
								}
								else if ( -fTmp >= fA00 )
								{
									fS = 1.0;
									fSqrDist = fA00+fA11+fC+(2.0)*(fB1+fTmp);
								}
								else
								{
									fS = -fTmp/fA00;
									fSqrDist = fTmp*fS+fA11+(2.0)*fB1+fC;
								}
							}
						}
						else  // region 7 (side)
						{
							fT = 0.0;
							if ( fB0 >= 0.0 )
							{
								fS = 0.0;
								fSqrDist = fC;
							}
							else if ( -fB0 >= fA00 )
							{
								fS = 1.0;
								fSqrDist = fA00+(2.0)*fB0+fC;
							}
							else
							{
								fS = -fB0/fA00;
								fSqrDist = fB0*fS+fC;
							}
						}
					}
					else
					{
						if ( fT >= 0.0 )
						{
							if ( fT <= fDet )  // region 1 (side)
							{
								fS = 1.0;
								fTmp = fA01+fB1;
								if ( fTmp >= 0.0 )
								{
									fT = 0.0;
									fSqrDist = fA00+(2.0)*fB0+fC;
								}
								else if ( -fTmp >= fA11 )
								{
									fT = 1.0;
									fSqrDist = fA00+fA11+fC+(2.0)*(fB0+fTmp);
								}
								else
								{
									fT = -fTmp/fA11;
									fSqrDist = fTmp*fT+fA00+(2.0)*fB0+fC;
								}
							}
							else  // region 2 (corner)
							{
								fTmp = fA01+fB0;
								if ( -fTmp <= fA00 )
								{
									fT = 1.0;
									if ( fTmp >= 0.0 )
									{
										fS = 0.0;
										fSqrDist = fA11+(2.0)*fB1+fC;
									}
									else
									{
										fS = -fTmp/fA00;
										fSqrDist = fTmp*fS+fA11+(2.0)*fB1+fC;
									}
								}
								else
								{
									fS = 1.0;
									fTmp = fA01+fB1;
									if ( fTmp >= 0.0 )
									{
										fT = 0.0;
										fSqrDist = fA00+(2.0)*fB0+fC;
									}
									else if ( -fTmp >= fA11 )
									{
										fT = 1.0;
										fSqrDist = fA00+fA11+fC+
											(2.0)*(fB0+fTmp);
									}
									else
									{
										fT = -fTmp/fA11;
										fSqrDist = fTmp*fT+fA00+(2.0)*fB0+fC;
									}
								}
							}
						}
						else  // region 8 (corner)
						{
							if ( -fB0 < fA00 )
							{
								fT = 0.0;
								if ( fB0 >= 0.0 )
								{
									fS = 0.0;
									fSqrDist = fC;
								}
								else
								{
									fS = -fB0/fA00;
									fSqrDist = fB0*fS+fC;
								}
							}
							else
							{
								fS = 1.0;
								fTmp = fA01+fB1;
								if ( fTmp >= 0.0 )
								{
									fT = 0.0;
									fSqrDist = fA00+(2.0)*fB0+fC;
								}
								else if ( -fTmp >= fA11 )
								{
									fT = 1.0;
									fSqrDist = fA00+fA11+fC+(2.0)*(fB0+fTmp);
								}
								else
								{
									fT = -fTmp/fA11;
									fSqrDist = fTmp*fT+fA00+(2.0)*fB0+fC;
								}
							}
						}
					}
				}
				else 
				{
					if ( fT >= 0.0 )
					{
						if ( fT <= fDet )  // region 5 (side)
						{
							fS = 0.0;
							if ( fB1 >= 0.0 )
							{
								fT = 0.0;
								fSqrDist = fC;
							}
							else if ( -fB1 >= fA11 )
							{
								fT = 1.0;
								fSqrDist = fA11+(2.0)*fB1+fC;
							}
							else
							{
								fT = -fB1/fA11;
								fSqrDist = fB1*fT+fC;
							}
						}
						else  // region 4 (corner)
						{
							fTmp = fA01+fB0;
							if ( fTmp < 0.0 )
							{
								fT = 1.0;
								if ( -fTmp >= fA00 )
								{
									fS = 1.0;
									fSqrDist = fA00+fA11+fC+(2.0)*(fB1+fTmp);
								}
								else
								{
									fS = -fTmp/fA00;
									fSqrDist = fTmp*fS+fA11+(2.0)*fB1+fC;
								}
							}
							else
							{
								fS = 0.0;
								if ( fB1 >= 0.0 )
								{
									fT = 0.0;
									fSqrDist = fC;
								}
								else if ( -fB1 >= fA11 )
								{
									fT = 1.0;
									fSqrDist = fA11+(2.0)*fB1+fC;
								}
								else
								{
									fT = -fB1/fA11;
									fSqrDist = fB1*fT+fC;
								}
							}
						}
					}
					else   // region 6 (corner)
					{
						if ( fB0 < 0.0 )
						{
							fT = 0.0;
							if ( -fB0 >= fA00 )
							{
								fS = 1.0;
								fSqrDist = fA00+(2.0)*fB0+fC;
							}
							else
							{
								fS = -fB0/fA00;
								fSqrDist = fB0*fS+fC;
							}
						}
						else
						{
							fS = 0.0;
							if ( fB1 >= 0.0 )
							{
								fT = 0.0;
								fSqrDist = fC;
							}
							else if ( -fB1 >= fA11 )
							{
								fT = 1.0;
								fSqrDist = fA11+(2.0)*fB1+fC;
							}
							else
							{
								fT = -fB1/fA11;
								fSqrDist = fB1*fT+fC;
							}
						}
					}
				}
			}
			else
			{
				// line segments are parallel
				if ( fA01 > 0.0 )
				{
					// direction vectors form an obtuse angle
					if ( fB0 >= 0.0 )
					{
						fS = 0.0;
						fT = 0.0;
						fSqrDist = fC;
					}
					else if ( -fB0 <= fA00 )
					{
						fS = -fB0/fA00;
						fT = 0.0;
						fSqrDist = fB0*fS+fC;
					}
					else
					{
						fB1 = -(kDiff|dir1);
						fS = 1.0;
						fTmp = fA00+fB0;
						if ( -fTmp >= fA01 )
						{
							fT = 1.0;
							fSqrDist = fA00+fA11+fC+(2.0)*(fA01+fB0+fB1);
						}
						else
						{
							fT = -fTmp/fA01;
							fSqrDist = fA00+(2.0)*fB0+fC+fT*(fA11*fT+
								(2.0)*(fA01+fB1));
						}
					}
				}
				else
				{
					// direction vectors form an acute angle
					if ( -fB0 >= fA00 )
					{
						fS = 1.0;
						fT = 0.0;
						fSqrDist = fA00+(2.0)*fB0+fC;
					}
					else if ( fB0 <= 0.0 )
					{
						fS = -fB0/fA00;
						fT = 0.0;
						fSqrDist = fB0*fS+fC;
					}
					else
					{
						fB1 = -(kDiff | dir1);
						fS = 0.0;
						if ( fB0 >= -fA01 )
						{
							fT = 1.0;
							fSqrDist = fA11+(2.0)*fB1+fC;
						}
						else
						{
							fT = -fB0/fA01;
							fSqrDist = fC+fT*((2.0)*fB1+fA11*fT);
						}
					}
				}
			}

			// put 
			if ( p0 ) *p0 = fS;
			if ( p1 ) *p1 = fT;

			return Math::Abs(fSqrDist);
		}
		// --------------------------------------------------------------------
		Real Line::distance( const Line& line1, Real* p0, Real* p1 ) const
		{
			return Math::Sqrt( squaredDistance(line1, p0, p1) );
		}
		// --------------------------------------------------------------------
	}
}
