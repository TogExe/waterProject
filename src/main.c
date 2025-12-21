#include "customgraphstruct.h"
#include "mmap_reader.h"
#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Historical data display logic.
 */
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
    } else if (!strcmp(mode, "src")) {
        printVolume(plant_index.tree);
    } else if (!strcmp(mode, "real")) {
        printRealVolume(plant_index.tree);
    } else {
        fprintf(stderr, "\nWrong param : %s use : max, src, or real\n", mode);
        FreeAVL(plant_index.tree);
        return 1;
    }

    FreeAVL(plant_index.tree);
    return 0;
}

/**
 * Leak calculation orchestrator.
 * Patched to correctly chain layers and clean up memory.
 */
 static char* get_type_text(char type) {
   switch (type) {
     case 'U': return "Unit ";
     case 'M': return "Module ";
     case 'C': return "Facility complex ";
     case 'P': return "Plant ";
     default: return "MissigNo? ";
   }
 }
 
int run_leaks(const char *dir, const char *target_id) {
    TogIndex full_index = { .tree = NULL };
    TogIndex entity_index = { .tree = NULL };
    char paths[6][256];
    const char *filenames[] = {
        "plants.dat", "sources_to_plants.dat", "plants_to_storage.dat",
        "storage_to_junction.dat", "junction_to_service.dat", "service_to_customer.dat"
    };

    for (int i = 0; i < 6; i++) {
        snprintf(paths[i], 256, "%s/%s", dir, filenames[i]);
    }

    // Layer 0 & 1: Plants and Sources
    for_each_line(paths[0], line_to_fac_idx_, &full_index);
    for_each_line(paths[1], source_to_plant_cb, &full_index);

    // Layer 2: Plants -> Storage
    Context storage_ctx = { .up = &full_index, .in = { .tree = NULL } };
    for_each_line(paths[2], storage_handler, &storage_ctx);

    // Layers 3-5: Storage -> Junction -> Service -> Customer
    // Chaining 'in' index to 'up' index ensures child nodes find their parents.
    Context layer_contexts[3];
    TogIndex *current_up = &storage_ctx.in;

    for (int i = 0; i < 3; i++) {
        layer_contexts[i].up = current_up;
        layer_contexts[i].in.tree = NULL;
        for_each_line(paths[3 + i], entity_handler, &layer_contexts[i]);
        current_up = &layer_contexts[i].in; 
    }

    // Report Logic
    IndNode *target_node = tog_index_find_IndNode(&full_index, (char*)target_id);
    if (target_node && target_node->connect[1]) {
        Park *p = (Park *)target_node->connect[1];
        float system_loss = 0;
        
        // Start recursion from Park level.
        // 0.0f pc is used because the Plant's specific link loss is already in p->lost.
        groupProject(&p->ng, 0.0f, p->received, &system_loss);
        
        float total_lost = p->lost + system_loss;
        fprintf(stdout,"--- Leak Report for %s ---\n", target_id);
        fprintf(stdout,"  Received at Plant: %.3f\n", p->received);
        fprintf(stdout,"  Total System Loss: %.3f\n", total_lost);
        fprintf(stdout,"  Final Delivered:   %.3f (%.2f%% loss)\n", 
               p->received - total_lost, (total_lost / p->received) * 100.0f);
       	printf("%s#%s;%f;%f\n",get_type_text(p->type),target_id,p->received,
       		total_lost);
    } else {
        fprintf(stderr,"Entity %s not found.\n", target_id);
    }

    // Cleanup: Properly free all allocated AVL trees
    for (int i = 0; i < 3; i++) {
        FreeAVL(layer_contexts[i].in.tree);
    }
    FreeAVL(storage_ctx.in.tree);
    FreeAVL(full_index.tree);

    return 0;
}

int main(const int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage:\n %s <datadir> histo <max|src|real>\n %s <datadir> leaks <plant_id>\n", argv[0], argv[0]);
        return 1;
    }

    const char *dir = argv[1];
    const char *cmd = argv[2];

    if (strcmp(cmd, "histo") == 0) {
        return histo(dir, argv[3]);
    } 
    else if (strcmp(cmd, "leaks") == 0) {
        return run_leaks(dir, argv[3]);
    } else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        return 1;
    }
    
    return 0;
}
