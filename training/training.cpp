
//int generic_test(int argc, char **argv);
//int ai_main(int argc, char **argv);
//int anim_main(int argc, char **argv);
//int network_main(int argc, char **argv);
//int physics_main(int argc, char **argv);
//int threads_main(int argc, char **argv);
//int manual_example_main(int argc, char **argv);
//int draw_geometry_main(int argc, char **argv);
//int terrain_sample(int argc, char **argv);
//int meshdrawer_main(int argc, char **argv);
//int rocket_main(int argc, char **argv);
//int render_to_texture_main(int argc, char **argv);
//int memory_pool_main(int argc, char **argv);
int octree_main(int argc, char **argv);

///XXX routine pointer
int (*trainer)(int argc, char **argv) = octree_main;

int main(int argc, char **argv)
{
	int result = (*trainer)(argc, argv);
	return result;
}
