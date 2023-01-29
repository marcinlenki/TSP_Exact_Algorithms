#include "Graph.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Graph::Graph() {
    verticesNum = 0;
    A = nullptr;
}

Graph::~Graph() {
    clear();
}

bool Graph::makeFromFile(const string& fileName) {
    int tempV, w;
    string line, space;
    ifstream inFile;
    inFile.open(fileName);

    if (!inFile.is_open()) {
        cout<<"Plik nie istnieje."<<endl;
        return false;
    }

    clear();

    inFile >> tempV;
    if (tempV <= 0) {
        cout<<"W pliku wykryto blednę dane."<<endl;
        return false;
    }

    verticesNum = tempV;
    A = new int * [verticesNum];
    for(int i = 0; i < verticesNum; i++) {
        A[i] = new int[verticesNum]{};
    }


    for (int i = 0; i < verticesNum; i++) {
        for (int j = 0; j < verticesNum; j++) {
            inFile >> w;
            A[i][j] = w;
        }
    }
    inFile>>w;


    if (inFile.eof()) {
        cout<<"Wczytywanie danych zakończone."<<endl;
    } else if(inFile.fail()) {
        cout<<"Wczytywanie danych przerwane, nie udało się wczytać pliku."<<endl;
        return false;
    } else {
        cout<<"Wczytywanie danych przerwane."<<endl;
        return false;
    }

    inFile.close();
    return true;
}


void Graph::generateRandomGraph(int vertices) {
    if(vertices <= 0) {
        cout<<"Wystąpił błąd, nie udało się stworzyć grafu."<<endl;
        return;
    }

    srand(time(nullptr));
    clear();

    verticesNum = vertices;
    A = new int * [verticesNum];
    for(int i = 0; i < verticesNum; i++) {
        A[i] = new int[verticesNum]{};
    }

    int w1, w2;
    for (int i = 0; i < verticesNum; i++) {
        for (int j = 0 + i; j < verticesNum; j++) {
            if (i == j) {
                A[i][j] = -1;
                continue;
            }

            w1 = (rand() % MAX_WEIGHT) + 1;
            w2 = (rand() % MAX_WEIGHT) + 1;
            A[i][j] = w1;
            A[j][i] = w2;
        }
    }

    cout<<"Z powodzeniem wygenerowane graf."<<endl;
}

void Graph::clear() {
    for(int i = 0; i < verticesNum; i++) {
        for(int j = 0; j < verticesNum; j++) {
            A[i][j] = 0;
        }

        delete [] A[i];
    }

    verticesNum = 0;
    delete [] A;
}

void Graph::show() {
    for (int i = 0; i < verticesNum; i++) {
        for (int j = 0; j < verticesNum; j++) {
            cout<<setw(4)<<A[i][j];
        }
        cout<<endl;
    }
    cout<<endl;
}