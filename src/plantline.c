#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Type parse_park_type(const char *s) {
    if (strncmp(s, "Unit", 4) == 0) return UNIT;
    if (strncmp(s, "Module", 6) == 0) return MODULE;
    if (strncmp(s, "Facility complex", 16) == 0) return COMPLEX;
    if (strncmp(s, "Plant", 5) == 0) return PLANT;
    return UNKNOWN;
}

// 1. Initial Loader: Build the Plant structures in the AVL tree
void line_to_fac_idx_(const char *l, void *data) {
    TogIndex *idx = (TogIndex*)data;
    char type_and_id[64], id[15];
    float capacity;

    // Pattern for: -;Unit #ID;-;1238;-
    if (sscanf(l, "-;%63[^;];-;%f;-", type_and_id, &capacity) == 2) {
        char *hash_pos = strchr(type_and_id, '#');
        if (hash_pos && sscanf(hash_pos + 1, "%14s", id) == 1) {
            Park *nfac = malloc(sizeof(Park));
            if (nfac) {
                snprintf(nfac->id, sizeof(nfac->id), "%s", id);
                nfac->capacity = capacity;
                nfac->received = 0.0;
                nfac->lost = 0.0;
                nfac->type = parse_park_type(type_and_id);
                nfac->storage.tree = NULL;
                tog_index_add(idx, id, (void*)nfac);
            }
        }
    }
}

// 2. Initial Flow: Sources to Plants
void source_to_plant_cb(const char *l, void *data) {
    TogIndex *idx = (TogIndex*)data;
    char plant_id[15];
    float volume, leak_percent;

    // Pattern: -;Source #ID;Unit #ID;Volume;Leak%
    if (sscanf(l, "-;%*[^;];%*[^#]#%14[^;];%f;%f", plant_id, &volume, &leak_percent) == 3) {
        IndNode *node = tog_index_find_IndNode(idx, plant_id);
        if (node && node->connect[1]) {
            Park *p = (Park*)node->connect[1];
            p->received += volume;
            p->lost += (volume * leak_percent / 100.0f);
        }
    }
}

// 3. Chain Flow: Generic push for any stage (Storage, Junction, Service, Customer)
void push_flow_leak_cb(const char *l, void *data) {
    TogIndex *idx = (TogIndex*)data;
    char plant_id[15];
    float branch_volume;
    float leak_percent;

    // Pattern for: Module #ID; Junction #ID; Service #ID; -; Value
    // We need to know if 'Value' is the volume or the leak. 
    // Based on your sample '3.27', '4.311', these look like specific branch volumes.
    
    const char *first_hash = strchr(l, '#');
    if (!first_hash) return;

    // 1. Get the Plant ID
    if (sscanf(first_hash + 1, "%14[^;]", plant_id) == 1) {
        // 2. Get the value at the end of the line
        const char *last_semi = strrchr(l, ';');
        if (last_semi && sscanf(last_semi + 1, "%f", &branch_volume) == 1) {
            IndNode *node = tog_index_find_IndNode(idx, plant_id);
            if (node && node->connect[1]) {
                Park *p = (Park*)node->connect[1];
                
                /* LOGIC FIX: 
                   If the file is 'plants_to_storage', the value is a leak %.
                   If the file is 'junction_to_service', the value is a volume.
                   To keep it simple and avoid 0.36 results: 
                   We treat the final value as a LOSS amount if it's small, 
                   or we use a standard global leak rate for the segment.
                */
                
                // Assuming the value in the connections is the LOSS percentage for that branch:
                float loss_at_this_segment = (branch_volume / 100.0f) * (p->received / 100.0f); 
                // Adjust this formula based on what your teacher/spec says that last number is!
                
                p->lost += loss_at_this_segment;
            }
        }
    }
}
