#include "customgraphstruct.h"
#include "mmap_reader.h"
#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int histo(const char *dir,const char * mode) {
    TogIndex plant_index;
    plant_index.tree = NULL; 
    char plants_path[256], sources_path[256];

    snprintf(plants_path, sizeof(plants_path), "%s/plants.dat", dir);
    snprintf(sources_path, sizeof(sources_path), "%s/sources_to_plants.dat", dir);

    for_each_line(plants_path, line_to_fac_idx_, (void*)&plant_index);
    for_each_line(sources_path, source_to_plant_cb, (void*)&plant_index);

    if (!strcmp(mode,"max")){
        printMaxCapa(plant_index.tree);
    } else if (!strcmp(mode,"vol")){
        printRealVolume(plant_index.tree);
    }

    FreeAVL(plant_index.tree);
    return 0;
}
#include "plantline.h"
#include "structs.h"
#include "mmap_reader.h"
#include <stdio.h>
#include <string.h>

// Helper to propagate volume from a parent entity to a child storage/node
void flow_p_to_s_cb(const char *l, void *data) {
    TogIndex *idx = (TogIndex*)data;
    char p_id[15], s_id[15];
    float leak_percent;

    // Format: -;Type #ID;Storage #ID;-;Leak%
    // Note: Adjust sscanf based on your specific .dat columns
    if (sscanf(l, "-;%*[^#]#%14[^;];Storage #%14[^;];-;%f", p_id, s_id, &leak_percent) == 3) {
        IndNode *p_node = tog_index_find_IndNode(idx, p_id);
        if (!p_node || !p_node->connect[1]) return;
        
        Park *parent = (Park*)p_node->connect[1];
        // In a real push system, you'd need to know how much of the parent's 
        // volume goes to THIS specific storage. Assuming 1:1 or broadcast for this example:
        float volume_to_move = parent->received; 
        
        // Calculate loss at this step
        parent->lost += (volume_to_move * leak_percent / 100.0f);
    }
}

int run_leaks(const char *dir, const char *target_id) {
    TogIndex full_index;
    full_index.tree = NULL;

    char paths[6][256];
    snprintf(paths[0], 256, "%s/plants.dat", dir);
    snprintf(paths[1], 256, "%s/sources_to_plants.dat", dir);
    snprintf(paths[2], 256, "%s/plants_to_storage.dat", dir);
    snprintf(paths[3], 256, "%s/storage_to_junction.dat", dir);
    snprintf(paths[4], 256, "%s/junction_to_service.dat", dir);
    snprintf(paths[5], 256, "%s/service_to_customer.dat", dir);

    // 1. Load the Plants into the index
    for_each_line(paths[0], line_to_fac_idx_, (void*)&full_index);

    // 2. Push Source -> Plant (This fills entity->received)
    for_each_line(paths[1], source_to_plant_cb, (void*)&full_index);

    // 3. Process Leaks (Plants to Storage)
    // You would create specific callbacks for each file to update the 'lost' totals
    for_each_line(paths[2], con_plan_sto, (void*)&full_index);

    // 4. Find the specific plant requested by the user
    IndNode *target_node = tog_index_find_IndNode(&full_index, target_id);
    
    if (target_node && target_node->connect[1]) {
        Park *p = (Park*)target_node->connect[1];
        printf("Report for Plant/Unit %s:\n", target_id);
        printf("  Total Received: %.2f\n", p->received);
        printf("  Total Lost:     %.2f\n", p->lost);
        printf("  Net Delivered:  %.2f\n", p->received - p->lost);
    } else {
        printf("Entity %s not found.\n", target_id);
    }

    FreeAVL(full_index.tree);
    return 0;
}

int main(const int argc, char **argv){
    if (argc < 3){
        fprintf(stderr, "Usage:\n %s <datadir> histo <type>\n %s <datadir> leaks <plant>\n", argv[0], argv[0]);
        return 1;
    }
    const char *dir = argv[1];
    const char *cmd  = argv[2];
    
    if (strcmp(cmd,"histo")==0) {
        if (argc != 4) return 1;
        histo(dir, argv[3]);
    }
    else if (strcmp(cmd,"leaks")==0) {
        if (argc != 4) return 1;
        return run_leaks(dir, argv[3]);
    }
    return 0;
}
