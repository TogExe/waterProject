// This is just some refractored code i'm working on to make the old avl files work the way they are supposed to
//  	the other goal is to make the code slightly more coherent (using only one language)
//		and finally not much needs to be done in the c programm with what i'm seing i think i will be able to have 
//		a fully working and safe programm.
// 		ill clean up in different files later btw
// 		Note : If you want to enjoy coding and code in a way you find esthetic you should : just code instead of always asking ai to do stuff for you. the result show in less than 5 hours.
#include <stdio.h>
#include <stdlib.h>

//unused
typedef struct {
	void * content;
	void * connect;
}Node;

// That shi looks good
typedef enum {
	EQUAL,
	SUP,
	INF
}Comp;

//	Our simple avl structure
typedef struct {
	int eq;
	char id[11];
	char name[20];
	float maxCapacity;
	float volCaptation;
	// i use tribranch so i felt like this was more comfy
	void * connect[3]; 
}Avl;

//	Very basic comparing features 

__attribute__((const)) Comp idcmp(char id[11],char id2[11]){
	for (int i=0;i<11;i++){
		if (abs(id[i]-id2[i])){
			if (id[i]<id2[i])return INF;
			else return SUP;
		}
	}
	return EQUAL;
}
__attribute__((const)) int max(int a, int b){
	return (a>b)?a:b;
}
__attribute__((const)) int min(int a, int b){
	return (a>b)?b:a;
}
__attribute__((const)) int max3(int a, int b, int c){
	return (a>b)? ((a>c)?a:c):((b>c)?b:c); 
}
__attribute__((const)) int min3(int a, int b, int c){
	return (b>a)? ((c>a)?a:c):((c>b)?b:c); 
}

//	Handling our avl

Avl * AddNode(Avl * facility,char id[11],char name[20])
{
	Avl * nf = malloc(sizeof(Avl));

	for (int i =0;i<20;i++){
		if (i<11)nf -> id[i] =id[i];
		nf -> name[i]=name[i];
	}
	for (int i=0;i< 3; i++)
		nf -> connect[i]=NULL;
	
	if (!facility)return nf;

	// Comparing some shi.
	else {
		Comp diff = idcmp(facility->id,nf-> id);
		if (diff==EQUAL){
			free(nf);
			printf("I think this alredy exists");
		}else if (diff == SUP){
			facility -> connect[0]=nf;
			facility -> eq++;
		}else if (diff == INF){
			facility -> connect[2]=nf;
			facility -> eq--;
		}
	}
	return facility;
}
// basically rotations and shi
Avl* rotationLeft(Avl* a)
{
    Avl* pivot;
    int eq_a, eq_p;
    pivot = a->connect[2];
    a->connect[2] = pivot->connect[0];
    pivot->connect[0] = a;
    eq_a = a->eq;
    eq_p = pivot->eq;
    a->eq = eq_a - max(eq_p, 0) - 1;
    pivot->eq = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);
    a = pivot;
    return a;
}

Avl* rotationRight(Avl* a)
{
    Avl* pivot;
    int eq_a, eq_p;
    pivot = a->connect[0];
    a->connect[0] = pivot->connect[2];
    pivot->connect[2] = a;
    eq_a = a->eq;
    eq_p = pivot->eq;
    a->eq = eq_a - min(eq_p, 0) + 1;
    pivot->eq = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);
    a = pivot;
    return a;
}

Avl* doubleRotationLeft(Avl* a){
    a->connect[2] = rotationRight(a->connect[2]);
    return rotationLeft(a);
}

Avl* doubleRotationRight(Avl* a){
    a->connect[0] = rotationLeft(a->connect[0]);
    return rotationRight(a);
}

// and that is usefull too (necessary)
Avl* equilibateAVL(Avl* a) {
    if (a == NULL){
        return a;
    }
    if (a->eq >= 2) {
        if (a->connect[2] && ((Avl*)a->connect[2])->eq >= 0) {
            return rotationLeft(a);
        } else {
            return doubleRotationRight(a);
        }
    }
    else if (a->eq <= -2) {
        if ((Avl*)a->connect[0] && ((Avl*)a->connect[0])->eq <= 0) {
            return rotationRight(a);
        } else {
            return doubleRotationLeft(a);
        }
    }
    return a;
}
