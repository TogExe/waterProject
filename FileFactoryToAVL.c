#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Avl* FileFactoryToAvl(const char* nameFile){
    FILE* f = fopen(nameFile, "r");
    if (f == NULL){
        printf("Error : impossible to open %s\n", nameFile);
        return NULL;
    }
    char* token;
    char name[20];
    char id[11];
    float capacityMax;
    AVL* head = NULL;
    char ligne[256];
    
    if(fgets(ligne, sizeof(ligne), f) == NULL){
        fclose(f);
        return NULL;
    };
    
    rewind(f);
    
    while(fgets(ligne, sizeof(ligne), f)){

        ligne[strcspn(ligne, "\n")] = '\0';

        token = strtok(ligne, ";");
        strcpy(name, token);

        token = strtok(NULL, ";");
        strcpy(id, token);

        token = strtok(NULL, ";");
        capacityMax = atof(token);

        head = insererAVL(head, name, id, capacityMax, 0.0, 0.0);
}
