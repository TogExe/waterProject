#ifndef CUSTOMGRAPHSTRUCT_H
#define CUSTOMGRAPHSTRUCT_H

#include "structs.h"
#include <stdio.h>

// Function declarations (Prototypes only)
IndNode* rotationLeft(IndNode* a);
IndNode* rotationRight(IndNode* a);
IndNode* doubleRotationLeft(IndNode* a);
IndNode* doubleRotationRight(IndNode* a);
IndNode* equilibateAVL(IndNode* a);
IndNode* AddNode(IndNode* f, char id[11], void* element);
void FreeAVL(IndNode* a);
void printfcs(IndNode* ind);
void printMaxCapa(IndNode* ind);
void printRealVolume(IndNode* ind);
void printVolume(IndNode* ind);
void tog_index_add(TogIndex* ind, char id[11], void* element);
IndNode* tog_index_find_IndNode(TogIndex* ind, char id[11]);
int tog_index_count(const TogIndex* ind);
void addnode(Ng * ng,void * data);
void groupProject(Ng * ng,float pc,float myvol,float*globalvol);

#endif // CUSTOMGRAPHSTRUCT_H
