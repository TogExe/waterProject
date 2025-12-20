#include "customgraphstruct.h"
#include "structs.h"
#include <stdio.h>

int main()
{
	TogIndex ind;
	ind.tree = NULL;

	Park p1;
	snprintf(p1.id, sizeof(p1.id), "TEST001");
	p1.capacity = 100.0;
	p1.received = 0.0;
	p1.lost = 0.0;

	printf("Adding node...\n");
	tog_index_add(&ind, "TEST001", &p1);
	printf("Node added.\n");

	printf("Finding node...\n");
	IndNode *found = tog_index_find_IndNode(&ind, "TEST001");
	if (found) {
	printf("Found node with id: %s\n", found->id);
	Park *park = (Park*)found->connect[1];
	printf("Park capacity: %f\n", park->capacity);
	}
	else {
	printf("Node not found.\n");
	}

	return 0;
}