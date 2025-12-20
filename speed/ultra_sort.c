#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1048576 // 1MB output buffers

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

int main(int argc, char *argv[]) {
    if (argc < 3) return 1;

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    OutFile out[6];
    const char *names[] = {"/plants.dat", "/sources_to_plants.dat", "/plants_to_storage.dat", 
                           "/storage_to_junction.dat", "/junction_to_service.dat", "/service_to_customer.dat"};
    
    for(int i=0; i<6; i++) {
        char path[1024];
        sprintf(path, "%s%s", argv[2], names[i]);
        out[i].fp = fopen(path, "w");
        out[i].buffer = malloc(BUF_SIZE);
        out[i].pos = 0;
    }

    char *ptr = data;
    char *end = data + st.st_size;

    while (ptr < end) {
        char *line_start = ptr;
        char *cols[5];
        int c = 0;

        // Fast scan for columns
        char *curr = ptr;
        while (c < 5 && *curr != '\n' && curr < end) {
            if (*curr == ';') {
                cols[c++] = curr;
            }
            curr++;
        }
        while (*curr != '\n' && curr < end) curr++; // Find end of line
        size_t line_len = curr - line_start + 1;

        // Logic using direct pointer offsets (Avoids strtok/strcmp)
        // col[0] is at line_start, col[2] is after 2nd semicolon, etc.
        char c0 = line_start[0];
        char c2_first = cols[1][1]; 
        char c3_first = cols[2][1];
        char c4_first = cols[3][1];

        if (c0 == '-') {
            if (c4_first == '-') {
                if (c2_first == '-') buf_write(&out[0], line_start, line_len);
                else if (c3_first == '-') buf_write(&out[2], line_start, line_len);
            } else if (c3_first != '-') {
                buf_write(&out[1], line_start, line_len);
            }
        } else if (c3_first == '-') {
            if (c2_first == 'C') buf_write(&out[5], line_start, line_len);
            else if (c2_first == 'S') buf_write(&out[4], line_start, line_len);
            else if (c2_first == 'J') buf_write(&out[3], line_start, line_len);
        }
        ptr = curr + 1;
    }

    for(int i=0; i<6; i++) {
        fwrite(out[i].buffer, 1, out[i].pos, out[i].fp);
        fclose(out[i].fp);
        free(out[i].buffer);
    }
    munmap(data, st.st_size);
    close(fd);
    return 0;
}
