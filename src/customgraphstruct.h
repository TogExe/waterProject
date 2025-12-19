#ifndef CUSTOMGRAPHSTRUCT_H
#define CUSTOMGRAPHSTRUCT_H

#include "structs.h"
#include <stdio.h>

// Function declarations
IndNode *rotationLeft(IndNode *a);
IndNode *rotationRight(IndNode *a);
IndNode *doubleRotationLeft(IndNode *a);
IndNode *doubleRotationRight(IndNode *a);
IndNode *equilibateAVL(IndNode *a);
IndNode *AddNode(IndNode *f, char id[11], void *element);
void FreeAVL(IndNode *a);
void prinfcs(IndNode *ind);
void printinafilecapa(IndNode * ind, FILE * f);
void tog_index_add(TogIndex *ind, char id[11], void *element);
IndNode *tog_index_find_IndNode(TogIndex *ind, char id[11]);

#endif // CUSTOMGRAPHSTRUCT_H
