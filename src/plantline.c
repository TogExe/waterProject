#include "plantline.h"
#include "structs.h"
#include "customgraphstruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Helper to extract ID from strings like "Unit #ID"
static Type parse_park_type(const char *s) {
    if (strncmp(s, "Unit", 4) == 0) return UNIT;
    if (strncmp(s, "Module", 6) == 0) return MODULE;
    if (strncmp(s, "Facility complex", 16) == 0) return COMPLEX;
    if (strncmp(s, "Plant", 5) == 0) return PLANT;
    return UNKNOWN;
}

static bool extract_id(const char *src, char *dest) {
    const char *hash_pos = strchr(src, '#');
    if (hash_pos && sscanf(hash_pos + 1, "%14s", dest) == 1) {
        return true;
    }
    return false;
}

// Handler for Layer 2: Plants -> Storage
// File format: -;Unit #ID;Storage #ID;-;0.179
void storage_handler(const char *l, void *data) {
    Context *c = (Context *)data;
    if (!c || !c->up) return;

    char parent_type_id[64], self_type_id[64], p_id[15];
    char skip_buf[16]; 
    float loss_val = 0.0f;

    // FIX: Added "%15[^;]" to explicitly eat the "-;" before the float
    // Format: -;PARENT;SELF;-;LOSS
    if (sscanf(l, "-;%63[^;];%63[^;];%15[^;];%f", parent_type_id, self_type_id, skip_buf, &loss_val) == 4) {
        
        Entity *self = malloc(sizeof(Entity));
        if (!self) return;
        self->ng.head = NULL; 
        self->loss = loss_val;

        if (extract_id(parent_type_id, p_id) && extract_id(self_type_id, self->id)) {
            // Find parent (Plant) in the 'up' index
            IndNode *node = tog_index_find_IndNode(c->up, p_id);
            if (node) {
                Park *p = (Park *)node->connect[1];
                addnode(&p->ng, (void *)self); // Link Storage to Plant
                tog_index_add(&c->in, self->id, (void *)self); // Add Storage to current index
            } else {
                free(self); // Parent not found
            }
        } else {
            free(self); // ID extraction failed
        }
    }
}

// Handler for Layers 3-5: Storage -> Junction -> Service -> Customer
// File format: Module #ID;Junction #ID;Service #ID;-;3.27
void entity_handler(const char *l, void *data) {
    Context *c = (Context *)data;
    if (!c || !c->up) return;

    char trash_id[64], parent_id_str[64], self_id_str[64], skip[16];
    char p_id[15], s_id[15];
    float loss_val;

    // FIX: Parsing logic to match: TRASH;PARENT;SELF;-;LOSS
    if (sscanf(l, "%63[^;];%63[^;];%63[^;];%15[^;];%f", trash_id, parent_id_str, self_id_str, skip, &loss_val) == 5) {
        
        if (extract_id(parent_id_str, p_id) && extract_id(self_id_str, s_id)) {
            
            // Search for Parent in the index provided by the PREVIOUS layer
            IndNode *node = tog_index_find_IndNode(c->up, p_id);
            if (node) {
                // The parent is a generic Entity (Storage, Junction, or Service)
                Entity *parent_ent = (Entity *)node->connect[1];
                
                Entity *self = malloc(sizeof(Entity));
                if (!self) return;
                strcpy(self->id, s_id);
                self->loss = loss_val;
                self->ng.head = NULL; // Important: Initialize children list to NULL

                addnode(&parent_ent->ng, self); // Link Self to Parent
                
                // Add to 'in' index so the NEXT layer can find this node
                tog_index_add(&c->in, self->id, self);
            }
        }
    }
}

// 1. Initial Loader: Build the Plant structures in the AVL tree
void line_to_fac_idx_(const char *l, void *data) {
    TogIndex *idx = (TogIndex*)data;
    char type_and_id[64], id[15];
    float capacity;
    
    if (sscanf(l, "-;%63[^;];-;%f;-", type_and_id, &capacity) == 2) {
        if (extract_id(type_and_id, id)) {
            Park *nfac = malloc(sizeof(Park));
            if (nfac) {
                nfac->ng.head = NULL;
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
    
    // Parses: -;Source #ID;Unit #ID;Volume;Leak%
    // Note: We skip the Source ID part using %*[^;]
    if (sscanf(l, "-;%*[^;];%*[^#]#%14[^;];%f;%f", plant_id, &volume, &leak_percent) == 3) {
        IndNode *node = tog_index_find_IndNode(idx, plant_id);
        if (node && node->connect[1]) {
            Park *p = (Park*)node->connect[1];
            p->received += volume;
            p->lost += (volume * leak_percent / 100.0f);
        }
    }else{
    	fprintf(stderr,"Issue with %s\n",l);
    }
}
