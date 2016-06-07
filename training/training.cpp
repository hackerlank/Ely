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
 * \file /Ely/training/training.cpp
 *
 * \date 2012-06-30
 * \author consultit
 */

//int generic_test(int argc, char **argv);
//int ai_main(int argc, char **argv);
//int anim_main(int argc, char **argv);
//int draw_geometry_main(int argc, char **argv);
//int manual_example_main(int argc, char **argv);
//int memory_pool_main(int argc, char **argv);
//int meshdrawer_main(int argc, char **argv);
//int network_main(int argc, char **argv);
int octree_main(int argc, char **argv);
//int particles_main(int argc, char **argv);
//int physics_main(int argc, char **argv);
//int render_to_texture_main(int argc, char **argv);
//int rocket_main(int argc, char **argv);
//int terrain_sample(int argc, char **argv);
//int threads_main(int argc, char **argv);

int main(int argc, char **argv)
{
	int result = 0;
//	result = generic_test(argc, argv);
//	result = ai_main(argc, argv);
//	result = anim_main(argc, argv);
//	result = network_main(argc, argv);
//	result = physics_main(argc, argv);
//	result = threads_main(argc, argv);
//	result = manual_example_main(argc, argv);
//	result = draw_geometry_main(argc, argv);
//	result = terrain_sample(argc, argv);
//	result = meshdrawer_main(argc, argv);
//	result = rocket_main(argc, argv);
//	result = render_to_texture_main(argc, argv);
//	result = memory_pool_main(argc, argv);
	result = octree_main(argc, argv);
//	result = particles_main(argc, argv);
	return result;
}
