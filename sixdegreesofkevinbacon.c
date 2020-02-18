#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define LoadF 0.6
#define R 2
#define TRUE  (1==1)
#define FALSE (!TRUE)



typedef struct node {
    int  id;
    char *name;
    struct node **edges;
    int  visited;
    struct node *parent;
    int numberOfEdges;
} node;

typedef struct hashTable {
    int tableSize;
    int nodeCount;
    node **table;
    int *flags;
} hashTable;

typedef struct graphs{
	hashTable  *graph;
}graphs;

typedef struct Queue{
	node **queue;
	int size;
}Queue;







long int hash(long int key, int i, int M);
long int hash1(long int key, int M);
long int hash2(long int key, int M);
int isPrime(int n);
int nextPrime(int N);
int lenghtOfHashTable(int numberOfElements);
long int keyFromWord(char* wordR);
long int calculateKey(char *line);

hashTable *addToTable(hashTable *table, node *newNode);
hashTable *reHash(hashTable *table);

int isSame(node *first, node *second);

graphs *fromFileToHash(hashTable *table);
node* getNode(hashTable *table,node* actNode);
void addEdge(node* movie, node *act);
void Add(Queue *list, node* add);
node* Pop(Queue *list);
int findDistance(hashTable *table,char *f_act, char *s_act);




int main(int argc, char **argv)
{
	char f_act[96];
	char f_act_f[30];
	char f_act_l[30];
	char s_act[96];
	char s_act_f[30];
	char s_act_l[30];
	
	int dist=0;
	
	hashTable *table = (hashTable*)malloc(sizeof(hashTable));
	
	table->tableSize = 1;
	table->nodeCount = 0;
	table->table = (node**)malloc(sizeof(node*)*table->tableSize);
	table->flags = (int*)calloc(table->tableSize,sizeof(int));
	
	graphs *bipartiteGraph = fromFileToHash(table);
	
	
	printf("\nWelcome to the game.. ");
	printf("\nGive me two actor/actress and i will give the distance between them");
	
	while(1){
		printf("\n");
		printf("\n first actor/actress: ");
		scanf("%s %s",f_act_f, f_act_l);
		printf("\n second actor/actress: ");
		scanf("%s %s",s_act_f, s_act_l);
		printf("\n");
		
		strcpy(f_act,f_act_l);
		strcat(f_act,", ");;
		strcat(f_act, f_act_f);

		
		strcpy(s_act,s_act_l);
		strcat(s_act,", ");
		strcat(s_act, s_act_f);
		
		
		dist = findDistance(bipartiteGraph->graph, f_act, s_act);
		printf("\n Distance between %s , %s is %d",f_act,s_act,dist);
	}
	
	
	return 0;
}













long int hash(long int key, int i, int M){
	return ((hash1(key, M) + i*(hash2(key, M))) % M);
	}
long int hash1(long int key, int M){
	return (key % M); 
	}
long int hash2(long int key, int M){
	int MM = M-1;
	return 1+(key % MM);
	}	
int isPrime(int n){  
	int i = 0;
    if (n <= 1)  return 0;  
    if (n <= 3)  return 1;  
    if (n%2 == 0 || n%3 == 0) return 0;      
    for (i=5; i*i<=n; i=i+6)  
        if (n%i == 0 || n%(i+2) == 0)  
           return 0;  
    return 1;  
	}  
int nextPrime(int N){ 
    if (N <= 1) 
        return 2;   
    int prime = N; 
    int found = 0; 
    while (!found) { 
        prime++;   
        if (isPrime(prime)) 
            found = 1; 
    } 
    return prime; 
	} 
int lenghtOfHashTable(int numberOfElements){
	return nextPrime((int)(numberOfElements/LoadF));
	}
long int keyFromWord(char* wordR){
	long int key = 0;
	int size = strlen(wordR);
	int i;
	for(i = size; i > 0; i--){
		key += (int)(wordR[size-i])*pow(R,i-1);
		}
	return key;
	}
long int calculateKey(char *line){
	long int key=0;
	char *token;
	char buffer[1024];
	
	strcpy(buffer,line);
	char *rest=buffer;
	while ((token = strtok_r(rest, " ", &rest))) {
	key += keyFromWord(token);
	}
	return key;
	}
hashTable *addToTable(hashTable *table, node *newNode){
	int i = 0;
	int same = 0, key = 0, place = 0;
	int full = 1;
	
	while(table->nodeCount+1 >= (int)(table->tableSize*LoadF)){
		
		table = reHash(table);
		}	
	if(newNode != NULL){

		key = calculateKey(newNode->name);
		while(full == 1){
			place = hash(key,i,table->tableSize);		
			if(table->flags[place] == 1){
				same = isSame(newNode,table->table[place]);
				if(same != 1){
					i++;
					}
				else{
					full = 0;
					}
				}
			else{
				full = 0;
				}
			}
		if(table->flags[place] != 1){				
			table->table[place] = newNode;

			table->flags[place] = 1;
			table->nodeCount += 1;
			newNode->id = place;
			}	
		}	
 	
	return table;
	}
	
	
	
hashTable *reHash(hashTable *table){
	hashTable *tmpTable = (hashTable*)malloc(sizeof(hashTable));
	int i=0;
	tmpTable->tableSize = lenghtOfHashTable((int)(table->tableSize/LoadF));
	tmpTable->nodeCount = 0;
	tmpTable->table = (node**)malloc(sizeof(node*)*tmpTable->tableSize);
	tmpTable->flags = (int*)calloc(tmpTable->tableSize,sizeof(int));
	for (i=0; i < table->tableSize; i++){
		if(table->flags[i] == 1){
			addToTable(tmpTable,table->table[i]);
			}
		}
	return tmpTable;
	}


int isSame(node *first, node *second){
	int same = 0;
	if(strcmp(first->name,second->name) == 0){
		same = 1;
		}
	return same;
	}

graphs *fromFileToHash(hashTable *table){
	FILE *fp = NULL;
	int movie = TRUE;
	graphs *bipartite = (graphs*)malloc(sizeof(graphs));
	char line[4048];
	char *token;
	char* rest;
	node *movieNode, *actNode;
	int i;
	int j;
	int n = 0;
	char path[50] = "input-mpaa.txt";
	char show = 'n';
	
	fp = fopen(path, "r");
	if (fp == NULL){
        printf ("Error opening the file\n\n'");
        exit(EXIT_FAILURE);
    	} else {
    		
    	printf("\n Reading file from : %s ", path);
    	printf("\n Do you want to see movie and actors being read? (y/n) :");
    	show = getchar();
    	
    	
		while( fgets (line, 1024, fp)!=NULL ) {
    		rest = line;
    		n++;
    		movie=TRUE;
    		while ((token = strtok_r(rest, "/", &rest))) {

				actNode = (node*)malloc(sizeof(node));
	    		if(strlen(token)<1024){
	    			actNode->name = (char*)calloc(strlen(token)+1,sizeof(char));
	    			
					strcpy(actNode->name,token);
					actNode->id = 0;
					actNode->edges = (node**)malloc(0);
					actNode->numberOfEdges = 0;
					
					actNode->parent = (node*)malloc(sizeof(node));
					
					if(movie){
						movieNode = actNode;
						movie=FALSE;
						}else{
						actNode = getNode(table,actNode);
						addEdge(movieNode,actNode);	
						}
						
					table = addToTable(table, actNode);
					}
				
				}	
				
				if(tolower(show) == 'y'){
				
					printf("\n");
					printf("\n------------------------------");
					printf("\n |%d|  %s",n,movieNode->name);
					printf("\n------------------------------");
					printf("\n");
					for(i=0;i< movieNode->numberOfEdges;i++){
						printf("\nedge[%d]: %s",i,movieNode->edges[i]->name);
							for(j=0; j<movieNode->edges[i]->numberOfEdges;j++){
								printf("\n\t-- %s",movieNode->edges[i]->edges[j]->name);
								}
						}
					}
				
		   }
		}
		
	j = 0;
	printf("\nConnections between movies and actor/actresses have been established.");
	bipartite->graph =  table;
	printf("\nGraph is ready.");

	fclose(fp);
	
	return bipartite;
}




node* getNode(hashTable *table,node* actNode){
	
	long key = calculateKey(actNode->name);
	int i = 0;
	int same = 0,  place = 0;
	int full = 1;
	while(full == 1){
	place = hash(key,i,table->tableSize);			
	if(table->flags[place] == 1){
		same = isSame(actNode,table->table[place]);
		if(same != 1){
			i++;
			}
		else{
			return table->table[place];
			
			}
		}
	else{
		return actNode;
		
		}
	}
	return actNode;
}


void addEdge(node *movie, node *act){

	
	movie->edges = realloc(movie->edges,(movie->numberOfEdges+1)*sizeof(node*));
	movie->edges[movie->numberOfEdges]= act;
	movie->numberOfEdges++;
	
	
	act->edges = realloc(act->edges,(act->numberOfEdges+1)*sizeof(node*));
	act->edges[act->numberOfEdges]= movie;
	act->numberOfEdges++;
	
}



node* Pop(Queue *list){
	int i=0;
	node* result;
	if(list->size>0){	
		result = list->queue[0];	
		if(list->size == 1){
			list->queue[0]=NULL; 
			}else{
				for(i = 1; i<list->size; i++){
					list->queue[i-1]=list->queue[i];
					}
				}
		list->size--;
		}else{
			result = NULL;
			}
	return result;
}

void Add(Queue *list, node* add){
	
	node **tmp = (node**)realloc(list->queue,(list->size+1)*(sizeof(node*)));

	if(tmp!=NULL){
		
		list->queue = tmp;
		
		list->queue[list->size]=add;
		
		list->size++;
		}else{
			printf("adding to queue is not succesful..");
			exit(0);
			}
}


int findDistance(hashTable *table,char *f_act, char *s_act){
	
	printf("\nDistance between %s - %s : ?", f_act, s_act);
	
	int dist=0;
	int i;
	int n = 0;
	node *start;
	node *finish;
	Queue *list = (Queue*)calloc(1,sizeof(Queue));
	clock_t first, second;
	double cpu_time_used;
	double timeout = (double)(200);
	node *tmp = (node*)calloc(1,sizeof(node));
	tmp->name = (char*)calloc(96,sizeof(char));
	strcpy(tmp->name,f_act);
	start = getNode(table,tmp);
	start->parent = NULL;
	if(start==tmp){
		printf("\n\n %s isn't in the graph", start->name);
		printf("\n Name format is 'NAME SURNAME'");
		return(0);
	}
	
	tmp->name = (char*)calloc(96,sizeof(char));
	strcpy(tmp->name,s_act);
	finish = getNode(table,tmp);
	if(finish==tmp){
		printf("\n\n %s isn't in the graph",finish->name);
		printf("\n Name format is 'NAME SURNAME'");
		return(0);
		}	
	
	printf("\n");
	printf("\nfindDistance: |%d| start: %s",start->id,start->name);
	printf("\nfindDistance: |%d| finish: %s",finish->id,finish->name);
	printf("\n");
	printf("\nSearching a path from %s to %s",start->name, finish->name);	
	printf("\n");
	
	tmp = start;
	first = clock();
	while (tmp != finish && (cpu_time_used < timeout)){
		tmp->visited = TRUE;
		for(i = 0;i < tmp->numberOfEdges;i++){
			if(tmp->edges[i]->visited != TRUE){

				Add(list,tmp->edges[i]);	
							
				tmp->edges[i]->parent = tmp;

				}
			}
		second = clock();
		cpu_time_used = ((double)(second-first)) / CLOCKS_PER_SEC;
		tmp = Pop(list);
	}
	
	n = 0;
	if((cpu_time_used >= timeout)){
		printf("\n timeout: %f seconds -- couln't find a connection between two actors/actresses", timeout);
		dist = -1;
		}else{
			while(tmp != NULL){
				printf("\n --- %s",tmp->name);
				printf("\n");
				tmp = tmp->parent;
				n++;
				getch();
				}
			dist = n/2;
			}

	
	
	return dist;
}
