int max(int a, int b){
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int min(int a, int b){
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
int min3(int a, int b, int c) {
    return min(min(a, b), c);
}

int max3(int a, int b, int c) {
    return max(max(a, b), c);
}

AVL* rotationLeft(AVL* a){
    AVL* pivot;
    int eq_a, eq_p;
    pivot = a->fd;
    a->fd = pivot->fg;
    pivot->fg = a;
    eq_a = a->eq;
    eq_p = pivot->eq;
    a->eq = eq_a - max(eq_p, 0) - 1;
    pivot->eq = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);
    a = pivot;
    return a;
}

AVL* rotationRight(AVL* a) {
    AVL* pivot;
    int eq_a, eq_p;
    pivot = a->fg;
    a->fg = pivot->fd;
    pivot->fd = a;
    eq_a = a->eq;
    eq_p = pivot->eq;
    a->eq = eq_a - min(eq_p, 0) + 1;
    pivot->eq = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);
    a = pivot;
    return a;
}

AVL* doubleRotationLeft(AVL* a){
    a->fd = rotationRight(a->fd);
    return rotationLeft(a);
}

AVL* doubleRotationRight(AVL* a){
    a->fg = rotationLeft(a->fg);
    return rotationRight(a);
}

AVL* equilibrerAVL(AVL* a) {
    if (a == NULL){
        return a;
    }
    if (a->eq >= 2) {
        if (a->fd && a->fd->eq >= 0) {
            return rotationGauche(a);
        } else {
            return doubleRotationGauche(a);
        }
    }
    else if (a->eq <= -2) {
        if (a->fg && a->fg->eq <= 0) {
            return rotationDroite(a);
        } else {
            return doubleRotationDroite(a);
        }
    }
    return a;
}

AVL* insererAVL(AVL* a, AVL* e, int* h, const char* name, const char* id, float capacityMax, float volCaptation, float volProcessed) {
    if (a == NULL){
        *h = 1;
        return creationAVL(name, id, capacityMax, volCaptation, volProcessed);
    }

    if(strcmp(e->id, a->id) < 0){
        a->fd = insererAVL(a->fd, e, h);
        *h = -(*h);
    }else if (strcmp(e->id, a->id) > 0){
        a->fg = insererAVL(a->fg, e, h);
    }else{
        *h = 0;
        return a;
    }

    if(*h != 0){
        a->eq = a->eq + *h;
        a = equilibrerAVL(a);
        if(a->eq == 0){
            *h = 0;
        }else{
            *h = 1;
        }
    }

    return a;
}
