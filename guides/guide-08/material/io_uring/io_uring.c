#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <liburing.h>

#define CHECK(cond, fmt, ...)                          \
    do {                                               \
        if (!(cond)) {                                 \
            fprintf(stderr, fmt "\n", ##__VA_ARGS__);  \
            io_uring_queue_exit(&ring);                \
			close(src_fd);                             \
			close(dst_fd);                             \
			return 1;                                  \
        }                                              \
    } while (0)

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
    assert(st.st_size % BLK_SIZE == 0);

	int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dst_fd < 0) {
		perror("open destination");
		close(src_fd);
		return 1;
	}

	// IO_URING setup
	struct io_uring ring;
	if (io_uring_queue_init(1, &ring, 0) < 0) {
		perror("io_uring_queue_init");
		close(src_fd);
		close(dst_fd);
		return 1;
	}

	char buffer[BLK_SIZE];
	struct io_uring_sqe *sqe;
	struct io_uring_cqe *cqe;
	int ret;

	for (off_t offset = 0; offset < st.st_size; offset += BLK_SIZE) {
		// Read
		sqe = io_uring_get_sqe(&ring);
		CHECK(sqe != NULL, "io_uring_get_sqe (read) failed");
		// TODO

		// Write
		// TODO
	}

	io_uring_queue_exit(&ring);
	close(src_fd);
	close(dst_fd);
	return 0;
}
