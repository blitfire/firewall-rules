//
// Created by george on 28/10/23.
//
#include "QueryList.h"
#include "myBool.h"
#include "Rule.h"
#include "Node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void make_query_list(QueryList *list, char *ip_str, char *port_str) {
    make_rule(&list->head_rule, ip_str, port_str);
    list->head = NULL;
}

Node* new_query_node(char *str) {
    Node *new_node;
    if ((new_node= (Node *) malloc(sizeof(Node))) == NULL) {
        printf("Failed to create node: Allocation failed.");
        exit(-1);
    }
    if ((new_node->data= (char *) malloc(sizeof(char) * MAX_QUERY_LENGTH)) == NULL) {
        printf("Failed to create node: Allocation failed.");
        exit(-1);
    }
    strcpy(new_node->data, str);

    return new_node;
}

void delete_query_node(Node* node) {
    (node->prev)->next = node->next;
    (node->next)->prev = node->prev;
    free(node->data);
    free(node);
}

void add_query(QueryList *list, char *packet_ip, char *packet_port) {
    char query[MAX_QUERY_LENGTH];
    sprintf(query, "%s %s", packet_ip, packet_port);
    Node* new_node = new_query_node(query);
    if (list->head == NULL) {
        new_node->next = new_node;
        new_node->prev = new_node;
        list->head = new_node;
        list->length++;
        return;
    }

    Node* last_node = (list->head)->prev;
    last_node->next = new_node;
    new_node->prev = last_node;
    new_node->next = list->head;
    (list->head)->prev = new_node;
    list->length++;
}

void empty_queries(QueryList* list) {
    if (list->head != NULL) {
        while ((list->head)->prev != list->head) {
            delete_query_node((list->head)->prev);
        }
        delete_query_node(list->head);
        list->head = NULL;
    }
    list->length = 0;
}

void stream_query(QueryList *list, char *buffer) {
    char rule_str[MAX_RULE_LENGTH] = {};
    Node* curr = list->head;
    get_rule_string(&list->head_rule, rule_str);
    strcat(buffer, "Rule: ");
    strcat(buffer, rule_str);
    strcat(buffer, "\n");
    if (curr == NULL) return;
    do {
        strcat(buffer, "Query: ");
        strcat(buffer, (char *) curr->data);
        strcat(buffer, "\n");
        curr = curr->next;
    } while (curr != list->head);
}
