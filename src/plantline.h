#ifndef PLANTLINE_H
#define PLANTLINE_H

//#include "structs.h"
#include "customgraphstruct.h"

typedef struct entity{
	char id[15];
	Ng ng;
	float loss;
	char connect[15];
}Entity;

typedef struct c_context{
	TogIndex in;
	TogIndex * up;
}Context;

/* --- Data Loading Callbacks --- */
void line_to_fac_idx_(const char *l, void *data);
void source_to_plant_cb(const char *l, void *data);
void con_plan_sto(const char *l, void *data);
void con_sto_jun(const char *l, void *data);
void con_jun_ser(const char *l, void *data);
void con_ser_cus(const char *l, void *data);

//likning shit
void storage_handler(const char *l, void *data);
void entity_handler(const char *l, void *data);

/* --- Calculation Logic --- */
/**
 * Calculates total consumption by summing demand from all Customers 
 * nested deep within the Park's hierarchy.
 */
void push_flow_leak_cb(const char *l, void *data);
double calculate_park_consumption(Park *p);

#endif // PLANTLINE_H
