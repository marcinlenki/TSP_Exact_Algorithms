#ifndef PROJEKT1_STACK_H
#define PROJEKT1_STACK_H

/**
 * Naiwna implementacja stosu o stałej wielkości za pomocą dynamicznie alokowanej tablicy.
 */
class Stack {
private:
    int stackSize;
    int stackPointer;
    int* arr;

public:
    Stack(int stackSize);
    ~Stack();
    void push(int newVal);
    int pop();
    int currentSize();

    /**
     * Funkcja, która kopiuje zawartość stosu do wyznaczonej tablicy.
     * @param dest - tablica do której kopiowana jest zawartość stosu.
     */
    void copyTo(int *dest);
};


#endif