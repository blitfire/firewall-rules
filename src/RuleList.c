//
// Created by george on 28/10/23.
//

#include "RuleList.h"
#include "QueryList.h"
#include "myBool.h"
#include <stdlib.h>
#include <string.h>

void make_rule_list(RuleList* rule_list) {
    rule_list->head = NULL;
    rule_list->line_count = 0;
}

void empty_rule_list(RuleList* rule_list) {
    if (rule_list->head != NULL) {
        while ((rule_list->head)->prev != rule_list->head) {
            delete_list_node((rule_list->head)->prev);
        }
        delete_list_node(rule_list->head);
    }
    rule_list->head = NULL;
    rule_list->line_count = 0;
}

void add_list_rule(RuleList *rule_list, char *ip_str, char *port_str) {
    QueryList* new_list;
    if ((new_list= (QueryList *) malloc(sizeof(QueryList))) == NULL) {
        printf("Failed to create query list: Allocation failed.\n");
        exit(-1);
    }
    make_query_list(new_list, ip_str, port_str);
    Node *new_node;
    if ((new_node= (Node *) malloc(sizeof(Node))) == NULL) {
        printf("Failed to create node: Allocation failed.\n");
        exit(-1);
    }
    new_node->data = new_list;

    if (rule_list->head == NULL) {
        new_node->next = new_node;
        new_node->prev = new_node;
        rule_list->head = new_node;
        printf("First rule added.\n");
        rule_list->line_count++;
        return;
    }

    printf("Rule added.\n");
    Node* last_node = (rule_list->head)->prev;
    new_node->prev = last_node;
    new_node->next = rule_list->head;
    last_node->next = new_node;
    (rule_list->head)->prev = new_node;
    rule_list->line_count++;
}

bool delete_list_rule(RuleList *rule_list, char *ip_str, char *port_str) {
    Rule temp_rule;
    make_rule(&temp_rule, ip_str, port_str);

    Node* curr_rule = rule_list->head;
    if (curr_rule == NULL) return false;
    do {
        if (compare_rules(&((QueryList *) curr_rule->data)->head_rule, &temp_rule) == 0) {
            rule_list->line_count -= 1 + ((QueryList *) (curr_rule->data))->length;
            if (curr_rule == rule_list->head) {
                if (curr_rule->next != rule_list->head) rule_list->head = curr_rule->next;
                else rule_list->head = NULL;
            }
            delete_list_node(curr_rule);
            return true;
        }
        curr_rule = curr_rule->next;
    } while (curr_rule != rule_list->head);

    return false;
}

void delete_list_node(Node* node) {
    (node->prev)->next = node->next;
    (node->next)->prev = node->prev;
    empty_queries((QueryList *) node->data);
    free(node->data);
    free(node);
}

bool check_packet(RuleList *rule_list, char *packet_ip, char *packet_port) {
    bool matches = false;
    Node* curr = rule_list->head;
    if (curr == NULL) return false;
    do {
        matches = follows_rule(&((QueryList *) curr->data)->head_rule, packet_ip, packet_port);
        curr = curr->next;
    } while (curr != rule_list->head && !matches);
    if (!matches) return false;

    QueryList* queries = (curr->prev)->data;
    add_query(queries, packet_ip, packet_port);
    rule_list->line_count++;
    return true;
}

void stream_rules(RuleList *list, char *buffer) {
    Node* curr = list->head;
    if (curr == NULL) return;
    do {
        stream_query(curr->data, buffer);
        curr = curr->next;
    } while (curr != list->head);
}