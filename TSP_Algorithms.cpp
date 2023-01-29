#include "TSP_Algorithms.h"
#include <iostream>
#include <queue>
#include <iomanip>
#include <bitset>
using namespace std;

// declaring static variable
int TSP_Algorithms::numberOfVertices;

bool TSP_Algorithms::bruteForceTSP(const Graph *graph, int startVertex) {
    // start measuring time
    start = chrono::high_resolution_clock::now();

    bool timedOut = false;
    int numOfVertices = graph->verticesNum;
    path = new int [numOfVertices + 1];
    visited = new bool [numOfVertices];
    currentPath = new Stack(numOfVertices + 1);
    temp_d = 0;
    d = MAX_INTEGER;

    for (int i = 0; i < graph->verticesNum; i++)
        visited[i] = false;

    // Call to the recursive function that finds the optimal path
    timedOut = bruteForceTSP(graph, startVertex, startVertex);

    if (!timedOut) {
        for (int i = 0; i < numOfVertices + 1; i++) {
            if (i == numOfVertices)
                cout<<path[i]<<endl;
            else
                cout<<path[i]<<" --> ";
        }
        cout<<"Długość ścieżki: " << d <<endl;
    } else {
        cout<<"Timed out."<<endl;
    }

    delete[] path;
    delete[] visited;
    delete currentPath;
    return timedOut;
}

bool TSP_Algorithms::bruteForceTSP(const Graph *graph, int startVertex, int currentVertex) {
    // check for time out
    bool timedOut = false;
    stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    if ((duration.count() / 1000000000) > BRUTE_TIME_OUT) {
        return true;
    }

    int numOfVertices = graph->verticesNum;
    int** adjMatrix = graph->A;

    // add currently visited vertex to temporary path
    currentPath->push(currentVertex);

    // if all vertices have been visited
    if (currentPath->currentSize() == numOfVertices) {
        if (adjMatrix[currentVertex][startVertex] > 0) {
            temp_d += adjMatrix[currentVertex][startVertex];

            // check if the temporary path is better than current best path
            if (temp_d < d) {
                d = temp_d;
                currentPath->copyTo(path);
                path[numOfVertices] = startVertex;
            }

            temp_d -= adjMatrix[currentVertex][startVertex];
        }

    } else {
        // mark current vertex as visited
        visited[currentVertex] = true;

        // explore each child of currentVertex
        for (int u = 0; u < numOfVertices && !timedOut; u++) {
            if (adjMatrix[currentVertex][u] > 0 && !visited[u]) {
                temp_d += adjMatrix[currentVertex][u];
                timedOut = bruteForceTSP(graph, startVertex, u);    // recursive call
                temp_d -= adjMatrix[currentVertex][u];
            }
        }

        // after all of its children have been visited, mark current vertex as not visited
        visited[currentVertex] = false;
    }

    currentPath->pop();
    return timedOut;
}

int TSP_Algorithms::findMinRow(int** matrix, int size,  int rowNum) {
    int min = MAX_INTEGER;

    for (int i = 0; i < size; i++) {
        // return immediately if 0 is found
        if (matrix[rowNum][i] == 0)
            return 0;

        if (matrix[rowNum][i] > 0 && matrix[rowNum][i] < min) {
            min = matrix[rowNum][i];
        }
    }

    // all values less than 0
    if (min == MAX_INTEGER)
        min = 0;

    return min;
}

int TSP_Algorithms::findMinColumn(int **matrix, int size, int colNum) {
    int min = MAX_INTEGER;

    for (int i = 0; i < size; i++) {
        if (matrix[i][colNum] == 0)
            return 0;

        if (matrix[i][colNum] > 0 && matrix[i][colNum] < min) {
            min = matrix[i][colNum];
        }
    }

    if (min == MAX_INTEGER)
        min = 0;

    return min;
}

void TSP_Algorithms::subtractRow(int **matrix, int size, int rowNum, int numberToSubtract) {
    for (int i = 0; i < size; i++) {
        matrix[rowNum][i] -= numberToSubtract;
    }
}

void TSP_Algorithms::subtractColumn(int **matrix, int size, int colNum, int numberToSubtract) {
    for (int i = 0; i < size; i++)
        matrix[i][colNum] -= numberToSubtract;
}

int TSP_Algorithms::reduceMatrix(int **matrix, int size) {
    int rowMin, colMin, bound = 0;

    for (int i = 0; i < size; i++) {
        if ((rowMin = findMinRow(matrix, size, i)) != 0) {
            subtractRow(matrix, size, i, rowMin);
            bound += rowMin;
        }
    }

    for (int i = 0; i < size; i++) {
        if ((colMin = findMinColumn(matrix, size, i)) != 0) {
            subtractColumn(matrix, size, i, colMin);
            bound += colMin;
        }
    }

    return bound;
}

void TSP_Algorithms::alterMatrix(int **matrix, int size, int v, int u) {
    for (int i = 0; i < size; ++i) {
        matrix[v][i] = -1;
    }

    for (int i = 0; i < size; ++i) {
        matrix[i][u] = -1;
    }

    matrix[u][v] = -1;
}

void TSP_Algorithms:: printMatrix(int **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout<<setw(4)<<matrix[i][j];
        }
        cout<<endl;
    }
}

// Commented out lines of code are for debug purposes only.
bool TSP_Algorithms::LC_branchAndBound(const Graph* graph, int startVertex) {
    // start measuring time
    start = chrono::high_resolution_clock::now();

    bool pathFound = false, timedOut = false;
    int** adjMatrix = graph->A;
    numberOfVertices = graph->verticesNum;
    path = new int [numberOfVertices + 1];
    visited = new bool [numberOfVertices];
    for (int i = 0; i < numberOfVertices; i++)
        visited[i] = false;

    // Generate upper bound
    upperBound = generateUpperBound(graph, startVertex);

    // Minimum priority queue of Node*, sorted by bound value
    priority_queue<Node*, vector<Node*>, CompareNode> nodesPQ;

    // Create a copy of the original adjacency matrix and calculate initial bound
    int** modifiableCopy;
    copy(adjMatrix, modifiableCopy);
    int startingBound = reduceMatrix(modifiableCopy, numberOfVertices);     // Modifies the passed matrix

    Node* startNode = new Node(startVertex, startingBound, 0);
    startNode->reducedMatrix = modifiableCopy;
    startNode->visitedVertices = visited;
    startNode->visitedVertices[startVertex] = true;
    startNode->currentPath.push_back(startVertex);

    nodesPQ.push(startNode);

    while(!nodesPQ.empty()) {
        // Because of the implementation of the priority queue, the popped Node is always going to be the node with
        // the lowest lower bound (most promising).
        Node* node = nodesPQ.top();
        nodesPQ.pop();

        // if the optimal path is already found or the algorithm is timed out,
        // delete the rest of the added nodes
        if (pathFound || timedOut) {
            delete node;
            continue;
        }

        // check for time out
        stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
        if ((duration.count() / 1000000000) > BB_TIME_OUT) {
            timedOut = true;
            continue;
        }

        // check if path is found
        if (node->level == numberOfVertices - 1) {
            pathFound = true;

            for (int i = 0; i < numberOfVertices; i++) {
                path[i] = node->currentPath[i];
            }
            delete node;

            path[numberOfVertices] = startVertex;
            upperBound = node->bound;
            continue;
        }

        int** matrixCopy;

        // for each child u of Node node, check if its lower bound is better (lower) than generated upper bound
        // and if it is, add it to the queue.
        for (int u = 0; u < numberOfVertices; u++) {
            if (adjMatrix[node->vertexNum][u] > 0 && !node->visitedVertices[u]) {
                copy(node->reducedMatrix, matrixCopy);  // copy parents reduced matrix

                // Calculate lower bound of u
                alterMatrix(matrixCopy, numberOfVertices, node->vertexNum, u);
                int reduce = reduceMatrix(matrixCopy, numberOfVertices);
                int lowerBound = node->bound + reduce + (node->reducedMatrix)[node->vertexNum][u];

                if (lowerBound >= upperBound)
                    continue;

                Node* nextNode = new Node(u, lowerBound, node->level + 1);
                nextNode->reducedMatrix = matrixCopy;
                nextNode->visitedVertices = new bool [numberOfVertices];
                for (int i = 0; i < numberOfVertices; i++) {
                    nextNode->visitedVertices[i] = node->visitedVertices[i];
                }
                nextNode->visitedVertices[u] = true;
                nextNode->currentPath = node->currentPath;
                nextNode->currentPath.push_back(u);
                nodesPQ.push(nextNode);
            }
        }
        // delete the node after processing it
            delete node;
    }

    if (!timedOut) {
        for (int i = 0; i < numberOfVertices + 1; i++) {
            if (i == numberOfVertices)
                cout<<path[i]<<endl;
            else
                cout<<path[i]<<" --> ";
        }
        cout<<"Długość ścieżki: " << upperBound <<endl;
    } else {
        cout<<"Timed out."<<endl;
    }

    delete[] path;

    return timedOut;
}

bool TSP_Algorithms::DFS_branchAndBound(const Graph *graph, int startVertex) {
    // start measuring time
    start = chrono::high_resolution_clock::now();

    bool timedOut = false;
    int** adjMatrix = graph->A;
    numberOfVertices = graph->verticesNum;
    visited = new bool [numberOfVertices];
    for (int i = 0; i < numberOfVertices; i++)
        visited[i] = false;

    currentPath = new Stack(numberOfVertices + 1);
    path = new int [numberOfVertices + 1];

    tempUpperBound = 0;
    upperBound = generateUpperBound(graph, startVertex);    // Generate upper bound

    // Create a copy of the original adjacency matrix and calculate initial bound
    int** modifiableCopy;
    copy(adjMatrix, modifiableCopy);
    int bound = reduceMatrix(modifiableCopy, numberOfVertices);

    // Call to the recursive function that finds the optimal path
    timedOut = DFS_branchAndBound(graph, startVertex, startVertex,
                                  modifiableCopy, bound);

    if (!timedOut) {
        for (int i = 0; i < numberOfVertices + 1; i++) {
            if (i == numberOfVertices)
                cout<<path[i]<<endl;
            else
                cout<<path[i]<<" --> ";
        }
        cout<<"Długość ścieżki: " << upperBound <<endl;
    } else {
        cout<<"Timed out."<<endl;
    }

    delete[] path;
    delete visited;
    delete currentPath;
    return timedOut;
}

bool TSP_Algorithms::DFS_branchAndBound(const Graph *graph, int startVertex, int currentVertex, int **reducedParentMatrix,
                                        int parentBound) {

    // check for time out
    bool timedOut = false;
    stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    if ((duration.count() / 1000000000) > BB_TIME_OUT) {
        return true;
    }

    int** adjMatrix = graph->A;
    int** matrixCopy;

    currentPath->push(currentVertex);

    // update upper bound if all vertices have been visited
    if (currentPath->currentSize() == numberOfVertices) {
        if (adjMatrix[currentVertex][startVertex] > 0) {
            upperBound = parentBound;
            currentPath->copyTo(path);
            path[numberOfVertices] = startVertex;
        }

    } else {
        // mark current vertex as visited
        visited[currentVertex] = true;

        // for each child u of currentVertex, check if its lower bound is better (lower) than current upper bound
        // and if it is, explore the chosen child. If the algorithm is timed out the loop is exited.
        for (int u = 0; u < numberOfVertices && !timedOut; u++) {
            if (adjMatrix[currentVertex][u] > 0 && !visited[u]) {
                copy(reducedParentMatrix, matrixCopy);  // copy parents reduced matrix

                // Calculate lower bound of u
                alterMatrix(matrixCopy, numberOfVertices, currentVertex, u);
                int reduce = reduceMatrix(matrixCopy, numberOfVertices);
                int lowerBound = parentBound + reduce + reducedParentMatrix[currentVertex][u];

                if (lowerBound < upperBound) {
                    tempUpperBound += adjMatrix[currentVertex][u];
                    // recursive call
                    timedOut = DFS_branchAndBound(graph, startVertex, u, matrixCopy, lowerBound);
                    tempUpperBound -= adjMatrix[currentVertex][u];
                }
            }
        }

        // after all of its children have been visited, mark current vertex as not visited
        visited[currentVertex] = false;

        // delete the matrix after processing it
        for(int i = 0; i < numberOfVertices; i++) {
            delete [] matrixCopy[i];
        }
        delete[] matrixCopy;
    }

    currentPath->pop();
    return timedOut;
}

void TSP_Algorithms::copy(int **src, int ** &dst) {
    dst = new int * [numberOfVertices];
    for (int i = 0; i < numberOfVertices; i++) {
        dst[i] = new int [numberOfVertices]{};
    }

    for (int i = 0; i < numberOfVertices; i++) {
        for (int j = 0; j < numberOfVertices; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

int TSP_Algorithms::generateUpperBound(const Graph *graph, int startVertex) {
    int currentVertex = startVertex;
    int upper = 0, count = 0;
    path[count] = currentVertex;
    int** adjMatrix = graph->A;
    bool* tempVisited = new bool [numberOfVertices];

    for (int i = 0; i < numberOfVertices; i++)
        tempVisited[i] = false;

    while(count != numberOfVertices - 1) {
        for (int u = 0; u < numberOfVertices; u++) {
            if (adjMatrix[currentVertex][u] > 0 && !tempVisited[u]) {
                tempVisited[currentVertex] = true;
                upper += adjMatrix[currentVertex][u];
                currentVertex = u;
                path[++count] = currentVertex;
                break;
            }
        }
    }

    upper += adjMatrix[currentVertex][startVertex];
    path[numberOfVertices] = startVertex;

    delete[] tempVisited;
    return upper;
}

TSP_Algorithms::Node::Node(int vertexNum, int bound, int level) : vertexNum(vertexNum), bound(bound), level(level) {}

TSP_Algorithms::Node::~Node() {
    for(int i = 0; i < numberOfVertices; i++) {
        delete [] reducedMatrix[i];
    }

    delete[] visitedVertices;
    delete[] reducedMatrix;
}

void TSP_Algorithms::dynamicTSP(const Graph *graph, int startVertex) {
    numberOfVertices = graph->verticesNum;
    int numberOfSubsets = pow(2, numberOfVertices);

    D = new int* [numberOfSubsets];
    for (int i = 0; i < numberOfSubsets; i++)
        D[i] = new int [numberOfVertices];

    for (int i = 0; i < numberOfSubsets; i++) {
        for (int j = 0; j < numberOfVertices; j++) {
            D[i][j] = -1;
        }
    }

    P = new int* [numberOfSubsets];
    for (int i = 0; i < numberOfSubsets; i++)
        P[i] = new int [numberOfVertices];

    for (int i = 0; i < numberOfSubsets; i++) {
        for (int j = 0; j < numberOfVertices; j++) {
            P[i][j] = -1;
        }
    }

    int optimalPath = dynamicTSP(graph, startVertex, (1 << startVertex), startVertex);

    cout<<startVertex<<" --> ";
    printPath(startVertex, 1);
    cout<<startVertex<<endl;
    cout<<"Długość ścieżki: " << optimalPath <<endl;

    for (int i = 0; i < numberOfVertices; i++) {
        delete[] D[i];
    }
    delete [] D;

    for (int i = 0; i < numberOfVertices; i++) {
        delete[] P[i];
    }
    delete [] P;
}

// Commented out lines of code are for debug purposes only.
int TSP_Algorithms::dynamicTSP(const Graph *graph, int startVertex, int mask, int i) {
//    cout << "Wierzchołek: " << i << endl;
//    cout << "Maska:\t dziesiętnie: " << mask << ", binarnie: " << bitset<8>(mask)<<endl<<endl;

    int** adjMatrix = graph->A;
    int currentWeight = 0, minWeight = INT_MAX;
    int bestVertex;

    // Check if all vertices have been visited (visited vertex is a leaf)
    if (mask == (1 << numberOfVertices) - 1) {
//        cout << "Odwiedzono wszystkie miasta w tej ścieżce (wierzchołek jest liściem)" << endl;
//        cout << "\t Zwróć wartość krawędzi (" << i << ", " << startVertex <<") = " << adjMatrix[i][startVertex]<<endl << endl;
        P[mask][i] = -1;
        return adjMatrix[i][startVertex];
    }

    // Check if we already have solution
    if (D[mask][i] != -1) {
//        cout << "Rozwiązanie D[" << mask << "] [" << i << "] zostało wcześniej znalezione, więc możemy je wykorzystać." << endl << endl;
        return D[mask][i];
    }

    for (int u = 0; u < numberOfVertices; u++) {
        // Check if the vertex hasn't already been visited in a current path
        if ((mask & (1 << u)) == 0) {
//            cout << "Pobrany wierzchołek: " << u <<  endl;
//            cout << "Poziom rekurencji: " << poziomRekurencji << endl;

//            cout << "\tRekurencyjne wywołanie funkcji dla argumentów mask = (dziesiętnie): " << (mask | (1 << u)) << ", (binarnie): " << bitset<8>((mask | (1 << u)))
//                    << " oraz i = "<< u << endl << endl;
//            poziomRekurencji ++;

            // recursive call with new vertex added to the mask
            currentWeight = adjMatrix[i][u] + dynamicTSP(graph, startVertex, mask | (1 << u), u);

//            poziomRekurencji --;

            if (minWeight > currentWeight) {
//                cout << "\t Zapamiętaj wierzchołek " << u << "{" << poziomRekurencji << "} " << " jako najlepszy." << endl;
                minWeight = currentWeight;
                bestVertex = u;
            }
        }
    }

    P[mask][i] = bestVertex;
    return D[mask][i] = minWeight;
}

void TSP_Algorithms::printPath(int currentVertex, int oldMask) {
    if (P[oldMask][currentVertex] == -1)
        return;

    int nextVer = P[oldMask][currentVertex];
    int newMask = oldMask | (1 << nextVer);

    cout<<nextVer<<" --> ";;

    printPath(nextVer, newMask);
}