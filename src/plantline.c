#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Type parse_park_type(const char *s)
{
    /* trailing space is present in input */
    if (strncmp(s, "Unit", 4) == 0)
        return UNIT;
    if (strncmp(s, "Module", 6) == 0)
        return MODULE;
    if (strncmp(s, "Facility complex", 16) == 0)
        return COMPLEX;
    if (strncmp(s, "Plant", 5) == 0)
        return PLANT;

    return UNKNOWN;
}


void line_to_fac_idx_(const char * l,void *data)
{
	TogIndex * idx = (TogIndex*)data;
    
    // Find the end of the line (either newline or reasonable max)
    const char *line_end = l;
    size_t max_len = 200; // Increased from 100 to be safe
    while (*line_end != '\n' && *line_end != '\0' && (line_end - l) < max_len) {
        line_end++;
    }
    
    // Create a null-terminated copy on the heap
    size_t line_len = line_end - l;
    char *line_copy = malloc(line_len + 1);
    if (!line_copy) return;
    
    memcpy(line_copy, l, line_len);
    line_copy[line_len] = '\0';
    
    char type_and_id[50];
    char id[11];
    float stoval;
    int count = sscanf(line_copy, "-;%49[^;];-;%f;-", type_and_id, &stoval);
	
    free(line_copy);
	
	// trying to get knowledge of the facility type
	if (count==2)
	{
    	char *hash_pos = strchr(type_and_id, '#');
		if (hash_pos && sscanf(hash_pos + 1, "%10[^;]", id) == 1) 
		{
    		Park * nfac = malloc(sizeof(Park));
			if (nfac) {
				snprintf(nfac->id, sizeof(nfac->id), "%s", id);
				nfac->capacity = stoval;
				nfac->received = 0.0;
				nfac->lost = 0.0;
				nfac->type = parse_park_type(type_and_id);
				nfac->storages.head = NULL;
				nfac->sources.head = NULL;
				tog_index_add(idx,id,(void*)nfac);
			}
		}
    }
}

void source_to_plant_cb(const char *l, void *data) {
    TogIndex * idx = (TogIndex*)data;
    
    // Find the end of the line (either newline or reasonable max)
    const char *line_end = l;
    size_t max_len = 300; // Increased to be safe
    while (*line_end != '\n' && *line_end != '\0' && (line_end - l) < max_len) {
        line_end++;
    }
    
    // Create a null-terminated copy on the heap
    size_t line_len = line_end - l;
    char *line_copy = malloc(line_len + 1);
    if (!line_copy) return;
    
    memcpy(line_copy, l, line_len);
    line_copy[line_len] = '\0';
    
    char source[30];
    char entity_type_and_id[50];
    char entity_id[11];
    float volume;
    float leak_percent;

    int count = sscanf(line_copy, "-;%29[^;];%49[^;];%f;%f", source, entity_type_and_id, &volume, &leak_percent);
    
    free(line_copy);
    
    if (count == 4) {
        char *hash_pos = strchr(entity_type_and_id, '#');
        if (hash_pos && sscanf(hash_pos + 1, "%10[^;]", entity_id) == 1) {
            IndNode * pentity = tog_index_find_IndNode(idx, entity_id);
            if (!pentity||!pentity->connect[1])return;
			Park * entity = (Park*)pentity->connect[1];
            if (entity) {
                entity->received += volume;
                entity-> lost    += volume*leak_percent/100.0f;
            }
        }
    }
}
