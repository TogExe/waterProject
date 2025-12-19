#ifndef MMAP_READER_H
#define MMAP_READER_H
int for_each_line(const char *filename,void (*line_fn)(const char*,void*),void *ctx);
#endif
