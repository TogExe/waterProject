#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1048576 
#define BAR_CHARS 40 
#define COL_TEAL    "\033[38;5;116m"
#define COL_GREEN   "\033[32m"
#define COL_RESET   "\033[0m"

typedef struct {
    FILE *fp;
    char *buffer;
    size_t pos;
    long count;
} OutFile;

void buf_write(OutFile *out, const char *start, size_t len) {
    if (out->pos + len > BUF_SIZE) {
        fwrite(out->buffer, 1, out->pos, out->fp);
        out->pos = 0;
    }
    memcpy(out->buffer + out->pos, start, len);
    out->pos += len;
    out->count++;
}

void update_ui(size_t current, size_t total, int final) {
    if (final) {
        fprintf(stderr, "\r %sProcessing Complete! [###################################] 100%%%s\n", COL_GREEN, COL_RESET);
        return;
    }
    double percent = (double)current / total;
    int hashes = (int)(percent * BAR_CHARS);
    fprintf(stderr, "\r %sScanning: [", COL_TEAL);
    for (int i = 0; i < BAR_CHARS; i++) {
        if (i < hashes) fputc('#', stderr);
        else fputc('-', stderr);
    }
    fprintf(stderr, "] %d%%%s", (int)(percent * 100), COL_RESET);
    fflush(stderr);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input> <output_dir> [id_filter]\n", argv[0]);
        return 1;
    }

    const char *filter_id = (argc > 3) ? argv[3] : NULL;
    size_t filter_len = filter_id ? strlen(filter_id) : 0;

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) return 1;

    struct stat st;
    fstat(fd, &st);
    size_t total_size = st.st_size;
    char *data = mmap(NULL, total_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) return 1;
    madvise(data, total_size, MADV_SEQUENTIAL | MADV_WILLNEED);

    OutFile out[6];
    const char *names[] = {
        "plants.dat", "sources_to_plants.dat", "plants_to_storage.dat", 
        "storage_to_junction.dat", "junction_to_service.dat", "service_to_customer.dat"
    };

    for(int i=0; i<6; i++) {
        char path[1024];
        sprintf(path, "%s/%s", argv[2], names[i]);
        out[i].fp = fopen(path, "w");
        out[i].buffer = malloc(BUF_SIZE);
        out[i].pos = 0;
        out[i].count = 0;
    }

    char *ptr = data;
    char *end = data + total_size;
    size_t last_ui_update = 0;
    size_t ui_step = total_size / 50; 

    while (ptr < end) {
        size_t offset = (size_t)(ptr - data);
        if (offset - last_ui_update > ui_step) {
            update_ui(offset, total_size, 0);
            last_ui_update = offset;
        }

        char *line_start = ptr;
        char *curr = ptr;
        while (curr < end && *curr != '\n') curr++;
        size_t line_len = curr - line_start + (curr < end ? 1 : 0);
        char *line_end = curr;
        ptr = curr + 1;

        // "LEAKS" OPTIMIZATION: If filter_id is set, skip lines that don't match
        if (filter_id && !memmem(line_start, line_len, filter_id, filter_len)) {
            continue;
        }

        // Parse columns only for matching lines
        char *cols[6];
        int c = 0;
        char *col_ptr = line_start;
        cols[c++] = col_ptr;
        while (col_ptr < line_end) {
            if (*col_ptr == ';') { if (c < 6) cols[c++] = col_ptr + 1; }
            col_ptr++;
        }

        if (c >= 4) {
            auto int is_dash(char* s, char* e) {
                while (s < e && (*s == ' ' || *s == '\t')) s++;
                return (s < e && *s == '-');
            };

            char *c1s = cols[0], *c1e = cols[1]-1;
            char *c3s = cols[2], *c3e = cols[3]-1;
            char *c4s = cols[3], *c4e = (c > 4 ? cols[4]-1 : line_end);
            char *c5s = (c > 4 ? cols[4] : line_end), *c5e = line_end;

            if (is_dash(c1s, c1e)) {
                if (is_dash(c5s, c5e)) buf_write(&out[0], line_start, line_len);
                else {
                    if (is_dash(c4s, c4e)) buf_write(&out[2], line_start, line_len);
                    else buf_write(&out[1], line_start, line_len);
                }
            } else if (is_dash(c4s, c4e)) {
                if (memmem(c3s, (c3e-c3s), "Cust #", 6))           buf_write(&out[5], line_start, line_len);
                else if (memmem(c3s, (c3e-c3s), "Service #", 9))   buf_write(&out[4], line_start, line_len);
                else if (memmem(c3s, (c3e-c3s), "Junction #", 10)) buf_write(&out[3], line_start, line_len);
                else if (memmem(c3s, (c3e-c3s), "Storage #", 9))   buf_write(&out[3], line_start, line_len);
            }
        }
    }

    update_ui(total_size, total_size, 1);
    fprintf(stderr, "\n %sFiltered Statistics (%s):%s\n", COL_TEAL, filter_id ? filter_id : "All", COL_RESET);
    for(int i=0; i<6; i++) {
        if (out[i].pos > 0) fwrite(out[i].buffer, 1, out[i].pos, out[i].fp);
        fprintf(stderr, "  %-25s : %ld lines\n", names[i], out[i].count);
        fclose(out[i].fp);
        free(out[i].buffer);
    }
    munmap(data, total_size);
    close(fd);
    return 0;
}
