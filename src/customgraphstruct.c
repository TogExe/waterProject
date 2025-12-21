#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "customgraphstruct.h"
#include "plantline.h"
#include "structs.h"

// Static helper functions
static int max(int a, int b) { return a > b ? a : b; }
static int min(int a, int b) { return a < b ? a : b; }

static int max3(int a, int b, int c) { return max(a, max(b, c)); }
static int min3(int a, int b, int c) { return min(a, min(b, c)); }

static Comp idcmp(const char id1[11], const char id2[11]) {
  int res = strcmp(id1, id2);
  if (res < 0) return INF;
  if (res > 0) return SUP;
  return EQUAL;
}

IndNode* rotationLeft(IndNode* a) {
  if (!a || !a->connect[2]) return a;

  IndNode* p = a->connect[2];
  int eq_a = a->eq;
  int eq_p = p->eq;

  a->connect[2] = p->connect[0];
  p->connect[0] = a;

  // Update balance factors algebraically
  a->eq = eq_a - max(eq_p, 0) - 1;
  p->eq = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);

  return p;
}

IndNode* rotationRight(IndNode* a) {
  if (!a || !a->connect[0]) return a;

  IndNode* p = a->connect[0];
  int eq_a = a->eq;
  int eq_p = p->eq;

  a->connect[0] = p->connect[2];
  p->connect[2] = a;

  // Update balance factors algebraically
  a->eq = eq_a - min(eq_p, 0) + 1;
  p->eq = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);

  return p;
}

IndNode* doubleRotationLeft(IndNode* a) {
  if (!a) return NULL;
  a->connect[2] = rotationRight(a->connect[2]);
  return rotationLeft(a);
}

IndNode* doubleRotationRight(IndNode* a) {
  if (!a) return NULL;
  a->connect[0] = rotationLeft(a->connect[0]);
  return rotationRight(a);
}

/* --- Balancing and Insertion --- */

IndNode* equilibateAVL(IndNode* a) {
  if (!a) return NULL;

  if (a->eq >= 2) {
    // Right-heavy
    if (a->connect[2] && a->connect[2]->eq >= 0) return rotationLeft(a);
    return doubleRotationLeft(a);
  }

  if (a->eq <= -2) {
    // Left-heavy
    if (a->connect[0] && a->connect[0]->eq <= 0) return rotationRight(a);
    return doubleRotationRight(a);
  }

  return a;
}

static IndNode* find_IndNode(IndNode* f, char id[11]) {
  if (!f) return NULL;
  switch (idcmp(id, f->id)) {
    case EQUAL:
      return f;
    case SUP:
      return find_IndNode(f->connect[2], id);
    case INF:
      return find_IndNode(f->connect[0], id);
    default:
      return NULL;
  }
}

static IndNode* recursiveAddNode(IndNode* f, char id[11], int* h_change,
                                 void* element) {
  if (!f) {
    IndNode* n = (IndNode*)malloc(sizeof(IndNode));
    if (!n) return NULL;

    n->id = malloc(11);
    if (!n->id) {
      free(n);
      return NULL;
    }
    strncpy(n->id, id, 10);
    n->id[10] = '\0';
    n->connect[1] = element;
    n->eq = 0;
    n->connect[0] = n->connect[2] = NULL;
    *h_change = 1;
    return n;
  }

  Comp d = idcmp(id, f->id);
  if (d == EQUAL) {
    printf("Warning: ID %s already exists in tree.\n", id);
    *h_change = 0;
    return f;
  }

  if (d == INF) {
    f->connect[0] = recursiveAddNode(f->connect[0], id, h_change, element);
    if (*h_change) {
      f->eq--;
      if (f->eq == 0)
        *h_change = 0;
      else if (f->eq == -2) {
        f = equilibateAVL(f);
        *h_change = 0;
      }
    }
  } else {
    f->connect[2] = recursiveAddNode(f->connect[2], id, h_change, element);
    if (*h_change) {
      f->eq++;
      if (f->eq == 0)
        *h_change = 0;
      else if (f->eq == 2) {
        f = equilibateAVL(f);
        *h_change = 0;
      }
    }
  }
  return f;
}

IndNode* AddNode(IndNode* f, char id[11], void* element) {
  int h_change = 0;
  return recursiveAddNode(f, id, &h_change, element);
}

/* --- Memory Cleanup --- */

void FreeAVL(IndNode* a) {
  if (!a) return;
  FreeAVL(a->connect[0]);
  FreeAVL(a->connect[2]);
  if (a->id) free(a->id);
  free(a);
}

/* --- Displays --- */

static char* get_type_text(char type) {
  switch (type) {
    case 'U': return "Unit ";
    case 'M': return "Module ";
    case 'C': return "Facility complex ";
    case 'P': return "Plant ";
    default: return "MissigNo? ";
  }
}

void printfcs(IndNode* ind) {
  if (ind) {
    printfcs(ind->connect[0]);
    Park* p = (Park*)(ind->connect[1]);
    printf("%s#%s;%f;%f;%f;%.0f percent loss\n", get_type_text(p->type), p->id,
           p->capacity, p->received, p->lost,
           (p->received > 0) ? (p->lost / p->received * 100) : 0);
    printfcs(ind->connect[2]);
  }
}

void printRealVolume(IndNode* ind) {
  if (ind) {
    printRealVolume(ind->connect[0]);
    Park* p = (Park*)(ind->connect[1]);
    printf("%s#%s;%f;%f\n", get_type_text(p->type), p->id, p->capacity/1000.0,
           (p->received - p->lost)/1000.0);
    printRealVolume(ind->connect[2]);
  }
}

void printVolume(IndNode* ind) {
  if (ind) {
    printRealVolume(ind->connect[0]);
    Park* p = (Park*)(ind->connect[1]);
    printf("%s#%s;%f;%f\n", get_type_text(p->type), p->id, p->capacity/1000.0,
           p->received/1000.0);
    printRealVolume(ind->connect[2]);
  }
}


void printMaxCapa(IndNode* ind) {
  if (ind) {
    printMaxCapa(ind->connect[0]);
    Park* p = (Park*)(ind->connect[1]);
    printf("%s#%s;%f\n", get_type_text(p->type), p->id, p->capacity/1000.0);
    printMaxCapa(ind->connect[2]);
  }
}

/* --- Improved procedures --- */

void tog_index_add(TogIndex* ind, char id[11], void* element) {
  int h_change = 0;
  ind->tree = recursiveAddNode(ind->tree, id, &h_change, element);
}

IndNode* tog_index_find_IndNode(TogIndex* ind, char id[11]) {
  return find_IndNode(ind->tree, id);
}

/* --- NEW: index size helpers (Option B support) --- */

static int count_nodes(IndNode* node)
{
  if (!node) return 0;
  return 1
       + count_nodes(node->connect[0])
       + count_nodes(node->connect[2]);
}

int tog_index_count(const TogIndex* ind)
{
  if (!ind) return 0;
  return count_nodes(ind->tree);
}

int divide_number(Ng*ng){
	Node * current = ng -> head;
	int count = 0;
	while(current){
		count++;
		current = current->next;
	}
	return count;
}

// customgraphstruct.c
void groupProject(Ng *ng, float pc, float myvol, float *globalvol) {
    // 1. CALCULATE LOSS FIRST
    // We do this immediately because even "Leaf" nodes (like Customers) 
    // have a loss percentage (pc) that must be counted.
    float rate = pc / 100.0f;
    float lost_here = myvol * rate;
    *globalvol += lost_here;

    // 2. NOW CHECK FOR CHILDREN
    // If there are no children (e.g., it's a Customer), we stop recursion here.
    // Crucially, we have already added 'lost_here' to 'globalvol' above.
    if (!ng || !ng->head) return;

    // 3. Distribute the remaining volume to the next layer
    float remaining_vol = myvol - lost_here;

    int branches = divide_number(ng);
    if (branches <= 0) return;

    float vol_per_branch = remaining_vol / (float)branches;

    Node *current = ng->head;
    while (current) {
        Entity *worker = (Entity *)current->data;
        if (worker) {
            // Recurse: Pass the child's own loss value as 'pc' for the next call
            groupProject(&worker->ng, worker->loss, vol_per_branch, globalvol);
        }
        current = current->next;
    }
}

void addnode(Ng * ng,void * data){
	Node * current = ng -> head;
	if (!current){
		ng -> head = malloc(sizeof(Node));
		ng -> head -> next = NULL;
		ng -> head -> data = data;
		return;
	}else{
		while(current->next){
			current = current->next;
		}
		current -> next =malloc(sizeof(Node));
		current -> next -> data = data;
		current -> next -> next = NULL;
	}
}
