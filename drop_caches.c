#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <stdbool.h>

#define die(args...) error(EXIT_FAILURE, errno, args)

void help(int exit_status)
{
	printf(
		"Usage: drop_caches [OPTIONS]... FILE...            \n"
		"Drop the page caches of the given files.           \n"
		"                                                   \n"
		"  -h            show help                          \n"
		"  -i INTERVAL   drop caches every INTERVAL seconds \n"
		"  -D            daemonize                          \n"
	);
	exit(exit_status);
}

void drop_caches(int *fds, char **file_names, int num_files)
{
	int i;

	for (i = 0; i < num_files; i++) {
		printf("%s... ", file_names[i]);
		fdatasync(fds[i]);
		if (posix_fadvise(fds[i], 0, 0, POSIX_FADV_DONTNEED) < 0)
			die("fadvise failed");
		printf("caches dropped\n");
	}
}

void drop_caches_every(int *fds, char **file_names, int num_files,
		       int interval, bool daemonize)
{
	if (daemonize) {
		pid_t pid = fork();
		if (pid < 0)
			die("cannot fork");
		if (pid > 0)
			return;
	}

	for (;;) {
		drop_caches(fds, file_names, num_files);
		sleep(interval);
	}
}

int *open_files(char **files, int num_files)
{
	int i;
	int *fds;
	
	fds = malloc(sizeof(int) * num_files);
	if (fds == NULL)
		die("malloc failed");

	for (i = 0; i < num_files; i++) {
		fds[i] = open(files[i], O_RDONLY);
		if (fds[i] < 0)
			die("cannot open %s", files[i]);
	}

	return fds;
}

int main(int argc, char **argv)
{
	int opt;
	bool daemonize = false;

	char **file_names;
	int *fds;
	int num_files;
	int interval = 0;

	while ((opt = getopt(argc, argv, "hi:D")) != -1) {
		switch (opt) {
		case 'h':
			help(EXIT_SUCCESS);
		case 'i':
			interval = atoi(optarg);
			break;
		case 'D':
			daemonize = true;
			break;
		default:
			help(EXIT_FAILURE);
		}
	}

	file_names = argv + optind;
	num_files = argc - optind;

	if (num_files <= 0)
		help(EXIT_FAILURE);

	fds = open_files(file_names, num_files);

	if (interval)
		drop_caches_every(fds, file_names, num_files,
				  interval, daemonize);
	else
		drop_caches(fds, file_names, num_files);

	return 0;
}
