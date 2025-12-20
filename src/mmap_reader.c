#include "mmap_reader.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Internal structure to manage memory-mapped file state.
 */
typedef struct {
	char *data;
	size_t size;
	int fd;
} MappedFile;

/**
 * Maps a file into memory for reading.
 * Returns 0 on success, -1 on failure.
 */
static int map_file(const char *filename, MappedFile *m)
{
	struct stat sb;

	m->fd = open(filename, O_RDONLY);
	if (m->fd < 0) {
	perror("Error: Could not open file");
	return -1;
	}

	if (fstat(m->fd, &sb) < 0) {
	perror("Error: Could not get file stats");
	close(m->fd);
	return -1;
	}

	m->size = (size_t)sb.st_size;
	if (m->size == 0) {
	close(m->fd);
	return -1; // Cannot map an empty file
	}

	m->data = mmap(NULL, m->size, PROT_READ, MAP_PRIVATE, m->fd, 0);
	if (m->data == MAP_FAILED) {
	perror("Error: mmap failed");
	close(m->fd);
	return -1;
	}

	return 0;
}

/**
 * Unmaps the file and closes the file descriptor.
 */
static void unmap_file(MappedFile *m)
{
	if (m->data && m->data != MAP_FAILED) {
	munmap(m->data, m->size);
	}
	if (m->fd != -1) {
	close(m->fd);
	}
	m->data = NULL;
	m->fd = -1;
	m->size = 0;
}

/**
 * Iterates through each line of a file using memory mapping.
 * Note: The line passed to the callback is NOT null-terminated
 * because mmap memory is often read-only.
 */
int for_each_line(const char *filename, void (*fn)(const char *, void *), void *ctx)
{
	MappedFile m = {NULL, 0, -1};

	if (map_file(filename, &m) < 0) {
	return -1;
	}



	char *current_pos = m.data;
	char *end_of_file = m.data + m.size;

	while (current_pos < end_of_file) {
	// Find the next newline character
	char *newline_pos = memchr(current_pos, '\n', (size_t)(end_of_file - current_pos));
	char *line_end = newline_pos ? newline_pos : end_of_file;

	if (line_end > current_pos) {
	/* * Since the map is PROT_READ, we cannot insert a '\0'.
	 * The callback implementation must handle line length
	 * or copy the string if null-termination is required.
	 */
	fn(current_pos, ctx);
	}

	// Move to the character after the newline
	current_pos = newline_pos ? newline_pos + 1 : end_of_file;
	}

	unmap_file(&m);
	return 0;
}
