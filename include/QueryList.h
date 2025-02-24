//
// Created by george on 28/10/23.
//

#ifndef SOCKETS_QUERYLIST_H
#define SOCKETS_QUERYLIST_H
#include "Node.h"
#include "Rule.h"


// xxx.xxx.xxx.xxx
#define MAX_QUERY_LENGTH 21

typedef struct QueryList {
    Node* head;
    Rule head_rule;
    long length;
} QueryList;

void make_query_list(QueryList *list, char *ip_str, char *port_str);

Node *new_query_node(char *str);
void delete_query_node(Node* node);

void add_query(QueryList *list, char *packet_ip, char *packet_port);
void empty_queries(QueryList* list);
void stream_query(QueryList *list, char *buffer);

#endif //SOCKETS_QUERYLIST_H
