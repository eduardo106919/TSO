CC = gcc
CFLAGS = -Wall -Wextra -Werror=pedantic -pedantic-errors -Wfatal-errors -O2
LIBURING = -luring

all: posix io_uring

posix: posix.c
	$(CC) $(CFLAGS) posix.c -o posix

io_uring: io_uring.c
	$(CC) $(CFLAGS) io_uring.c -o io_uring $(LIBURING)

run_posix: posix
	./posix input output_posix
	@if diff -q input output_posix >/dev/null; then \
		echo "[OK] Files are identical"; \
	else \
		echo "[FAIL] Files differ"; \
		exit 1; \
	fi

run_io_uring: io_uring
	./io_uring input output_io_uring
	@if diff -q input output_io_uring >/dev/null; then \
		echo "[OK] Files are identical"; \
	else \
		echo "[FAIL] Files differ"; \
		exit 1; \
	fi

clean:
	rm -f posix io_uring output_posix output_io_uring

.PHONY: all clean run_posix run_io_uring
