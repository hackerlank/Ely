int generic_test(int argc, char **argv);
int ai_main(int argc, char **argv);
int anim_main(int argc, char **argv);
int network_main(int argc, char **argv);
int physics_main(int argc, char **argv);
int threads_main(int argc, char **argv);
int manual_example_main(int argc, char **argv);
int draw_geometry_main(int argc, char **argv);
int test_wo_local_scale(int argc, char **argv);
int meshdrawer_main(int argc, char **argv);
int rocket_main(int argc, char **argv);

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
//	result = test_wo_local_scale(argc, argv);
//	result = meshdrawer_main(argc, argv);
	rocket_main(argc, argv);
	return result;
}
