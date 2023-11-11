//
// Created by george on 01/11/23.
//
#include "RuleHandler.h"
#include "myBool.h"
#include "RuleList.h"
#include "Rule.h"

void start_rule_handler(RuleHandler* handler) {
//    open_rule_file(&handler->file, "rules.txt");
    make_rule_list(&handler->list);
//    char ip_buffer[MAX_IP_LENGTH];
//    char port_buffer[MAX_PORT_LENGTH];
//    while (read_rule_line(&handler->file, ip_buffer, port_buffer)) {
//        add_list_rule(&handler->list, ip_buffer, port_buffer);
//    }
}

void stop_rule_handler(RuleHandler* handler) {
//    close_rule_file(&handler->file);
    empty_rule_list(&handler->list);
}

void stream(RuleHandler *handler, char *buffer) {
    stream_rules(&handler->list, buffer);
}

void check(RuleHandler* handler, char* ip, char* port, char* status) {
    if (!check_input(ip, port)) {
        sprintf(status, "%s\n", INVALID_CHECK);
        return;
    }
    if (check_packet(&handler->list, ip, port)) {
        sprintf(status, "Connection accepted\n");
    } else sprintf(status, "Connection rejected\n");
}

void add_rule(RuleHandler *handler, char *ips, char *ports, char *status) {
    if (!check_input(ips, ports)) {
        sprintf(status, "%s\n", INVALID_ADD);
        return;
    }
    add_list_rule(&handler->list, ips, ports);
//    write_rule_line(&handler->file, ips, ports);
    sprintf(status, "Rule added\n");
}

void delete_rule(RuleHandler *handler, char *ips, char *ports, char *status) {
    if (!check_input(ips, ports)) {
        sprintf(status, "%s\n", INVALID_DELETE);
        return;
    }
    if (delete_list_rule(&handler->list, ips, ports)) { // && delete_rule_line(&handler->file, ips, ports)

        sprintf(status, "Rule deleted\n");
        return;
    }
    sprintf(status, "Rule not found\n");
}

bool check_input(char* ip, char* port) {
    Rule temp_rule;
    make_rule(&temp_rule, ip, port);
    return !(temp_rule.ill_formed);
}