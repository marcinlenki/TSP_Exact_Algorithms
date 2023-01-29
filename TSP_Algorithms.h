#ifndef PROJEKT1_TSP_ALGORITHMS_H
#define PROJEKT1_TSP_ALGORITHMS_H

#include "Graph.h"
#include "Stack.h"
#include <vector>
#include <chrono>
using namespace std;

const int MAX_INTEGER = 2147483647;
const int BB_TIME_OUT = 60;     // in seconds
const int BRUTE_TIME_OUT = 40;     // in seconds


class TSP_Algorithms {
private:

    // Structures used in Least Cost algorithm
    struct Node {
        int vertexNum;
        int bound;  // lower bound
        int level;  // Level in the tree diagram
        int** reducedMatrix;
        bool* visitedVertices;

        vector<int> currentPath;

        Node(int vertexNum, int bound, int level);
        ~Node();
    };

    struct CompareNode {
        bool operator() (Node* const& n1, Node* const& n2) {
            return n1->bound > n2->bound;
        }
    };

//    int poziomRekurencji = 0;

    static int numberOfVertices;

    int upperBound; // Current upper bound of a graph
    int tempUpperBound;

    int d;  // Optimal tour cost
    int* path;  // Array representing the optimal tour
    int temp_d; // Temporary path cost
    Stack* currentPath; // Temporary tour
    bool* visited;  // Array representing visited vertices
    int counter;

    // Variables used for measuring time
    chrono::time_point<chrono::steady_clock, chrono::duration<long long, ratio<1LL, 1000000000LL>>> start;
    chrono::time_point<chrono::steady_clock, chrono::duration<long long, ratio<1LL, 1000000000LL>>> stop;

    int** D;
    int** P;

    // Helper functions for reducing an array.
    int findMinRow(int** matrix, int size, int rowNum);
    int findMinColumn(int** matrix, int size, int colNum);
    void subtractRow(int** matrix, int size, int rowNum, int numberToSubtract);
    void subtractColumn(int** matrix, int size, int colNum, int numberToSubtract);

    // Function used to reduce matrix.
    // It finds and subtracts the smallest positive value from each row and column.
    int reduceMatrix(int** matrix, int size);
    void alterMatrix(int** matrix, int size, int v, int u);

    // Function that allocates dst matrix and copies src elements into it.
    void copy(int** src, int** &dst);

    // Function that generates upper bound in a graph by visiting its nearest neighbour.
    int generateUpperBound(const Graph* graph, int startVertex);

    // Recursive functions calculating and finding optimal tour from a graph
    bool bruteForceTSP(const Graph* graph, int startVertex, int currentVertex);

    bool DFS_branchAndBound(const Graph* graph, int startVertex, int currentVertex, int** reducedParentMatrix,
                            int parentBound);

    int dynamicTSP(const Graph* graph, int startVertex, int mask, int i);

    void printPath(int currentVertex, int oldMask);

    // for debug purposes
    void printMatrix(int **matrix, int size);

public:
    // Wrapper functions responsible for optimal tour from a graph
    bool bruteForceTSP(const Graph* graph, int startVertex);

    bool LC_branchAndBound(const Graph* graph, int startVertex);

    bool DFS_branchAndBound(const Graph* graph, int startVertex);

    void dynamicTSP(const Graph* graph, int startVertex);

};

#endif