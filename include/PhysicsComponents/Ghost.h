/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/include/PhysicsComponents/Ghost.h
 *
 * \date 13/ago/2014 (11:36:18)
 * \author consultit
 */

#ifndef GHOST_H_
#define GHOST_H_

#include "ObjectModel/Component.h"

namespace ely
{
class GhostTemplate;

/**
 * \brief Component representing a ghost object attached to an object.
 *
 * It constructs a rigid body with the single specified collision shape_type
 * along with relevant parameters.\n
 * Collision shapes are:
 * - "sphere"
 * - "plane"
 * - "box"
 * - "cylinder"
 * - "capsule"
 * - "cone"
 * - "heightfield"
 * - "triangle mesh"
 * In case of "sphere", "box", "cylinder", "capsule", "cone", if any of
 * the relevant parameters is missing, the shape is automatically
 * constructed by guessing them through calculation of a tight bounding volume
 * of object geometry (supposedly specified by the model component).\n
 * For "plane" shape, in case of missing parameters, the default is
 * a plane with normal = (0,0,1) and d = 0.
 *
 * XML Param(s):
 * - "body_type"  				|single|"dynamic" (values: static|kinematic)
 * - "body_mass"  				|single|"1.0"
 * - "body_friction"  			|single|"0.8"
 * - "body_restitution"  		|single|"0.1"
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere" (values: sphere|plane|box|cylinder|capsule|cone|heightfield|triangle mesh)
 * - "shape_size"  				|single|"medium"  (values: minimum|medium|maximum)
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (for sphere,cylinder,capsule,cone)
 * - "shape_norm_x"  			|single|no default (for plane)
 * - "shape_norm_y"  			|single|no default (for plane)
 * - "shape_norm_z"  			|single|no default (for plane)
 * - "shape_d"  				|single|no default (for plane)
 * - "shape_half_x"  			|single|no default (for box)
 * - "shape_half_y"  			|single|no default (for box)
 * - "shape_half_z"  			|single|no default (for box)
 * - "shape_height"  			|single|"1.0" (for cylinder,capsule,cone,heightfield)
 * - "shape_up"  				|single|"z" (values: x|y|z for cylinder,capsule,cone,heightfield)
 * - "shape_heightfield_file" 	|single|no default (for heightfield)
 * - "shape_scale_w"  			|single|"1.0" (for heightfield)
 * - "shape_scale_d"  			|single|"1.0" (for heightfield)
 * - "ccd_motion_threshold"  	|single|no default
 * - "ccd_swept_sphere_radius" 	|single|no default
 *
 * \note parts inside [] are optional.\n
 */
class Ghost: public Component
{
protected:
	friend class GhostTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Ghost();
	virtual ~Ghost();
};

} /* namespace ely */

#endif /* GHOST_H_ */
