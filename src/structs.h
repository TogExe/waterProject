#ifndef STRUCTS_H
#define STRUCTS_H




typedef char Type;

#define UNIT     'U'
#define MODULE   'M'
#define COMPLEX  'C'
#define PLANT    'P'
#define UNKNOWN  '?'


typedef enum { EQUAL, INF, SUP } Comp;

typedef struct node node;
typedef struct node {
    void * data;
    node * next;
}Node;

typedef struct nodeGroup{
    Node * head;
}Ng;

#define idln 20

typedef struct indNode indNode;
typedef struct indNode{
    indNode * connect[3];
    void * content;
    char * id;
    int eq;
}IndNode;

typedef struct togindex{
    IndNode * tree;
}TogIndex;

typedef struct park park;
typedef struct park{
    char id[11];
    float capacity;
    float received;
    float lost;

    Ng storages;
    Ng sources;

    Type type;
}Park;

typedef struct source {
    char id[11];
    float * vol;
    float * loss;
    Type type;
    char * send_to_id;
}source;

typedef struct junction {
    char id[11];
    float * loss;
    char * sendingto;
}junction;

typedef struct service {
    char id[11];
    float * loss;
    char * cust_id;
}service;

typedef struct cust {
    char id[11];
    float * loss;
}Cust;

void printfcs(IndNode * ind);

#endif // STRUCTS_H
