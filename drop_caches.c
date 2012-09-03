#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void help(int exit_status)
{
	printf("Usage: drop_caches [OPTIONS]... FILE...            \n"
	       "Drop the page caches of the given files.           \n"
	       "                                                   \n"
	       "  -h            show help                          \n");
	exit(exit_status);
}

void drop_caches(char **files)
{
	int fd;
	char *file;

	for (; *files; files++) {
		fprintf(stderr, "%s... ", *files);
		fd = open(*files, O_RDONLY);
		if (fd < 0) {
			perror("cannot open file");
			continue;
		}

		fdatasync(fd);
		if (posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED) < 0) {
			perror("fadvise failed");
			continue;
		}
		close(fd);
		fprintf(stderr, "flushed\n");
	}
}

int main(int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "hi:")) != -1) {
		switch (opt) {
		case 'h':
			help(EXIT_SUCCESS);
		default:
			help(EXIT_FAILURE);
		}
	}

	if (optind >= argc)
		help(EXIT_FAILURE);

	drop_caches(argv + optind);

	return 0;
}
