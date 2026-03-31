#include <stdio.h>
#include <stdlib.h>

#define N 35

// Graph Definitions

typedef struct Node {
    int vertex;
    double weight;
    struct Node* next;
} Node;

typedef struct {
    int numVertices;
    Node** adjList;         
    double m2;         
} Graph;

// Graph Functions

Graph* createGraph(int n) {
    Graph* g = malloc(sizeof(Graph));
    g->numVertices = n;
    g->adjList = calloc(n, sizeof(Node*));
    g->m2 = 0;  
    return g;
}

void addEdge(Graph* graph, int u, int v, double w) {
    Node* a = malloc(sizeof(Node));
    a->vertex = v; a->weight = w; a->next = graph->adjList[u];
    graph->adjList[u] = a;

    Node* b = malloc(sizeof(Node));
    b->vertex = u; b->weight = w; b->next = graph->adjList[v];
    graph->adjList[v] = b;

    graph->m2 += 2*w;
}

double communityDegree(Graph* graph, int* comm, int c) {
    double sum = 0;
    for (int i = 1; i < graph->numVertices; i++) {
        if (comm[i] == c) {
            Node* p = graph->adjList[i];
            while (p) {
                sum += p->weight;
                p = p->next;
            }
        }
    }
    return sum;
}

double edgesBetween(Graph* graph, int* comm, int A, int B) {
    double e = 0;
    for (int i = 1; i < graph->numVertices; i++) {
        if (comm[i] != A) continue;
        Node* p = graph->adjList[i];
        while (p) {
            if (comm[p->vertex] == B)
                e += p->weight;
            p = p->next;
        }
    }
    return e;
}

Graph* readInput(){
    FILE* input = fopen("data.txt", "r");

    int inp1 = 0;
    int inp2 = 0;

    Graph* graph = createGraph(N);
    
    while (fscanf(input, "[%d %d]\n", &inp1, &inp2) == 2) {
        addEdge(graph, inp1, inp2, 1);
    }

    return graph;
}

void freeGraph(Graph* graph){
    for(int i = 0; i < graph->numVertices; i++){
        Node* p = graph->adjList[i];
        while(p){
            Node* temp = p;
            p = p->next;
            free(temp);
        }
    }

    free(graph->adjList);
    free(graph);
    return;
}

int reIndexCommunity(int* community){
    int map[N];
    for (int i = 0; i < N; i++) {
        map[i] = -1;
    }

    int newID = 1;
    for (int i = 1; i < N; i++) {
        if (map[community[i]] == -1){
            map[community[i]] = newID++;
        }
    }

    for(int i = 1; i < N; i++){
        community[i] = map[community[i]];
    }

    return newID;
}

// Fast Greedy Functions
double totalGraphWeight(Graph *graph)
{
    double sum = 0;
    for (int i = 1; i < graph->numVertices; i++)
    {
        Node *temp = graph->adjList[i];
        while (temp != NULL)
        {
            sum += temp->weight;
            temp = temp->next;
        }
    }
    return sum / 2.0;
}

int nodeDegree(Graph *graph, int v)
{
    int sum = 0;
    Node *temp = graph->adjList[v];
    while (temp != NULL)
    {
        sum += temp->weight;
        temp = temp->next;
    }
    return sum;
}

int CalcAij(Graph *graph, int vertex1, int vertex2)
{
    Node *temp = graph->adjList[vertex1];
    while (temp != NULL)
    {
        if (temp->vertex == vertex2){
            return temp->weight;
        }
        temp = temp->next;
    }
    return 0;
}

double modularity(Graph *graph, int *community)
{
    double Q = 0.0;
    double m = totalGraphWeight(graph); 

    for (int i = 1; i < graph->numVertices; i++)
    {
        for (int j = 1; j < graph->numVertices; j++)
        {
            if (community[i] == community[j])
            {
                double Aij = (double) CalcAij(graph, i, j);
                double ki = (double) nodeDegree(graph, i);
                double kj = (double) nodeDegree(graph, j);

                double addon = Aij - ((ki * kj) / (2.0 * m));
                Q += addon;
            }
        }
    }

    Q /= (2.0 * m);
    return Q;
}

void printOutput(Graph* graph, int* community, int communityCount){
    printf("Community count: %i\n", communityCount-1);
    for(int i = 1; i < communityCount; i++) {
        printf("Community %i: ", i);
        int printCount = 0;
        for(int j = 1; j < N; j++){
            if(community[j] == i){
                if(printCount == 0){
                    printf("%i", j);
                    printCount++;
                } else {
                    printf(", %i", j);
                }
            }
        }
        printf("\n");
    }
    printf("Modularity: %lf", modularity(graph, community));
}

// Fast Greedy
void fastGreedy(Graph* graph, int* comm) {
    // Initialize community array
    int n = graph->numVertices;

    for (int i = 1; i < n; i++) {
        comm[i] = i;
    }

    // Run until no change
    int changed = 1;
    while (changed) {
        changed = 0;

        double bestGain = 0;
        int bestA = -1, bestB = -1;

        // checking for gain loop, find best gain
        for (int A = 1; A < n; A++) {
            for (int B = A+1; B < n; B++) {

                double eAB = edgesBetween(graph, comm, A, B);
                if (eAB == 0) continue;

                double kA = communityDegree(graph, comm, A);
                double kB = communityDegree(graph, comm, B);

                double dQ = (eAB / graph->m2) - (kA * kB) / (graph->m2 * graph->m2);

                if (dQ > bestGain) {
                    bestGain = dQ;
                    bestA = A;
                    bestB = B;
                }
            }
        }

        // Change community to best gain
        if (bestGain > 1e-12) {
            for (int i = 1; i < n; i++) {
                if (comm[i] == bestB)
                    comm[i] = bestA;
            }
            changed = 1;
        }
    }
}

int main() {     
    Graph* graph = readInput();

    int* comm = malloc(N * sizeof(int));

    fastGreedy(graph, comm);

    int len = reIndexCommunity(comm);

    printOutput(graph, comm, len);

    freeGraph(graph);
    free(comm);
    return 0;
}
