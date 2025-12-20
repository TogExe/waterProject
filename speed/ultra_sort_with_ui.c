#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1048576 
#define BAR_CHARS 20 // The width of your [####--------] bar

// Colors from your AWK script
#define COL_TEAL    "\033[38;5;116m"
#define COL_GREEN   "\033[32m"
#define COL_RESET   "\033[0m"

typedef struct {
    FILE *fp;
    char *buffer;
    size_t pos;
} OutFile;

void buf_write(OutFile *out, const char *start, size_t len) {
    if (out->pos + len > BUF_SIZE) {
        fwrite(out->buffer, 1, out->pos, out->fp);
        out->pos = 0;
    }
    memcpy(out->buffer + out->pos, start, len);
    out->pos += len;
}

void update_ui(size_t current, size_t total, int final) {
    if (final) {
        fprintf(stderr, "\r %sSorting Complete! [####################] 100%%%s\n", COL_GREEN, COL_RESET);
        return;
    }
    
    double percent = (double)current / total;
    int hashes = (int)(percent * BAR_CHARS);
    
    fprintf(stderr, "\r %sSorting: [", COL_TEAL);
    for (int i = 0; i < BAR_CHARS; i++) {
        if (i < hashes) fputc('#', stderr);
        else fputc('-', stderr);
    }
    fprintf(stderr, "] %d%%%s", (int)(percent * 100), COL_RESET);
    fflush(stderr);
}

int main(int argc, char *argv[]) {
    if (argc < 3) return 1;

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
    const char *names[] = {"/plants.dat", "/sources_to_plants.dat", "/plants_to_storage.dat", 
                           "/storage_to_junction.dat", "/junction_to_service.dat", "/service_to_customer.dat"};
    
    // UI Setup: Inform user of files handled
    fprintf(stderr, " %sHandling 6 output files...%s\n", COL_TEAL, COL_RESET);

    for(int i=0; i<6; i++) {
        char path[1024];
        sprintf(path, "%s%s", argv[2], names[i]);
        out[i].fp = fopen(path, "w");
        out[i].buffer = malloc(BUF_SIZE);
        out[i].pos = 0;
    }

    char *ptr = data;
    char *end = data + total_size;
    size_t last_ui_update = 0;
    size_t ui_step = total_size / 50; // Update UI every 2% to keep it smooth but fast

    while (ptr < end) {
        size_t offset = (size_t)(ptr - data);
        if (offset - last_ui_update > ui_step) {
            update_ui(offset, total_size, 0);
            last_ui_update = offset;
        }

        char *line_start = ptr;
        char *cols[5];
        int c = 0;
        char *curr = ptr;

        while (curr < end && *curr != '\n') {
            if (*curr == ';') { if (c < 5) cols[c++] = curr; }
            curr++;
        }
        size_t line_len = curr - line_start + (curr < end ? 1 : 0);

        if (filter_id && (line_len < filter_len || memmem(line_start, line_len, filter_id, filter_len) == NULL)) {
            ptr = curr + 1;
            continue;
        }

        if (c >= 4) {
            char c0 = line_start[0];
            char c2_first = cols[1][1]; 
            char c3_first = cols[2][1];
            char c4_first = cols[3][1];

            if (c0 == '-') {
                if (c4_first == '-') {
                    if (c2_first == '-') buf_write(&out[0], line_start, line_len);
                    else if (c3_first == '-') buf_write(&out[2], line_start, line_len);
                } else if (c3_first != '-') buf_write(&out[1], line_start, line_len);
            } else if (c3_first == '-') {
                if (c2_first == 'C') buf_write(&out[5], line_start, line_len);
                else if (c2_first == 'S') buf_write(&out[4], line_start, line_len);
                else if (c2_first == 'J') buf_write(&out[3], line_start, line_len);
            }
        }
        ptr = curr + 1;
    }

    update_ui(total_size, total_size, 1);

    for(int i=0; i<6; i++) {
        if (out[i].pos > 0) fwrite(out[i].buffer, 1, out[i].pos, out[i].fp);
        fclose(out[i].fp);
        free(out[i].buffer);
    }
    munmap(data, total_size);
    close(fd);
    return 0;
}
