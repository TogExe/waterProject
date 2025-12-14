AVL* FileToAVL(const char* nameFile){
    FILE* f = fopen(nameFile, "r");
    if (f == NULL){
        printf("Error : impossible to open %s\n", nameFile);
        return NULL;
    }
    char* token;
    char* token2;
    char name[20];
    char id[11];
    float capacityMax;
    AVL* head = NULL;
    char ligne[256];
    char ligne_test[256];
    char* test;
    int count = 0;
    if(fgets(ligne, sizeof(ligne), f) == NULL){
        fclose(f);
        return NULL;
    };
    strcpy(ligne_test,ligne);
    test = strtok(ligne_test, ";");
    while(test != NULL){
        count++;
        test = strtok(NULL, ";");
    }
    if(count == 3){
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
    fclose(f);
    return head;
    
    }else if(count == 4){


        if(strchr(ligne, '*') == NULL){
            rewind(f);
            while(fgets(ligne, sizeof(ligne), f)){
        
            float sum = 0;
            char temp_colonne[256];

            ligne[strcspn(ligne, "\n")] = '\0';

            token = strtok(ligne, ";");
            strcpy(name, token);

            token = strtok(NULL, ";");
            strcpy(id, token);

            token = strtok(NULL, ";");
            capacityMax = atof(token);

            token = strtok(NULL, ";");
            strcpy(temp_colonne, token);
            token2 = strtok(temp_colonne, "-");
            while(token2 != NULL){
                sum += atof(token2);
                token2 = strtok(NULL, "-");
            }
            head = insererAVL(head, name, id, capacityMax, sum, 0.0);
    }
    fclose(f);
    return head;

        }else{
            rewind(f);
            float volume,percent;
            while(fgets(ligne, sizeof(ligne), f)){
            
            float sum = 0;
            char temp_colonne[256];

            ligne[strcspn(ligne, "\n")] = '\0';

            token = strtok(ligne, ";");
            strcpy(name, token);

            token = strtok(NULL, ";");
            strcpy(id, token);

            token = strtok(NULL, ";");
            capacityMax = atof(token);

            token = strtok(NULL, ";");
            strcpy(temp_colonne, token);
            token2 = strtok(temp_colonne, "-");
            while(token2 != NULL){
                sscanf(token2,"%f*%f",&volume, &percent);
                sum += volume*percent;
                token2 = strtok(NULL, "-");
            }
            head = insererAVL(head, name, id, capacityMax, 0.0, sum);
    }
    fclose(f);
    return head;
        }

    }
}
