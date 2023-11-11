//
// Created by george on 26/10/23.
//

#ifndef SOCKETS_RULE_H
#define SOCKETS_RULE_H
#include <stdio.h>
#include "myBool.h"

#define MAX_IP_LENGTH 31
#define MAX_PORT_LENGTH 11
#define MAX_RULE_LENGTH 50
#define MAX_PORT_VALUE 65535

typedef struct Rule {
    long ip_nums[2][4];
    long ports[2];
    bool ill_formed;
    unsigned int ip_count;
    unsigned int port_count;
} Rule;

void make_rule(Rule *rule, char *ip_str, char *port_str);
int compare_rules(const void* first, const void* second);
void get_rule_string(Rule* rule, char *out_str);
bool follows_rule(Rule* rule, char* packet_ip, char *packet_port);
void check_rule(Rule* rule);
void get_ip(Rule *rule, char *ip);
void get_ports(Rule *rule, const char *port_range);
bool check_port_range(Rule *rule);
bool check_ip(Rule *rule);

// utility
void ip_to_str(const long nums[], char *out_str);
bool is_ip_str(const char* ip_str);
bool is_port_str(const char* port_str);
#endif //SOCKETS_RULE_H
