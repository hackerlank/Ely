//int generic_test(int argc, char **argv);
//int ai_main(int argc, char **argv);
//int anim_main(int argc, char **argv);
//int network_main(int argc, char **argv);
//int physics_main(int argc, char **argv);
//int threads_main(int argc, char **argv);
//int manual_example_main(int argc, char **argv);
//int draw_geometry_main(int argc, char **argv);
int terrain_sample(int argc, char **argv);
//int meshdrawer_main(int argc, char **argv);
//int rocket_main(int argc, char **argv);
//int render_to_texture_main(int argc, char **argv);
//int memory_pool_main(int argc, char **argv);
//int octree_main(int argc, char **argv);
//int raknet_main(int argc, char *argv[]);

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
	result = terrain_sample(argc, argv);
//	result = meshdrawer_main(argc, argv);
//	result = rocket_main(argc, argv);
//	result = render_to_texture_main(argc, argv);
//	result = memory_pool_main(argc, argv);
//	result = octree_main(argc, argv);
//	result = raknet_main(argc, argv);
	return result;
}
