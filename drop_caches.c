#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd, i;

	for (i = 1; i < argc; i++) {
		fprintf(stderr, "%s... ", argv[i]);
		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			perror("cannot open file");
			continue;
		}

		fdatasync(fd);
		if (posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED) < 0) {
			perror("fadvise doesn't work");
			continue;
		}
		close(fd);
		fprintf(stderr, "flushed\n");
	}
	return 0;
}
