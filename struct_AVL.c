typedef struct AVL{
    int eq;
    struct AVL* fg;
    struct AVL* fd;
    char id[11];
    char name[20];
    float capacityMax;
    float volCaptation;
    float volProcessed;
}AVL;
