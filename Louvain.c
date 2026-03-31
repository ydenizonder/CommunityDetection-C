#include <stdio.h>
#include <stdlib.h>

#define N 35

// Graph Definitions

typedef struct Node {
    int vertex;
    int weight;
    struct Node* next;
} Node;

typedef struct Graph {
    int numVertices;
    Node** adjLists;
} Graph;

// Graph Functions

Node* createNode(int v, int w) {
    Node* newNode = malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->weight = w;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(int vertices) {
    Graph* graph = malloc(sizeof(Graph));
    graph->numVertices = vertices;

    graph->adjLists = malloc(vertices * sizeof(Node*));

    for (int i = 0; i < vertices; i++) {
        graph->adjLists[i] = NULL;
    }

    return graph;
}

void freeGraph(Graph* graph){
    for(int i = 0; i < graph->numVertices; i++){
        Node* p = graph->adjLists[i];
        while(p){
            Node* temp = p;
            p = p->next;
            free(temp);
        }
    }

    free(graph->adjLists);
    free(graph);
    return;
}

void addEdge(Graph* graph, int src, int dest, int weight) {
    if(dest == src){
        Node* newNode = createNode(dest, weight);
        newNode->next = graph->adjLists[src];
        graph->adjLists[src] = newNode;
    } else {
        Node* newNode = createNode(dest, weight);
        newNode->next = graph->adjLists[src];
        graph->adjLists[src] = newNode;

        newNode = createNode(src, weight);
        newNode->next = graph->adjLists[dest];
        graph->adjLists[dest] = newNode;
    }
}

void printGraph(Graph* graph) {
    for (int v = 1; v < graph->numVertices; v++) {
        Node* temp = graph->adjLists[v];
        printf("Vertex %d: ", v);
        while (temp) {
            printf("%d (%d) -> ", temp->vertex, temp->weight);
            temp = temp->next;
        }
        printf("NULL\n");
    }
    printf("----------------------------\n");
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

// Louvain Helper Functions

int degree(Graph* graph, int v) {
    int d = 0;
    for (Node* e = graph->adjLists[v]; e; e = e->next) {
        d += e->weight;
    }
    return d;
}

int total_weight(Graph* graph) {
    int sum = 0;
    for (int i = 1; i < graph->numVertices; i++) {
        sum += degree(graph, i);
    }
    return sum;
}

void printCommunity(int* community, int len){
    for(int i = 1; i < len; i++){
        printf("%i, ", community[i]);
    }
    printf("\n");
}

int nodeDegree(Graph *graph, int v)
{
    int sum = 0;
    Node *temp = graph->adjLists[v];
    while (temp != NULL)
    {
        sum += temp->weight;
        temp = temp->next;
    }
    return sum;
}

double totalGraphWeight(Graph *graph)
{
    double sum = 0;
    for (int i = 1; i < graph->numVertices; i++)
    {
        Node *temp = graph->adjLists[i];
        while (temp != NULL)
        {
            sum += temp->weight;
            temp = temp->next;
        }
    }
    return sum / 2.0;
}

double communityWeight(Graph *graph, int *community, int targetCom)
{
    double sum = 0;

    for (int v = 1; v < graph->numVertices; v++)
    {
        if (community[v] == targetCom){
            sum += (double) nodeDegree(graph, v);
        }   
    }

    return sum;
}

double findKi(Graph *graph, int i, int *community, int targetCom)
{
    double sum = 0.0;
    Node *temp = graph->adjLists[i];
    while (temp != NULL)
    {
        if (community[temp->vertex] == targetCom){
            sum += temp->weight;
        }
        temp = temp->next;
    }
    return sum;
}

double deltaModualarity(Graph *graph, int *community, int vertex, int targetCom)
{
    double m2 = 2.0 * totalGraphWeight(graph);
    double ki = nodeDegree(graph, vertex);

    int oldCommunity = community[vertex];
    community[vertex] = 0;

    double kiTarget = findKi(graph, vertex, community, targetCom);
    double totTarget = communityWeight(graph, community, targetCom);
    double kiOld = findKi(graph, vertex, community, oldCommunity);
    double totOld = communityWeight(graph, community, oldCommunity);

    community[vertex] = oldCommunity;

    double dQ = ((kiTarget / m2) - ((ki * totTarget) / (m2 * m2))) - ((kiOld / m2) - ((ki * totOld) / (m2 * m2)));

    return dQ;
}

int CalcAij(Graph *graph, int v1, int v2)
{
    Node *temp = graph->adjLists[v1];
    while (temp != NULL)
    {
        if (temp->vertex == v2)
            return temp->weight;
        temp = temp->next;
    }
    return 0;
}

double modularity(Graph *graph)
{
    double Q = 0.0;
    double m = totalGraphWeight(graph);

    for (int i = 1; i < graph->numVertices; i++)
    {
        for (int j = 1; j < graph->numVertices; j++)
        {
            if (i == j)
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
    printf("Modularity: %lf\n", modularity(graph));
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

// Louvain Phase 1
void louvain(Graph* graph, int community[], int communityInternal[]) {
    int improvement = 1;
    // Run until there is no improvement
    while (improvement) {
        improvement = 0;

        // Find best improvement
        for (int node = 1; node < graph->numVertices; node++) {
            double bestGain = 0;
            int bestComm = communityInternal[node];

            Node* p = graph->adjLists[node];
            while (p) {
                double gain = deltaModualarity(graph, communityInternal, node, communityInternal[p->vertex]);
                if (gain > bestGain)
                {
                    bestGain = gain;
                    bestComm = communityInternal[p->vertex];
                }
                p = p->next;
            }

            if (bestComm != communityInternal[node])
            {
                improvement = 1;
                communityInternal[node] = bestComm;
            }
        }
    }

    // Reindex community for phase 2
    reIndexCommunity(community);
    for(int i = 1; i < N; i++){
        community[i] = communityInternal[community[i]];
    }

    return;
}

// Louvain Phase 2, Combine nodes and shrink the graph
Graph* louvainCollapse(Graph* graph, int community[]) {

    // Find size for new graph
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

    int newCount = newID;

    // Initialize new graph
    Graph* newGraph = createGraph(newCount);

    // Calculate Weights for new graph
    int weights[N][N];
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            weights[i][j] = 0;
        }
    }
   
    FILE* input = fopen("data.txt", "r");
    int inp1;
    int inp2;

    while (fscanf(input, "[%d %d]\n", &inp1, &inp2) == 2) {
        weights[map[community[inp1]]][map[community[inp2]]] += 1; 
        weights[map[community[inp2]]][map[community[inp1]]] += 1;
    }

    // Add edges to new array (other communities) 
    for (int i = 2; i < newCount; i++) {
        for (int j = 1; j < i; j++) {
            if (weights[i][j] > 0) {
                int addweight = (weights[i][j] + weights[j][i])/2;
                addEdge(newGraph, i, j, addweight);
            }
        }
    }

    // Add self loops for new array (community weight)
    for(int i = 1; i < newCount; i++){
        int addweight = (weights[i][i]);
        addEdge(newGraph, i, i, addweight);
    }

    freeGraph(graph);

    return newGraph;
}

// Louvain main loop
int mainLouvain(){
    
    // Initialize
    Graph* graph = readInput();
    int community[N];
    int len = N;

    for (int i = 0; i < N; i++) {
        community[i] = i;
    }

    // Run until no improvement(No shrinking)
    int run = 1;
    int count = 0;
    while (run) {
        
        int communityInternal[N];

        for (int i = 0; i < N; i++) {
            communityInternal[i] = i;
        }

        louvain(graph, community, communityInternal);
        graph = louvainCollapse(graph, community);

        if(graph->numVertices == len){
            run = 0;
        }

        count++;
        len = graph->numVertices;
    }

    reIndexCommunity(community);
    printOutput(graph, community, len);

    freeGraph(graph);

    return 0;
}

int main(){
    mainLouvain();

    return 0;
}