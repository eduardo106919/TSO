#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#define BLK_SIZE 4096

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
		return 1;
	}

	int src_fd = open(argv[1], O_RDONLY);
	if (src_fd < 0) {
		perror("open source");
		return 1;
	}

	struct stat st;
	if (fstat(src_fd, &st) < 0) {
		perror("fstat");
		close(src_fd);
		return 1;
	}
    size_t file_size = st.st_size;
    assert(file_size % BLK_SIZE == 0);

	int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dst_fd < 0) {
		perror("open destination");
		close(src_fd);
		return 1;
	}

	char buffer[BLK_SIZE];


	for (off_t src_offset = 0; src_offset < st.st_size; src_offset += BLK_SIZE) {
        ssize_t bytes_read = pread(src_fd, buffer, BLK_SIZE, src_offset);
        if (bytes_read != BLK_SIZE) {
            perror("pread");
            close(src_fd);
            close(dst_fd);
            return 1;
        }

        ssize_t bytes_written = pwrite(dst_fd, buffer, BLK_SIZE, src_offset);
        if (bytes_written != BLK_SIZE) {
            perror("pwrite");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
	}

	close(src_fd);
	close(dst_fd);
	return 0;
}
