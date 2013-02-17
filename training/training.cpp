int ai_main(int argc, char **argv);
int anim_main(int argc, char **argv);
int network_main(int argc, char **argv);
int physics_main(int argc, char **argv);
int threads_main(int argc, char **argv);

int main(int argc, char **argv)
{
	int result = 0;
	result = ai_main(argc, argv);
	result = anim_main(argc, argv);
	result = network_main(argc, argv);
	result = physics_main(argc, argv);
	result = threads_main(argc, argv);
	return result;
}
