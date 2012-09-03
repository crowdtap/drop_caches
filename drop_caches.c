#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void help(int exit_status)
{
	printf(
		"Usage: drop_caches [OPTIONS]... FILE...            \n"
		"Drop the page caches of the given files.           \n"
		"                                                   \n"
		"  -h            show help                          \n"
		"  -i INTERVAL   drop caches every INTERVAL seconds \n"
	);
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
		fprintf(stderr, "caches dropped\n");
	}
}

void drop_caches_every(char **files, int interval)
{
	for (;;) {
		drop_caches(files);
		sleep(interval);
	}
}

int main(int argc, char **argv)
{
	int opt;
	int interval = 0;

	while ((opt = getopt(argc, argv, "hi:")) != -1) {
		switch (opt) {
		case 'h':
			help(EXIT_SUCCESS);
		case 'i':
			interval = atoi(optarg);
			break;
		default:
			help(EXIT_FAILURE);
		}
	}

	if (optind >= argc)
		help(EXIT_FAILURE);

	if (interval)
		drop_caches_every(argv + optind, interval);
	else
		drop_caches(argv + optind);

	return 0;
}
