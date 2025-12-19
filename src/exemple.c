#include <stdio.h>

int main() {
    // "w" = write mode; creates file if it doesn't exist
    FILE *f = fopen("parks.csv", "w");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }

    // write a header line
    fprintf(f, "ID,Type,Capacity\n");

    // write some sample data
    fprintf(f, "KU000032A,Unit,8050.00\n");
    fprintf(f, "CL100007D,Module,12592.00\n");

    fclose(f);
    return 0;
}
