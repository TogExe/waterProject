#include "customgraphstruct.h"
#include "mmap_reader.h"
#include "plantline.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


int histo(const char *dir,const char * mode) {
	TogIndex plant_index;
	plant_index.tree = NULL; // Initialize the tree pointer to NULL
	// Allocate enough space for the full path
	char plants_path[256];
	char sources_path[256];
	
	// Construct the full paths
	snprintf(plants_path, sizeof(plants_path), "%s/plants.dat", dir);
	snprintf(sources_path, sizeof(sources_path), "%s/sources_to_plants.dat", dir);
	
	// Use the full paths in your functions
	for_each_line(plants_path, line_to_fac_idx_, (void*)&plant_index);
	for_each_line(sources_path, source_to_plant_cb, (void*)&plant_index);
	//printfcs(plant_index.tree);
	if (!strcmp(mode,"max")){
		printMaxCapa(plant_index.tree);
	}else if (!strcmp(mode,"vol")){
		printRealVolume(plant_index.tree);
	}
	
	// Clean up the tree to prevent memory leaks
	FreeAVL(plant_index.tree);
	return 0;
}


int run_leaks(const char *dir,const char * id){
	return 0;
}


int main(const int argc,char **argv){
	//TogIndex alt_index;
	if (argc < 3){
		fprintf(stderr, "Usage:\n %s <datadir> histo <type>\n %s <datadir> leaks <plant>\n", argv[0], argv[0]);
	}
	const char *dir = argv[1];
	const char *cmd  = argv[2];
	const char *mode = argv[3];
	
	struct stat st;
	if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
		fprintf(stderr, "Error: Directory '%s' not found or not accessible\n", dir);
	return 1;
	}
	if (strcmp(cmd,"histo")==0) {
	if (argc != 4) { fprintf(stderr,"Missing histogram type\n"); return 1; }
		histo(dir,mode);
	}
	else if (strcmp(cmd,"histo_data")==0) {
	if (argc != 4) { fprintf(stderr,"Missing histogram type\n"); return 1; }
	
	}
	else if (strcmp(cmd,"leaks")==0) {
	if (argc != 4) { fprintf(stderr,"Missing plant ID\n"); return 1; }
	return run_leaks(dir, argv[3]);
	}
	else {
	fprintf(stderr,"Unknown command: %s\n", cmd); return 1;
	}
	
	//for_each_line("plants.dat",line_to_fac_idx_,(void*)&plant_index);
}
