//
// Created by george on 28/10/23.
//

#ifndef SOCKETS_RULELIST_H
#define SOCKETS_RULELIST_H

#include "QueryList.h"
#include "Node.h"
#include "myBool.h"

typedef struct RuleList {
    Node* head;
    long line_count; // For streaming
} RuleList;

void make_rule_list(RuleList* rule_list);
void empty_rule_list(RuleList* rule_list);

void delete_list_node(Node* node);

void add_list_rule(RuleList *rule_list, char *ip_str, char *port_str);
bool delete_list_rule(RuleList *rule_list, char *ip_str, char *port_str);
bool check_packet(RuleList *rule_list, char *packet_ip, char *packet_port);

void stream_rules(RuleList *list, char *buffer);

#endif //SOCKETS_RULELIST_H
