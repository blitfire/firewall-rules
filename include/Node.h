//
// Created by george on 28/10/23.
//

#ifndef SOCKETS_NODE_H
#define SOCKETS_NODE_H
#include "Rule.h"

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

#endif //SOCKETS_NODE_H
