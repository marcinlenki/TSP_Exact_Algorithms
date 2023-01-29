#include "Stack.h"

Stack::Stack(int stackSize) {
    this->stackSize = stackSize;
    this->stackPointer = 0;
    this->arr = new int [stackSize];
}
// stack currentSize = 2;
// stack pointer = 0;

void Stack::push(int newVal) {
    if (stackPointer + 1 <= stackSize)
        arr[stackPointer++] = newVal;
}

int Stack::pop() {
    int returnVal = -1;

    if (stackSize != 0)
        returnVal = arr[stackPointer--];

    return returnVal;
}

int Stack::currentSize() {
    return stackPointer;
}

Stack::~Stack() {
    delete[] arr;
}

void Stack::copyTo(int *dest) {
    for (int i = 0; i < stackSize; i++) {
        dest[i] = arr[i];
    }
}
