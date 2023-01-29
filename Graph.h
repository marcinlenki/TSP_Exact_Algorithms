#ifndef PROJEKT1_GRAPH_H
#define PROJEKT1_GRAPH_H
#include <string>

/**
 * Klasa reprezentująca graf. Przetrzymuje informacje o wierzchołkach oraz krawędziach między nimi za pomocą
 * macierzy sąsiedztwa. Ujemna wartość w macierzy oznacza brak połączenia między wierzchołkiem A i B.
 */
class Graph {
public:
    // Maksymalna waga krawędzi.
    const int MAX_WEIGHT = 100;

    int verticesNum;
    int** A;

public:
    Graph();
    ~Graph();
    bool makeFromFile(const std::string& fileName);

    /**
     * Funkcja służąca do generowania nowego grafu. Ze względu na rozpatrywany problem (ATSP),
     * domyślnie generowany jest pełny graf skierowany o podanej liczbie wierzchołków.
     *
     * @param vertices - liczba wierzchołków grafu, który ma zostać wygenerowany.
     */
    void generateRandomGraph(int vertices);
    void clear();
    void show();
};

#endif