#include "customgraphstruct.h"
#include "mmap_reader.h"
#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int histo(const char *dir, const char *mode) {
    TogIndex plant_index;
    plant_index.tree = NULL; 
    char plants_path[256], sources_path[256];

    snprintf(plants_path, sizeof(plants_path), "%s/plants.dat", dir);
    snprintf(sources_path, sizeof(sources_path), "%s/sources_to_plants.dat", dir);

    for_each_line(plants_path, line_to_fac_idx_, (void*)&plant_index);
    for_each_line(sources_path, source_to_plant_cb, (void*)&plant_index);

    if (!strcmp(mode, "max")) {
        printMaxCapa(plant_index.tree);
    } else if (!strcmp(mode, "vol")) {
        printRealVolume(plant_index.tree);
    }

    FreeAVL(plant_index.tree);
    return 0;
}

int run_leaks(const char *dir, const char *target_id) {
    TogIndex full_index;
    full_index.tree = NULL;

    char paths[6][256];
    const char *filenames[] = {
        "plants.dat",
        "sources_to_plants.dat",
        "plants_to_storage.dat",
        "storage_to_junction.dat",
        "junction_to_service.dat",
        "service_to_customer.dat"
    };

    for (int i = 0; i < 6; i++) {
        snprintf(paths[i], 256, "%s/%s", dir, filenames[i]);
    }

    // 1. Build Index
    for_each_line(paths[0], line_to_fac_idx_, &full_index);

    // 2. Initial Flow (Source -> Plant)
    for_each_line(paths[1], source_to_plant_cb, &full_index);

    // 3. Process the push through all remaining layers
    // (Storage -> Junction -> Service -> Customer)
    for (int i = 2; i < 6; i++) {
        for_each_line(paths[i], push_flow_leak_cb, &full_index);
    }

    // 4. Report for specific plant
    IndNode *target_node = tog_index_find_IndNode(&full_index, target_id);
    if (target_node && target_node->connect[1]) {
        Park *p = (Park*)target_node->connect[1];
        printf("--- Leak Report for %s ---\n", target_id);
        printf("  Initial Received: %.3f\n", p->received);
        printf("  Total System Loss: %.3f\n", p->lost);
        printf("  Final Delivered:   %.3f\n", p->received - p->lost);
    } else {
        printf("Entity %s not found.\n", target_id);
    }

    FreeAVL(full_index.tree);
    return 0;
}

int main(const int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n %s <datadir> histo <type>\n %s <datadir> leaks <plant>\n", argv[0], argv[0]);
        return 1;
    }

    const char *dir = argv[1];
    const char *cmd = argv[2];

    if (strcmp(cmd, "histo") == 0) {
        if (argc != 4) return 1;
        return histo(dir, argv[3]);
    } 
    else if (strcmp(cmd, "leaks") == 0) {
        if (argc != 4) return 1;
        return run_leaks(dir, argv[3]);
    }
    
    return 0;
}
