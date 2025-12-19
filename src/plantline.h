#ifndef PLANTLINE_H
#define PLANTLINE_H

#include "structs.h"
#include "customgraphstruct.h"

// Function declarations
void line_to_fac_idx_(const char *l, void *data);
void source_to_plant_cb(const char *l, void *data);

#endif // PLANTLINE_H