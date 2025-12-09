#include "multiThreaded.h"

// Essential for a thread to execute a scheduled task

void * doallTasks(void * arg){
	//printf("void * doallTasks(void * arg)\n");
	NodeGroup * schedule = (NodeGroup*)arg;
	if (!schedule->head)return NULL;
	Node *current = schedule->head;
	Node *temp =NULL;
	while(current){
		if (current->content){
			((Task*)current->content)->task(((Task*)current->content)->data);
		}
		temp = current;
		schedule -> head = current->next;
		current = current->next;
		free(temp);
	}
	return NULL;
}

// Thread creation and handling
void initNodeGroup(NodeGroup *ng)
{
	ng->head = malloc(sizeof(Node));
	ng->head -> content = NULL;
	ng->head -> next    = NULL;
}
Threads * setupThreads()
{
	//printf("setting up threads\n");
	Threads * newThreads = malloc(sizeof(Threads));
	newThreads -> doall =  doallTasks;
	
	for (int i =0;i<maxthreads;i++){
		newThreads->occupency[i]=0;
		// Setting up the first node;
		newThreads->scheduledTasks[i].head= malloc(sizeof(Node));
		newThreads->scheduledTasks[i].head->content = NULL;
		newThreads->scheduledTasks[i].head->next = NULL;
	}
	//printf("sucess\n");
	return newThreads;
}

void addTaskToGroup(NodeGroup g,Task * task)
{
	//printf("adding tasks to group\n");
	//if(!g)return;
	Node *current =g.head;
	while (current->next){
		current = current->next;
	}
	Node *new=malloc(sizeof(Node));
	new-> next =NULL;
	new->content = (void*)task;
	current-> next = new;
	return;
}


void addTaskInThreads(Threads * t, void (*task)(void*param),void*data)
{
	//printf("adding tasks to group\n");
	if(!t)return;
	int min = *t->occupency;
	int slot = 0;
	for	(int i = 0; i<maxthreads;i++){
		if (t->occupency[i]<min){
			min = t->occupency[i];
			slot = i;
		}
	}
	Task * ntsk = malloc(sizeof(Task));
	ntsk -> task = task;
	ntsk -> data = data;
	addTaskToGroup(t->scheduledTasks[slot],ntsk);
	t->occupency[slot]++;
}

// To execute the treads scheduled for a thread

void handleThreads(Threads* t)
{
	//printf("Handling threads\n");
	if(!t)return;
	for (int i =0; i<maxthreads;i++){
		//void * data = (void*)&t->scheduledTasks[i];
		pthread_create(&t->threads[i],NULL,t->doall,(void*)&t->scheduledTasks[i]);
	}
	for (int i = 0 ; i<maxthreads;i++){
		pthread_join(t->threads[i],NULL);
	}
}

// Exemple of tasks
void addContent(NodeGroup * ng,void*content){
	Node * current = (Node*)ng -> head;
	while(current->next){
		current = current->next;
	}
	current-> next = malloc(sizeof(Node));
	current-> next-> content = content;
}
